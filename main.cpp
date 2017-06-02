#include <string>
#include <cstring>
#include <unistd.h>
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <iostream>
#include "routerInterface.h"
#include "cparser.h"
#include "cparser_token.h"
#include "cparser_tree.h"  

using namespace std;


void restoreExistedRuels()
{

    string restoreCmd (" sudo iptables-restore ");
    restoreCmd.append(IPTABLESCONFIGFILE);
    system( restoreCmd.c_str());
}



int main(int argc, char *argv[])
{

    cparser_t parser; 
    char *config_file = NULL;
    int ch = 0;
    int debug=0;

    memset(&parser, 0, sizeof(parser));

    while (-1 != (ch = getopt(argc, argv, "pic:d"))) {
        switch (ch) {
            case 'p':
                printf("pid = %d\n", getpid());
                break;
            case 'c':
                config_file = optarg;
                break;
            case 'd':
                debug = 1;
                break;
        }
    }
    if (!InitRouter())
    {
        cout << "InitRouter failed, please restart application!" <<endl;
        return -1;
    }
    

    //create router Thread
    //
    thread routerThread(RouterMainThread);
    routerThread.detach();


    parser.cfg.root = &cparser_root; 
    parser.cfg.ch_complete = '\t'; 
    parser.cfg.ch_erase = '\b'; 
    parser.cfg.ch_del = 127; 
    parser.cfg.ch_help = '?'; 
    parser.cfg.flags = (debug ? CPARSER_FLAGS_DEBUG : 0); 
    snprintf(parser.cfg.prompt, CPARSER_MAX_PROMPT, "%s#", "hzspec"); 

    cparser_io_config(&parser); 

    if (CPARSER_OK != cparser_init(&parser.cfg, &parser)) 
    { 
        cout << "Fail to initialize parser." << endl; 
        return -1; 
    } 

    if (config_file) 
    { 
        (void)cparser_load_cmd(&parser, config_file); 
    }    
    cparser_run(&parser); 



    //restore existed iptables rules
    ifstream existedRules (IPTABLESCONFIGFILE, ifstream::in);
    if(existedRules.good())
    {
        cout << "Restore existed rules" << endl;
        restoreExistedRuels();
    }
    return 0;

}
