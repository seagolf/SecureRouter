#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "cparser.h"
#include "cparser_priv.h"
#include "cparser_token.h"
#include "cparser_tree.h"

cparser_result_t
cparser_glue_version (cparser_t *parser)
{
    cparser_cmd_version(&parser->context);
    return CPARSER_OK;
}

cparser_result_t
cparser_glue_show_black_list (cparser_t *parser)
{
    cparser_cmd_show_black_list(&parser->context);
    return CPARSER_OK;
}

cparser_result_t
cparser_glue_add_new_device_mac (cparser_t *parser)
{
    cparser_macaddr_t mac_val;
    cparser_macaddr_t *mac_ptr = NULL;
    cparser_result_t rc;

    rc = cparser_get_macaddr(&parser->tokens[3], &mac_val);
    assert(CPARSER_OK == rc);
    mac_ptr = &mac_val;
    cparser_cmd_add_new_device_mac(&parser->context,
        mac_ptr);
    return CPARSER_OK;
}

cparser_result_t
cparser_glue_add_mac_to_black_list (cparser_t *parser)
{
    cparser_macaddr_t mac_val;
    cparser_macaddr_t *mac_ptr = NULL;
    cparser_result_t rc;

    rc = cparser_get_macaddr(&parser->tokens[1], &mac_val);
    assert(CPARSER_OK == rc);
    mac_ptr = &mac_val;
    cparser_cmd_add_mac_to_black_list(&parser->context,
        mac_ptr);
    return CPARSER_OK;
}

cparser_result_t
cparser_glue_remove_mac_from_black_list (cparser_t *parser)
{
    cparser_macaddr_t mac_val;
    cparser_macaddr_t *mac_ptr = NULL;
    cparser_result_t rc;

    rc = cparser_get_macaddr(&parser->tokens[1], &mac_val);
    assert(CPARSER_OK == rc);
    mac_ptr = &mac_val;
    cparser_cmd_remove_mac_from_black_list(&parser->context,
        mac_ptr);
    return CPARSER_OK;
}

cparser_result_t
cparser_glue_check_latest_version (cparser_t *parser)
{
    cparser_cmd_check_latest_version(&parser->context);
    return CPARSER_OK;
}

cparser_result_t
cparser_glue_upgrade (cparser_t *parser)
{
    cparser_cmd_upgrade(&parser->context);
    return CPARSER_OK;
}

cparser_result_t
cparser_glue_debug_level_loglevel (cparser_t *parser)
{
    char *loglevel_val;
    char **loglevel_ptr = NULL;
    cparser_result_t rc;

    rc = cparser_get_list(&parser->tokens[2], &loglevel_val);
    assert(CPARSER_OK == rc);
    loglevel_ptr = &loglevel_val;
    cparser_cmd_debug_level_loglevel(&parser->context,
        loglevel_ptr);
    return CPARSER_OK;
}

cparser_result_t
cparser_glue_exit (cparser_t *parser)
{
    cparser_cmd_exit(&parser->context);
    return CPARSER_OK;
}

cparser_node_t cparser_node_exit_eol = {
    CPARSER_NODE_END,
    0,
    cparser_glue_exit,
    "quit the application",
    NULL,
    NULL
};

cparser_node_t cparser_node_exit = {
    CPARSER_NODE_KEYWORD,
    0,
    "exit",
    NULL,
    NULL,
    &cparser_node_exit_eol
};

cparser_node_t cparser_node_debug_level_loglevel_eol = {
    CPARSER_NODE_END,
    0,
    cparser_glue_debug_level_loglevel,
    "debug ",
    NULL,
    NULL
};

cparser_list_node_t cparser_list_node_debug_level_loglevel_error = {
    NULL,
    "error"
 };

cparser_list_node_t cparser_list_node_debug_level_loglevel_warning = {
    &cparser_list_node_debug_level_loglevel_error,
    "warning"
 };

cparser_list_node_t cparser_list_node_debug_level_loglevel_info = {
    &cparser_list_node_debug_level_loglevel_warning,
    "info"
 };

cparser_node_t cparser_node_debug_level_loglevel = {
    CPARSER_NODE_LIST,
    0,
    &cparser_list_node_debug_level_loglevel_info,
    NULL,
    NULL,
    &cparser_node_debug_level_loglevel_eol
};

cparser_node_t cparser_node_debug_level = {
    CPARSER_NODE_KEYWORD,
    0,
    "level",
    NULL,
    NULL,
    &cparser_node_debug_level_loglevel
};

cparser_node_t cparser_node_debug = {
    CPARSER_NODE_KEYWORD,
    0,
    "debug",
    NULL,
    &cparser_node_exit,
    &cparser_node_debug_level
};

cparser_node_t cparser_node_upgrade_eol = {
    CPARSER_NODE_END,
    0,
    cparser_glue_upgrade,
    "upgrad",
    NULL,
    NULL
};

cparser_node_t cparser_node_upgrade = {
    CPARSER_NODE_KEYWORD,
    0,
    "upgrade",
    NULL,
    &cparser_node_debug,
    &cparser_node_upgrade_eol
};

cparser_node_t cparser_node_check_latest_version_eol = {
    CPARSER_NODE_END,
    0,
    cparser_glue_check_latest_version,
    "check latest version",
    NULL,
    NULL
};

cparser_node_t cparser_node_check_latest_version = {
    CPARSER_NODE_KEYWORD,
    0,
    "version",
    NULL,
    NULL,
    &cparser_node_check_latest_version_eol
};

cparser_node_t cparser_node_check_latest = {
    CPARSER_NODE_KEYWORD,
    0,
    "latest",
    NULL,
    NULL,
    &cparser_node_check_latest_version
};

cparser_node_t cparser_node_check = {
    CPARSER_NODE_KEYWORD,
    0,
    "check",
    NULL,
    &cparser_node_upgrade,
    &cparser_node_check_latest
};

cparser_node_t cparser_node_remove_mac_from_black_list_eol = {
    CPARSER_NODE_END,
    0,
    cparser_glue_remove_mac_from_black_list,
    "remove a device from black list",
    NULL,
    NULL
};

cparser_node_t cparser_node_remove_mac_from_black_list = {
    CPARSER_NODE_KEYWORD,
    0,
    "list",
    NULL,
    NULL,
    &cparser_node_remove_mac_from_black_list_eol
};

cparser_node_t cparser_node_remove_mac_from_black = {
    CPARSER_NODE_KEYWORD,
    0,
    "black",
    NULL,
    NULL,
    &cparser_node_remove_mac_from_black_list
};

cparser_node_t cparser_node_remove_mac_from = {
    CPARSER_NODE_KEYWORD,
    0,
    "from",
    NULL,
    NULL,
    &cparser_node_remove_mac_from_black
};

cparser_node_t cparser_node_remove_mac = {
    CPARSER_NODE_MACADDR,
    0,
    "<MACADDR:mac>",
    NULL,
    NULL,
    &cparser_node_remove_mac_from
};

cparser_node_t cparser_node_remove = {
    CPARSER_NODE_KEYWORD,
    0,
    "remove",
    NULL,
    &cparser_node_check,
    &cparser_node_remove_mac
};

cparser_node_t cparser_node_add_mac_to_black_list_eol = {
    CPARSER_NODE_END,
    0,
    cparser_glue_add_mac_to_black_list,
    "add a device to black list",
    NULL,
    NULL
};

cparser_node_t cparser_node_add_mac_to_black_list = {
    CPARSER_NODE_KEYWORD,
    0,
    "list",
    NULL,
    NULL,
    &cparser_node_add_mac_to_black_list_eol
};

cparser_node_t cparser_node_add_mac_to_black = {
    CPARSER_NODE_KEYWORD,
    0,
    "black",
    NULL,
    NULL,
    &cparser_node_add_mac_to_black_list
};

cparser_node_t cparser_node_add_mac_to = {
    CPARSER_NODE_KEYWORD,
    0,
    "to",
    NULL,
    NULL,
    &cparser_node_add_mac_to_black
};

cparser_node_t cparser_node_add_mac = {
    CPARSER_NODE_MACADDR,
    0,
    "<MACADDR:mac>",
    NULL,
    NULL,
    &cparser_node_add_mac_to
};

cparser_node_t cparser_node_add_new_device_mac_eol = {
    CPARSER_NODE_END,
    0,
    cparser_glue_add_new_device_mac,
    "add new device",
    NULL,
    NULL
};

cparser_node_t cparser_node_add_new_device_mac = {
    CPARSER_NODE_MACADDR,
    0,
    "<MACADDR:mac>",
    NULL,
    NULL,
    &cparser_node_add_new_device_mac_eol
};

cparser_node_t cparser_node_add_new_device = {
    CPARSER_NODE_KEYWORD,
    0,
    "device",
    NULL,
    NULL,
    &cparser_node_add_new_device_mac
};

cparser_node_t cparser_node_add_new = {
    CPARSER_NODE_KEYWORD,
    0,
    "new",
    NULL,
    &cparser_node_add_mac,
    &cparser_node_add_new_device
};

cparser_node_t cparser_node_add = {
    CPARSER_NODE_KEYWORD,
    0,
    "add",
    NULL,
    &cparser_node_remove,
    &cparser_node_add_new
};

cparser_node_t cparser_node_show_black_list_eol = {
    CPARSER_NODE_END,
    0,
    cparser_glue_show_black_list,
    "show black list",
    NULL,
    NULL
};

cparser_node_t cparser_node_show_black_list = {
    CPARSER_NODE_KEYWORD,
    0,
    "list",
    NULL,
    NULL,
    &cparser_node_show_black_list_eol
};

cparser_node_t cparser_node_show_black = {
    CPARSER_NODE_KEYWORD,
    0,
    "black",
    NULL,
    NULL,
    &cparser_node_show_black_list
};

cparser_node_t cparser_node_show = {
    CPARSER_NODE_KEYWORD,
    0,
    "show",
    NULL,
    &cparser_node_add,
    &cparser_node_show_black
};

cparser_node_t cparser_node_version_eol = {
    CPARSER_NODE_END,
    0,
    cparser_glue_version,
    "show software version",
    NULL,
    NULL
};

cparser_node_t cparser_node_version = {
    CPARSER_NODE_KEYWORD,
    0,
    "version",
    NULL,
    &cparser_node_show,
    &cparser_node_version_eol
};

cparser_node_t cparser_root = {
    CPARSER_NODE_ROOT,
    0,
    NULL,
    "Root node of the parser tree",
    NULL,
    &cparser_node_version
};

