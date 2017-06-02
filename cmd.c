#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdbool.h>
#include "cparser.h"
#include "cparser_priv.h"
#include "cparser_token.h"
#include "cparser_tree.h"

extern  bool InitRouter();
extern  void RouterMainThread();
extern  void AddNewDevice( char * devMacAddr);
extern  void ShowVersion();
extern  void ShowBlacklist();
extern  void AddDeviceToBlacklist(char *devMacAddr);
extern  void RemoveDeviceFromBlacklist(char *devMacAddr);
extern  void SetLogLevel(char *logLevel);

cparser_result_t cparser_cmd_version(cparser_context_t *context)
{
    assert(context);
    ShowVersion();

    return CPARSER_OK;
    


}
cparser_result_t cparser_cmd_add_new_device_mac(cparser_context_t *context,
    cparser_macaddr_t *mac_ptr)
{
    assert(context && mac_ptr);
    
    char macAddr[18];

    snprintf(macAddr, 18, "%02x:%02x:%02x:%02x:%02x:%02x",mac_ptr->octet[0],mac_ptr->octet[1],
            mac_ptr->octet[2], mac_ptr->octet[3],
            mac_ptr->octet[4], mac_ptr->octet[5] );

    AddNewDevice(macAddr);
    return CPARSER_OK;
}
cparser_result_t cparser_cmd_show_black_list(cparser_context_t *context)
{
    assert(context);
    ShowBlacklist();
    return CPARSER_OK;
}
cparser_result_t cparser_cmd_add_mac_to_black_list(cparser_context_t *context,
    cparser_macaddr_t *mac_ptr)

{
    assert(context && mac_ptr);

    char macAddr[18];

    snprintf(macAddr, 18, "%02x:%02x:%02x:%02x:%02x:%02x",mac_ptr->octet[0],mac_ptr->octet[1],
            mac_ptr->octet[2], mac_ptr->octet[3],
            mac_ptr->octet[4], mac_ptr->octet[5] );

    AddDeviceToBlacklist(macAddr);
    return CPARSER_OK;
}
cparser_result_t cparser_cmd_remove_mac_from_black_list(cparser_context_t *context,
    cparser_macaddr_t *mac_ptr)
{
    assert(context && mac_ptr);
    char macAddr[18];

    snprintf(macAddr, 18, "%02x:%02x:%02x:%02x:%02x:%02x",mac_ptr->octet[0],mac_ptr->octet[1],
            mac_ptr->octet[2], mac_ptr->octet[3],
            mac_ptr->octet[4], mac_ptr->octet[5] );

    RemoveDeviceFromBlacklist(macAddr);

    return CPARSER_OK;
}
cparser_result_t cparser_cmd_check_latest_version(cparser_context_t *context)
{
    printf("to be done!\n");
    return CPARSER_OK;
}
cparser_result_t cparser_cmd_upgrade(cparser_context_t *context)
{

    printf("to be done!\n");
    return CPARSER_OK;
}
cparser_result_t cparser_cmd_debug_level_loglevel(cparser_context_t *context,
    char **loglevel_ptr)
{
    
    assert(context && loglevel_ptr);
    SetLogLevel( *loglevel_ptr);
    return CPARSER_OK;
}

cparser_result_t cparser_cmd_exit(cparser_context_t *context)
{
    assert(context);
    return cparser_quit(context->parser);
}

