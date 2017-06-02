#ifndef __CPARSER_TOKEN_H__
#define __CPARSER_TOKEN_H__

#define CPARSER_MAX_NODE_TYPES (16)

/**
 * Parser node type.
 */
typedef enum {
    CPARSER_NODE_ROOT = 0,
    CPARSER_NODE_END,
    CPARSER_NODE_KEYWORD,
    CPARSER_NODE_STRING,
    CPARSER_NODE_UINT,
    CPARSER_NODE_UINT64,
    CPARSER_NODE_INT,
    CPARSER_NODE_INT64,
    CPARSER_NODE_HEX,
    CPARSER_NODE_HEX64,
    CPARSER_NODE_FLOAT,
    CPARSER_NODE_MACADDR,
    CPARSER_NODE_IPV4ADDR,
    CPARSER_NODE_FILE,
    CPARSER_NODE_LIST,
    CPARSER_MAX_NODES
} cparser_node_type_t;

/**
 * 48-bit MAC address.
 */
typedef struct {
    /** Six octet holding the MAC address */
    uint8_t octet[6];
} cparser_macaddr_t;

/**
 * \brief    Match function pointer.
 * \details  This function pointer is the prototype of all
 *           CLI Parser match functions.
 *
 * \param    token     Pointer to the token.
 * \param    token_len Number of valid characters in the token.
 * \param    node      The parse tree node to be matched against.
 *
 * \retval   is_complete Return 1 from if the token matches the entire
 *                       token; 0 otherwise.
 * \return   CPARSER_OK if it is a complete or partial match; 
 *           CPARSER_NOT_OK if it does match the node.
 */
typedef cparser_result_t (*cparser_match_fn)(const char *token, 
                                             const int token_len, 
                                             cparser_node_t *node, 
                                             int *is_complete);

/**
 * \brief    Completion function pointer.
 * \details  This function pointer is the prototype of all
 *           CLI Parser match functions.
 *
 * \param    parser    Pointer to the parser.
 * \param    node      Pointer to the current matching parse node.
 * \param    token     Pointer to the token.
 * \param    token_len Number of valid characters in the token.
 */
typedef cparser_result_t (*cparser_complete_fn)(cparser_t *parser,
                                                const cparser_node_t *node,
                                                const char *token,
                                                const int token_len);

/**
 * \brief    Get function pointer.
 * \details  This function pointer is the prototype of all CLI Parser 
 *           get functions.
 *
 * \param    token     Pointer to the token.
 *
 * \retval   val       Pointer to the returned parameter value.
 * \return   CPARSER_OK if succeeded; CPARSER_NOT_OK otherwise.
 */
typedef cparser_result_t (*cparser_get_fn)(const cparser_token_t *token,
                                           void *val);

extern cparser_match_fn    cparser_match_fn_tbl[CPARSER_MAX_NODE_TYPES];
extern cparser_complete_fn cparser_complete_fn_tbl[CPARSER_MAX_NODE_TYPES];
extern cparser_get_fn      cparser_get_fn_tbl[CPARSER_MAX_NODE_TYPES];

/********** Token match functions **********/
cparser_result_t cparser_match_root(const char *token, const int token_len,
                                    cparser_node_t *node, int *is_complete);
cparser_result_t cparser_match_end(const char *token, const int token_len,
                                   cparser_node_t *node, int *is_complete);
cparser_result_t cparser_match_keyword(const char *token, const int token_len,
                                       cparser_node_t *node, int *is_complete);
cparser_result_t cparser_match_string(const char *token, const int token_len,
                                      cparser_node_t *node, int *is_complete);
cparser_result_t cparser_match_uint(const char *token, const int token_len,
                                    cparser_node_t *node, int *is_complete);
cparser_result_t cparser_match_uint64(const char *token, const int token_len,
                                      cparser_node_t *node, int *is_complete);
cparser_result_t cparser_match_int(const char *token, const int token_len,
                                   cparser_node_t *node, int *is_complete);
cparser_result_t cparser_match_int64(const char *token, const int token_len,
                                   cparser_node_t *node, int *is_complete);
cparser_result_t cparser_match_hex(const char *token, const int token_len,
                                   cparser_node_t *node, int *is_complete);
cparser_result_t cparser_match_hex64(const char *token, const int token_len,
                                     cparser_node_t *node, int *is_complete);
cparser_result_t cparser_match_float(const char *token, const int token_len,
                                     cparser_node_t *node, int *is_complete);
cparser_result_t cparser_match_macaddr(const char *token, const int token_len,
                                       cparser_node_t *node, int *is_complete);
cparser_result_t cparser_match_ipv4addr(const char *token, const int token_len,
                                        cparser_node_t *node, int *is_complete);
cparser_result_t cparser_match_file(const char *token, const int token_len,
                                    cparser_node_t *node, int *is_complete);
cparser_result_t cparser_match_list(const char *token, const int token_len,
                                    cparser_node_t *node, int *is_complete);

/********** Token complete functions **********/
cparser_result_t cparser_complete_keyword(cparser_t *parser, const cparser_node_t *node,
                                          const char *token, const int token_len);
cparser_result_t cparser_complete_file(cparser_t *parser, const cparser_node_t *node,
                                       const char *token, const int token_len);
cparser_result_t cparser_complete_list(cparser_t *parser, const cparser_node_t *node,
                                       const char *token, const int token_len);

/********** Token get functions **********/
cparser_result_t cparser_get_string(const cparser_token_t *token, void *value);
cparser_result_t cparser_get_uint(const cparser_token_t *token, void *value);
cparser_result_t cparser_get_uint64(const cparser_token_t *token, void *value);
cparser_result_t cparser_get_int(const cparser_token_t *token, void *value);
cparser_result_t cparser_get_int64(const cparser_token_t *token, void *value);
cparser_result_t cparser_get_hex(const cparser_token_t *token, void *value);
cparser_result_t cparser_get_hex64(const cparser_token_t *token, void *value);
cparser_result_t cparser_get_float(const cparser_token_t *token, void *value);
cparser_result_t cparser_get_macaddr(const cparser_token_t *token, void *value);
cparser_result_t cparser_get_ipv4addr(const cparser_token_t *token, void *value);
cparser_result_t cparser_get_file(const cparser_token_t *token, void *value);
cparser_result_t cparser_get_list(const cparser_token_t *token, void *value);

#endif /* __CPARSER_MATCH_H__ */
