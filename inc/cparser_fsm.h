
#ifndef __CPARSER_FSM_H__
#define __CPARSER_FSM_H__

#include "cparser.h"

#define CUR_TOKEN(p) (&((p)->tokens[(p)->token_tos]))

/**
 * Reset all parser FSM states.
 *
 * \param    parser Pointer to the parser structure.
 *
 * \return   None.
 */
void cparser_fsm_reset(cparser_t *parser);

/**
 * Input a character to parser FSM.
 *
 * \param    parser Pointer to the parser structure.
 * \param    ch     Input character.
 *
 * \return   CPARSER_OK if succeeded; CPARSER_NOT_OK otherwise.
 */
cparser_result_t cparser_fsm_input(cparser_t *parser, char ch);

/**
 * Walk through all children of a node. Return a match node if one is found.
 *
 * \param    parser    Pointer to the parser structure.
 * \param    token     Pointer to the beginning of the token.
 * \param    token_len Length of the token.
 * \param    parent    Pointer to the parent node.
 *
 * \retval   match       Pointer to a node that matches the token.
 *                       If there are multiple matches, the highest priority match
 *                       is returned.
 * \retval   is_complete 1 if the token completely matches 
 * \return   Number of matches.
 */
int cparser_match(const cparser_t *parser, const char *token, const int token_len,
                  cparser_node_t *parent, cparser_node_t **match,
                  int *is_complete);

#endif /* __CPARSER_FSM_H__ */
