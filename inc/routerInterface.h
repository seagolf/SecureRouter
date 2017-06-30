#ifndef ROUTERINTERFACE_H
#define ROUTERINTERFACE_H

#include <curl/curl.h>
#include <string>
#include <mutex>


#define IPTABLESCONFIGFILE "/tmp/iptables.txt"
typedef struct 
{
  char *memory;
  size_t size;
}MemoryStruct;

typedef struct
{
    unsigned char macAddr[6];
}MacAddressT;


typedef enum
{
    DebugInfo,
    DebugWarning,
    DebugError
}DebugLevelT;

class RouterInterface
{

public:
    static RouterInterface *  Instance();

    bool SendQueryRequest(std::string apMacAddr);
    bool SendNewDevicePost(std::string devMacAddr);

    void Run();
    bool HandleQueryResponse(MemoryStruct getResponse);

    bool UpdateIptables(std::string macAddr, int opCode);

    void ShowVersion();
    void ShowBlacklist();
    void GetApMacAddr(std::string & apMacAddr);

    void SetDebugLevel(DebugLevelT level)
    {
        fDebugLevel = level;
    }

    DebugLevelT GetDebugLevel()
    {
        return fDebugLevel;
    }


private:
    RouterInterface();
    ~RouterInterface();
    std::mutex fMutex;
    static RouterInterface * pfSingleton;
    bool bfShutdown;
    std::string fVersion;
    DebugLevelT fDebugLevel; //0-info 1-warning 2-fatal
};


extern "C"
{
    bool InitRouter();
    void RouterMainThread();
    void AddNewDevice( char * devMacAddr);
    void ShowVersion();
    void ShowBlacklist();
    void AddDeviceToBlacklist(char * devMacAddr);
    void RemoveDeviceFromBlacklist(char * devMacAddr);
    void SetLogLevel(char * logLevel);
}

#endif
