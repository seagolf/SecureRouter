
#ifndef __CPARSER_H__
#define __CPARSER_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cparser_options.h"

/*
 * CLI Parser assumes the following types are defined: uint8_t, int8_t,
 * uint16_t, int16_t, uint32_t, int32_t. In many platforms (including
 * all platforms with GNU toolchain), stdint.h defines these types.
 * Thus, including this file is sufficient. If your platform does not 
 * have these types defined, you need to define them here.
 */
#include <stdint.h>

/*
 * This is to match Cisco CLI behavior. For example, if there is a
 * command "show crypto interfaces", one can just enter "sh cry int"
 * if no other command has the same prefix form.
 */
#define SHORTEST_UNIQUE_KEYWORD

/**
 * \brief    Parser API result code.
 */
typedef enum cparser_result_ {
    CPARSER_OK = 0,
    CPARSER_NOT_OK,
    CPARSER_ERR_INVALID_PARAMS, /**< Invalid input parameters to a call */
    CPARSER_ERR_NOT_EXIST,      /**< Requested object does not exist    */
    CPARSER_ERR_OUT_OF_RES,     /**< Requested resource is exhausted    */
    CPARSER_ERR_PARSE_ERR,      /**< Parse error                        */
    CPARSER_ERR_INCOMP_CMD,     /**< Incomplete command                 */
    CPARSER_MAX_RESULTS
} cparser_result_t;

/**
 * \struct   cparser_t
 * \brief    CLI parser structure.
 * \details  See cparser_ for detais.
 */
typedef struct cparser_ cparser_t;
typedef struct cparser_node_ cparser_node_t;

#include "cparser_line.h"
#include "cparser_io.h"

/**
 * \struct   cparser_context_t
 * \brief    A structure passed back in action function.
 * \details  It contains the parser where the command is accepted 
 *           and some cookie that can be set the caller.
 */
typedef struct {
    /** Pointer to the parser instance */
    cparser_t *parser;
    /** An array of opaque pointers for nested levels */
    void      *cookie[CPARSER_MAX_NESTED_LEVELS];
} cparser_context_t;

#define CPARSER_FLAGS_DEBUG        (1 << 0)

/**
 * Return the number of tokens in the cparser for a particular parsed command.
 */
#define CPARSER_NUM_TOKENS(p)      (p->token_tos)

/**
 * User input callback. This function is called after a user input 
 * is completed.
 */
typedef cparser_result_t (*cparser_input_cb)(cparser_t *conext,
                                             char *buf, int buf_size);

/**
 * \struct   cparser_cfg_
 * \brief    Contains all configurable parameters of a parser.
 */
typedef struct cparser_cfg_ {
    cparser_node_t  *root;
    char            ch_complete;
    char            ch_erase;
    char            ch_del;
    char            ch_help;
    char            prompt[CPARSER_MAX_PROMPT];
    int             fd;
    unsigned long   flags;

    /* I/O API */
    cparser_io_init_fn     io_init;
    cparser_io_cleanup_fn  io_cleanup;
    cparser_getch_fn       getch;
    cparser_printc_fn      printc;
    cparser_prints_fn      prints;
} cparser_cfg_t;

/**
 * \struct   cparser_token_t
 * \brief    A parsed token.
 */
typedef struct cparser_token_ {
    /** Index (in the line) of the beginning of the token */
    short          begin_ptr;
    short          token_len;  /**< Number of character in the token */
    char           buf[CPARSER_MAX_TOKEN_SIZE]; /**< Local copy of the token */
    /** 
     * Pointer to the parent node whose one of its children matches
     * this token.
     */
    cparser_node_t *parent;
    /**
     * Pointer to the parse node that matches this token. Note that this
     * field is only filled out when the token is pushed into the stack.
     */
    cparser_node_t *node;
} cparser_token_t;

/**
 * \brief    Parser FSM states.
 * \details  There are 3 possible states in parser FSM.
 */
typedef enum cparser_state_ {
    CPARSER_STATE_WHITESPACE = 0,
    CPARSER_STATE_TOKEN,
    CPARSER_STATE_ERROR,
    CPARSER_MAX_STATES
} cparser_state_t;

/**
 * \brief    CLI parser structrure.
 * \details  This structure contains all configuration and running states.
 *           of a parser instance.
 */
struct cparser_ {
    /** Parser configuration structure */
    cparser_cfg_t     cfg;
    /** Current nested level */
    int               root_level;
    /** Parse tree root node at different nested levels */
    cparser_node_t    *root[CPARSER_MAX_NESTED_LEVELS];
    /** Parser prompt at different nested levels */
    char              prompt[CPARSER_MAX_NESTED_LEVELS][CPARSER_MAX_PROMPT];
    /** Current node */
    cparser_node_t    *cur_node;

    /********** FSM states **********/
    cparser_state_t   state;       /**< Current state */
    short             token_tos;   /**< Token stack top pointer */
    short             current_pos; /**< Current cursor in the line */
    /** Last cursor position that is in a non-error state */
    short             last_good;
    /** Token stack */
    cparser_token_t   tokens[CPARSER_MAX_NUM_TOKENS]; /* parsed tokens */
    /** Privileged mode (1) or not (0) */
    int               is_privileged_mode;

    /********** Line buffering states **********/
    short             max_line;
    short             cur_line;
    cparser_line_t    lines[CPARSER_MAX_LINES];

    /** Flag indicating if the parser should continue to except input */
    int               done;   
    /** Context passed back to action function */ 
    cparser_context_t context;

    /********** User input **********/
    /** Pointer to the buffer provided by user for input */
    char              *user_buf;
    /** Size (in bytes) of the buffer provided by user */
    int               user_buf_size;
    /** Current number of bytes of the input */
    int               user_buf_count;
    /** Whether to echo user input */
    int               user_do_echo;
    /** Callback function when the input is complete */
    cparser_input_cb  user_input_cb;

    /********** Last executed command **********/
    /** Index to the line buffer that holds the command */
    int               last_line_idx;
    /** Result code of the command */
    cparser_result_t  last_rc;
    /** End node of the command. NULL if the command is invalid. */
    cparser_node_t    *last_end_node;
};

typedef cparser_result_t (*cparser_glue_fn)(cparser_t *parser);
typedef cparser_result_t (*cparser_token_fn)(char *token, int token_len,
                                             int *is_complete);

/**
 * \typedef  cparser_walker_fn
 * \brief    Walker function prototype used in cparser_walk().
 *
 * \param    parser Pointer to the parser structure.
 * \param    node   Pointer to the current node being walked.
 * \param    cookie An opaque pointer passed from cparser_walk().
 *
 * \return   CPARSER_OK to continue the walk; CPARSER_NOT_OK to abort.
 */
typedef cparser_result_t (*cparser_walker_fn)(cparser_t *parser, 
                                              cparser_node_t *node, void *cookie);

/**
 * \brief    Initialize a parser.
 *
 * \param    cfg Pointer to the parser configuration structure.
 *
 * \retval   parser Pointer to the initialized parser.
 * \return   CPARSER_OK if succeeded; CPARSER_NOT_OK if failed.
 */
cparser_result_t cparser_init(cparser_cfg_t *cfg, cparser_t *parser);

/**
 * \brief    Input a character to the parser.
 *
 * \param    parser  Pointer to the parser structure.
 * \param    ch      Character to be input.
 * \param    ch_type Character type.
 *
 * \return   CPARSER_OK if succeeded; CPARSER_NOT_OK if failed.
 */
cparser_result_t cparser_input(cparser_t *parser, char ch, cparser_char_t ch_type);

/**
 * \brief    Run the parser. 
 * \details  This function is a wrapper around cparser_input(). It first 
 *           calls cparser_io_init(). Then, it calls cparser_getch() and 
 *           feeds character into the parser until it quits.
 *
 * \param    parser Pointer to the parser structure.
 *
 * \return   CPARSER_OK if succeeded; CPARSER_NOT_OK if failed.
 */
cparser_result_t cparser_run(cparser_t *parser);

/**
 * \brief    Walk the parse tree in the parser.
 *
 * \param    parser  Pointer to the parser structure.
 * \param    pre_fn  Walker function that called before tranverse its children.
 * \param    post_fn Walker function that called after transvere its children.
 * \param    cookie  An opaque pointer that is passed back to the caller via
 *                   callback functions 'pre_fn' and 'post_fn'.
 *
 * \return   CPARSER_OK if succeeded; CPARSER_NOT_OK if failed.
 */
cparser_result_t cparser_walk(cparser_t *parser, cparser_walker_fn pre_fn,
                              cparser_walker_fn post_fn, void *cookie);

/**
 * \brief    Walk the parser tree and generate a list of all available commands.
 *
 * \param    parser Pointer to the parser structure.
 * \param    str    Pointer to a filter string. If it is NULL, all
 *                  commands in the parse tree are displayed. Otherwise,
 *                  only commands with keywords that contain 'str' as
 *                  a substring are displayed.
 *
 * \return   CPARSER_OK if succeeded; CPARSER_ERR_INVALID_PARAMS if 
 *           the parser structure is invalid.
 */
cparser_result_t cparser_help_cmd(cparser_t *parser, char *str);

/**
 * \brief    Load a command/config file to the parser. 
 * \details  A command/config file is just a text file with CLI commands. 
 *           (One command per line.) The only difference is that submode 
 *           is automatically exited if the indentation changes. This 
 *           behavior is the same as Cisco CLI.
 *
 * \param    parser   Pointer to the parser structure.
 * \param    filename Pointer to the filename.
 *
 * \return   CPARSER_OK if succeeded; CPARSER_ERR_INVALID_PARAMS
 *           if the input parameters are NULL; CPARSER_NOT_OK if the file
 *           cannot be opened.
 */
cparser_result_t cparser_load_cmd(cparser_t *parser, char *filename);

/**
 * \brief    Exit a parser session.
 * \details  This call causes the parser to exit and returns from 
 *           cparser_run().
 *
 * \param    parser - Pointer to the parser structure.
 *
 * \return   CPARSER_OK if succeeded; CPARSER_ERR_INVALID_PARAMS if failed.
 */
cparser_result_t cparser_quit(cparser_t *parser);

/**
 * \brief    Enter a submode.
 *
 * \param    parser Pointer to the parser structure.
 * \param    cookie A parameter that is passed back to each submode command
 *                  via cparser_context_t.
 * \param    prompt The new submode prompt.
 *
 * \return   CPARSER_OK if succeeded; CPARSER_ERR_INVALID_PARAMS if the input 
 *           parameters are invalid; CPARSER_NOT_OK if there too many levels 
 *           of submode already.
 */
cparser_result_t cparser_submode_enter(cparser_t *parser, void *cookie, 
                                       char *prompt);

/**
 * \brief    Leave a submode. 
 * \details  The previous mode context and prompt are automatically restored.
 *
 * \param    parser Pointer to the parser structure.
 *
 * \return   CPARSER_OK if succeeded; CPARSER_ERR_INVALID_PARAMS if the input 
 *           parameters are invalid; CPARSER_NOT_OK if the parser has not 
 *           entered any submode.
 */
cparser_result_t cparser_submode_exit(cparser_t *parser);

/**
 * \brief    Set the parser context cookie.
 *
 * \param    parser Pointer to the parser structure.
 * \param    context A parameter that is passed back via cparser_context_t
 *                   in top-level (non-submode).
 *
 * \return   CPARSER_OK if succeeded; CPARSER_ERR_INVALID_PARAMS if the input 
 *           parameters are invalid.
 */
cparser_result_t cparser_set_root_context(cparser_t *parser, void *context);

/**
 * \brief    Get the parser root context cookie.
 *
 * \param    parser Pointer to the parser structure.
 *
 * \retval   context Root context cookie.
 * \return   CPARSER_OK if succeeded; CPARSER_ERR_INVALID_PARAMS if the input 
 *           parameters are invalid.
 */
cparser_result_t cparser_get_root_context(cparser_t *parser, void **context);

/**
 * \brief    Set the privileged mode.
 *
 * \param    parser Pointer to the parser structure.
 * \param    enable 1 to enable privileged mode; 0 to disable.
 *
 * \return   CPARSER_OK if succeeded;
 *           CPARSER_ERR_INVALID_PARAMS if the input parameters are invalid.
 */
cparser_result_t cparser_set_privileged_mode(cparser_t *parser, int enable);

/**
 * \brief    Check whether we are in privileged mode.
 *
 * \param    parser Pointer to the parser structure.
 *
 * \return   1 if it is in privileged mode;
             0 otherwise (or 'parser' is NULL).
 */
int cparser_is_in_privileged_mode(const cparser_t *parser);

/**
 * \brief    Read a raw string from the user via the terminal.
 *
 * \param    parser   Pointer to the parser structure.
 * \param    prompt   Pointer to a prompt string. If NULL, nothing is printed.
 * \param    echoed   1 to echo the input; 0 otherwise.
 * \param    buf      Pointer to the buffer for user input.
 * \param    buf_size Size of the user input buffer.
 * \param    cb       Callback function that is called when input is complete.
 *
 * \return   CPARSER_OK if succeeded;
 *           CPARSER_ERR_INVALID_PARAMS if the input parameters are invalid.
 */
cparser_result_t cparser_user_input(cparser_t *parser, const char *prompt,
                                    int echoed, char *buf, int buf_size,
                                    cparser_input_cb cb);

/**
 * Abort an outstanding user input. This is used when a terminal cannot 
 * unechoed input.
 * 
 * \param    parser   Pointer to the parser structure.
 *
 * \return   CPARSER_OK if succeeded;
 *           CPARSER_ERR_NOT_EXIST if the parser is not expecting user input;
 *           CPARSER_ERR_INVALID_PARAMS if the input parameter is invalid.
 */
cparser_result_t cparser_abort_user_input(cparser_t *parser);

/**
 * \brief    Return whether the parser is expected in a user input or 
 *           a command.
 *
 * \param    parser   Pointer to the parser structure.
 *
 * \retval   do_echo  1 if the input is echoed; 0 otherwise.
 * \return   1 if it is expecting user input;
 *           0 if it is expecting commands.
 */
int cparser_is_user_input(cparser_t *parser, int *do_echo);

/**
 * \brief    Get information about the last executed command.
 *
 * \param    parser   Pointer to the parser structure.
 *
 * \retval   cmd      Pointer to the command string.
 * \retval   rc       The result code.
 * \retval   is_priv  1 if the command is privileged; 0 otherwise. Only 
 *                    meaningful if the command is valid.
 */
cparser_result_t cparser_last_command(cparser_t *parser, char **cmd,
                                      cparser_result_t *rc, int *is_priv);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CPARSER_H__ */
