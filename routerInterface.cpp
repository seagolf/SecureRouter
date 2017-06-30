#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <mutex>

#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>


#include "routerInterface.h"

using namespace std;

RouterInterface *RouterInterface::pfSingleton = NULL;

RouterInterface::RouterInterface()
    :bfShutdown(false),
    fVersion("0.0.1"),
    fDebugLevel(DebugError)

{

    curl_global_init(CURL_GLOBAL_ALL);

}


RouterInterface::~RouterInterface()
{
}


RouterInterface *  RouterInterface::Instance()
{
    //create the sigleton instance
    if(pfSingleton == NULL)
    {
       pfSingleton = new RouterInterface();
    }

    return pfSingleton;
}




void RouterInterface::Run()
{

    while (!bfShutdown)
    {
        sleep(20);

#ifdef DEBUG
        string apMacAddr("FC:F5:28:D4:81:AA");
#else
        string apMacAddr;
        GetApMacAddr(apMacAddr);
#endif
        

        //send http GET  and handle reponse message
        if (!SendQueryRequest(apMacAddr))
        {
            cout << "sendQueryRequest failed" <<endl;
        }


    }

}
static
size_t IptablesOperateCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  MemoryStruct *mem = (MemoryStruct *)userp;
 
  mem->memory = (char *) realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
      /* out of memory! */ 
      printf("not enough memory (realloc returned NULL)\n");
      return 0;
    }
 
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}


bool RouterInterface::SendQueryRequest(string apMacAddr)
{

    //init the curl session 
    
    CURL  *pCurl;
    CURLcode res;

    pCurl = curl_easy_init();

    if(pCurl == NULL)
    {
        cout << " Error curl instance has not been created" << endl;
        return false;
    }

    
    string url("http://60.205.212.99/squirrel/v1/devices/ap_interval?apMacAddr=");

    for(int i = 0; i < 18; i+=3)
    {
        string tmpString = apMacAddr.substr(i,2);
        url.append(tmpString);

        if( i < 15)
        {
            url.append("%3A");    
        }
    
    }


    

    MemoryStruct getResponse;
    getResponse.memory = (char*) malloc(1);  // will be grown as needed by the realloc above 
    getResponse.size = 0;    // no data at this point 

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Accept: */*");

#ifdef DEBUG
    curl_slist_append(headers, "Authorization:");

#endif

    curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(pCurl, CURLOPT_URL, url.c_str());


    //register callback
    curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, IptablesOperateCallback);
    curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void *) &getResponse);

    //some servers don't like requests that are made without a user-agent field, so we provide one  
    curl_easy_setopt(pCurl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    res = curl_easy_perform(pCurl);

    // check for errors
    if(res != CURLE_OK)
    {
        cout<<"ERROR: curl_easy_perform failed" << curl_easy_strerror(res) << endl;
    }
    else
    {
        //Now, our chunk.memory points to a memory block that is chunk.size
        //bytes big and contains the remote file.

        if(CURLE_OK == res) 
        {
            long response_code;
            curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &response_code);

            if( response_code != 200)
            {
                cout <<" Error! response code: " << (int) response_code << endl;
                return false;
            }

            char *ct;
             //ask for the content-type  
            res = curl_easy_getinfo(pCurl, CURLINFO_CONTENT_TYPE, &ct);

            if((CURLE_OK == res) && ct)
            {
                if (fDebugLevel == DebugInfo)
                {
                    cout << "We received Content-Type: " << ct << endl;;
                }
            }
            HandleQueryResponse (getResponse);
        }


    }

    curl_easy_cleanup(pCurl);

}

bool RouterInterface::HandleQueryResponse(MemoryStruct getResponse)
{

    //clean all iptables for avoid duplicated rules
    //system("sudo iptables -F");
    char *iptablesEntry;

    //get each iptables entry, format:  AB:CD:EF:00:00:00-1
    iptablesEntry = strtok(getResponse.memory, ",");
    while(iptablesEntry != NULL)
    {
        string tableString(iptablesEntry);

        if (fDebugLevel == DebugInfo)
        {
            cout << "iptables Entry:" << tableString << endl; 
        }
        std::size_t dashPosition = tableString.find("-");
        string address = tableString.substr(0, dashPosition);

        if (fDebugLevel == DebugInfo)
        {
            cout<< "---> address: " << address << endl;
        }

        int opCode= stoi (tableString.substr(dashPosition+1));
        //string operation = (opCode== "1") ? "ACCEPT":"DROP" ;

        if (fDebugLevel == DebugInfo)
        {
            cout<< "---> action:" << opCode<< endl;
        }

        if(! UpdateIptables(address, opCode))

        {
            cout << " UpdateIptables failed: " << address << " opCode: " << opCode << endl;
        
        }


        iptablesEntry = strtok(NULL, ",");


    }

}


void RouterInterface::GetApMacAddr(string & apMacAddr)
{

    ostringstream targetMacAddr;
    string ifname = "eno1";
    unsigned char *pApMacAddr = NULL;
    struct ifreq s;

    memset(&s, 0, sizeof(s));

    s.ifr_addr.sa_family = AF_INET;
    strncpy(s.ifr_name, ifname.c_str(), IFNAMSIZ-1);


    int fd = socket (AF_INET, SOCK_DGRAM, 0);

    if (0 == ioctl (fd, SIOCGIFHWADDR, &s))
    {
        pApMacAddr = (unsigned char * ) s.ifr_hwaddr.sa_data;
        for (int i = 0; i < 6; i++)
        {
        
            targetMacAddr << setfill('0') << setw(2) << hex << (int) pApMacAddr[i];
            if (i != 5)
            {
                targetMacAddr <<":";
            }

        }

        apMacAddr = targetMacAddr.str();

        if (fDebugLevel == DebugInfo)
        {
            cout << "apMacAddr : " <<apMacAddr << endl; 
        }
    }

    close(fd);


}

bool RouterInterface::SendNewDevicePost(string devMacAddr)
{
    CURL *pCurl;
    CURLcode res;

    pCurl = curl_easy_init();

    if (NULL == pCurl)
    {
        cout << "faile to init curl" << endl;
        return false;
    }

    string url("http://60.205.212.99/squirrel/v1/devices/add_device_to_user");

    string apMacAddr;

    
    if (fDebugLevel == DebugInfo)
    {
        cout << "URL: " << url << endl;
    }

    GetApMacAddr(apMacAddr);

    string jsonObj("{\"apMacAddr\" : \"");
    
    jsonObj.append (apMacAddr);
    jsonObj.append ("\","); 
    jsonObj. append (" \"deviceMacAddr\" : \"");
    jsonObj.append (devMacAddr);
    jsonObj.append ("\",");
    jsonObj.append (" \"hostName\" : \"hzspec\", \
            \"ip\" : \"192.168.1.2\", \
            \"os\" : \"mac\", \
            \"vendor\" : \"apple\" \
            }");

    
    if (fDebugLevel == DebugInfo)
    {
        cout << "jsonObj: " << jsonObj  <<endl;
    }

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charsets: utf-8");




    curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(pCurl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(pCurl, CURLOPT_POST, 1L);
    curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, jsonObj.c_str()); 


    //register callback

    //some servers don't like requests that are made without a user-agent field, so we provide one  
    curl_easy_setopt(pCurl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    res = curl_easy_perform(pCurl);

    // check for errors
    if(res != CURLE_OK)
    {
        cout<<"ERROR: curl_easy_perform failed" << curl_easy_strerror(res) << endl;
    }
    else
    {
        //Now, our chunk.memory points to a memory block that is chunk.size
        //bytes big and contains the remote file.

        if(CURLE_OK == res) 
        {
            long response_code;
        
            curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &response_code);

            if( response_code != 200)
            {
                cout <<" Error! response code:"  << (int) response_code << endl;
            }

            char *ct;
             //ask for the content-type  
            res = curl_easy_getinfo(pCurl, CURLINFO_CONTENT_TYPE, &ct);

            if((CURLE_OK == res) && ct)
            {
                if (fDebugLevel == DebugInfo)
                {
                    cout << "We received Content-Type:" <<  ct <<endl;
                }
            }
        }
    }

    curl_easy_cleanup(pCurl);
}

bool RouterInterface::UpdateIptables(string address, int opCode) 
{
    fMutex.lock();
    ifstream iptablesFile;
    string iptablesEntry;

    iptablesFile.open(IPTABLESCONFIGFILE, std::ifstream::in);

    bool bFound = false;
    bool bNeedUpdate = false;

    string opStr = (opCode == 1) ? "ACCEPT" : "DROP";
    while (getline(iptablesFile, iptablesEntry))

    {

        //a exist device mac
        if (string::npos != iptablesEntry.find(address))
        {
            size_t actionPos = iptablesEntry.find("-j ");   
            if(string::npos != actionPos)
            {
                actionPos +=3;
                string prevAction = iptablesEntry.substr(actionPos);
                

                if(prevAction != opStr)
                {
                    ostringstream replaceCmd;
                    if (opCode != 2)
                    {
                        replaceCmd << "sudo iptables -C FORWARD -m mac --mac-source ";
                    }
                    else if (opCode == 2)
                    {
                        replaceCmd << "sudo iptables -A INPUT -s";
                    }

                    else 
                    {
                        cout << "opCode Error" << endl;
                        return false;
                    }
                    replaceCmd << address << "-j " << opStr;

                    if (fDebugLevel == DebugInfo)
                    {
                        cout << replaceCmd.str() << endl;
                    }
                    system(replaceCmd.str().c_str());

                    bFound = true;
                    bNeedUpdate = true;
                    break;


                }
                else
                {
                
                    bFound = true;
                    bNeedUpdate = false;

                    if (fDebugLevel == DebugInfo)
                    {
                        cout <<iptablesEntry << " : entry already exists!" << endl;
                    }
                }

            }
            else
            {
            
                cout << " current rule error!" << endl;
            
            }

        }

    }

    //a new entry
    if( !bFound )
    {
        ostringstream addNewEntry;

        if (opCode != 2)
        {
            addNewEntry << "sudo iptables -A FORWARD -m mac --mac-source " << address << " -j ";
        }
        else
        {
        
            addNewEntry << "sudo iptables -A INPUT -s " << address << " -j ";

        }
        
        addNewEntry << opStr  << endl;

        system(addNewEntry.str().c_str());

        bNeedUpdate = true;

    
    }
    if (bNeedUpdate)
    {
        ostringstream saveCmd;
        saveCmd << "sudo iptables-save > " << IPTABLESCONFIGFILE ;

        system(saveCmd.str().c_str());
     }
    

    iptablesFile.close();
    fMutex.unlock();

    return true;

}


void RouterInterface::ShowVersion()
{
    cout << "===================" << endl;
    cout << "software version " << fVersion << endl;
    cout << "===================" << endl;

}


void RouterInterface::ShowBlacklist()
{
    fMutex.lock();

    int index  = 0;
    string iptablesEntry;
    ifstream  iptablesFile;
    cout << "Blacklist:" << endl;
    

    iptablesFile.open(IPTABLESCONFIGFILE, std::ifstream::in);


    while (getline(iptablesFile, iptablesEntry))

    {

        //a exist device mac
        if (string::npos != iptablesEntry.find("DROP"))
        {
            size_t macPos= iptablesEntry.find("--mac-source ");   
            if(string::npos != macPos)
            {
                index ++;
                macPos +=13;
                string macAddr = iptablesEntry.substr(macPos, 17);
                cout << "\t #" << index << " : "  << macAddr << endl;
            }

        }

    }


    iptablesFile.close();
    fMutex.unlock();

}




extern "C"
{
    bool InitRouter()
    {
        
        if (NULL == RouterInterface::Instance())
        {
            return false;
        }
    
        return true;
    }


    void RouterMainThread()
    {
        RouterInterface::Instance()->Run();
    
    }

    void AddNewDevice( char *devMacAddr)
    {
        string macAddr (devMacAddr);
        if (RouterInterface::Instance()->GetDebugLevel() == DebugInfo)
        {
            cout << "mac Address: " << macAddr << endl;
        }
        RouterInterface::Instance()->SendNewDevicePost(devMacAddr);

    }

    void ShowVersion()
    {

        RouterInterface::Instance()->ShowVersion();
    }

    void ShowBlacklist()
    {
        RouterInterface::Instance()->ShowBlacklist();
    }


    void AddDeviceToBlacklist(char *devMacAddr)
    {

        string macAddr(devMacAddr);

        if (RouterInterface::Instance()->GetDebugLevel() == DebugInfo)
        {
            cout << "mac Address: " << macAddr << endl;
        }
        if (! RouterInterface::Instance()->UpdateIptables(macAddr, 0))
        {
            cout << " AddDeviceToBlacklist failed " <<endl;
        
        }
    }
    void RemoveDeviceFromBlacklist(char *devMacAddr)
    {

        string macAddr(devMacAddr);

        if (RouterInterface::Instance()->GetDebugLevel() == DebugInfo)
        {
            cout << "mac Address: " << macAddr << endl;
        }

        if (! RouterInterface::Instance()->UpdateIptables(macAddr, 1))
        {
            cout << " RemovesDeviceFromBlacklist failed " <<endl;
        
        }
    }


    void SetLogLevel(char * logLevl)
    {
        if(NULL == logLevl)
        {
            cout << "error log level not valid" << endl;
        }

        if (!strncmp(logLevl, "info", 4))
        {
            RouterInterface::Instance()->SetDebugLevel(DebugInfo);
        }

        else if (!strncmp(logLevl, "warning", 7))
        {
                RouterInterface::Instance()->SetDebugLevel(DebugWarning);
        }
        else if (!strncmp(logLevl, "error", 5))
        {
            RouterInterface::Instance()-> SetDebugLevel(DebugError);
        }

        else 
        {
            RouterInterface::Instance()-> SetDebugLevel(DebugError);
        }
    }

}
