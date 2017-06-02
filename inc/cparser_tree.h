#ifndef __CPARSER_TREE_H__
#define __CPARSER_TREE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern cparser_node_t cparser_root;

cparser_result_t cparser_cmd_version(cparser_context_t *context);
cparser_result_t cparser_cmd_show_black_list(cparser_context_t *context);
cparser_result_t cparser_cmd_add_new_device_mac(cparser_context_t *context,
    cparser_macaddr_t *mac_ptr);
cparser_result_t cparser_cmd_add_mac_to_black_list(cparser_context_t *context,
    cparser_macaddr_t *mac_ptr);
cparser_result_t cparser_cmd_remove_mac_from_black_list(cparser_context_t *context,
    cparser_macaddr_t *mac_ptr);
cparser_result_t cparser_cmd_check_latest_version(cparser_context_t *context);
cparser_result_t cparser_cmd_upgrade(cparser_context_t *context);
cparser_result_t cparser_cmd_debug_level_loglevel(cparser_context_t *context,
    char **loglevel_ptr);
cparser_result_t cparser_cmd_exit(cparser_context_t *context);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CPARSER_TREE_H__ */
