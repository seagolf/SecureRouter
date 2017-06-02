
#ifndef __CPARSER_IO_H__
#define __CPARSER_IO_H__

/**
 * Character type return by cparser_getch().
 */
typedef enum {
    CPARSER_CHAR_UNKNOWN = 0, /**< Character that cannot be classified */
    CPARSER_CHAR_REGULAR,     /**< All alpha-numeric + punctuation charcters */
    CPARSER_CHAR_UP_ARROW,    /**< Up arrow (recall previous command) */
    CPARSER_CHAR_DOWN_ARROW,  /**< Down arrow (recall next command) */
    CPARSER_CHAR_LEFT_ARROW,  /**< Left arrow (prev. character in the command) */
    CPARSER_CHAR_RIGHT_ARROW, /**< Right arrow (next character in the command) */
    CPARSER_CHAR_FIRST,       /**< Go to the first character in the command */
    CPARSER_CHAR_LAST,        /**< Go to the first character in the command */
    CPARSER_MAX_CHAR
} cparser_char_t;

/**
 * Configure all I/O functions to their default for a platform.
 *
 * \param    parser Pointer to the parser structure.
 */
void cparser_io_config(cparser_t *parser);

/**
 * Initialize I/O interface to the parser.
 *
 * \param    parser Pointer to the parser structure.
 */
typedef void (*cparser_io_init_fn)(cparser_t *parser);

/**
 * Cleanup I/O interface to the parser.
 *
 * \param    parser Pointer to the parser structure.
 */
typedef void (*cparser_io_cleanup_fn)(cparser_t *parser);

/**
 * Get a character input from the keyboard.
 *
 * \param    parser Pointer to the parser structure.
 *
 * \retval   ch   Pointer to the returned character code.
 * \retval   type Type of the returned character. If it is CPARSER_CHAR_REGULAR,
 *                *ch holds the ASCII code.
 * \return   None.
 */
typedef void (*cparser_getch_fn)(cparser_t *parser, int *ch, 
                                 cparser_char_t *type);

/**
 * Print a single character to the output file descriptor.
 *
 * \param   parser Pointer to the parser structure.
 * \param   ch     Character to be printed.
 */
typedef void (*cparser_printc_fn)(const cparser_t *parser, const char ch);

/**
 * Print a character string to the output file descriptor.
 *
 * \param   parser Pointer to the parser structure.
 * \param   s      Pointer to the string to be printed.
 */
typedef void (*cparser_prints_fn)(const cparser_t *parser, const char *s);

#endif /* __CPARSER_IO_H__ */
