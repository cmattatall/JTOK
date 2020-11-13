#ifndef JTOK_H_
#define JTOK_H_

#ifndef __JTOK_H_
#define __JTOK_H_
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

#define JTOK_STANDALONE_TOKENS

/**
 * JTOK type identifier. Basic types are:
 *  - Object
 *  - Array
 *  - String
 *  - Other primitive: number, boolean (true/false) or null
 */
typedef enum
{
    JTOK_PRIMITIVE = 0,
    JTOK_OBJECT    = 1,
    JTOK_ARRAY     = 2,
    JTOK_STRING    = 3
} jtoktype_t;

typedef enum
{
    /* Not enough tokens were provided */
    JTOK_ERROR_NOMEM = -1,

    /* Invalid character inside JTOK string */
    JTOK_ERROR_INVAL = -2,

    /* The string is not a full JTOK packet, more bytes expected */
    JTOK_ERROR_PART = -3
} jtokerr_t;


/**
 * JTOK token description.
 * @param       type    type (object, array, string etc.)
 * @param       start   start position in JTOK data string
 * @param       end     end position in JTOK data string
 */

typedef struct jtoktok_struct jtoktok_t;
struct jtoktok_struct
{
#if defined(JTOK_STANDALONE_TOKENS)
    char *json;          /* pointer to json string              */
    jtoktok_t *list;     /* pointer to start of token list      */
#endif /* #if defined(JTOK_STANDALONE_TOKENS) */
    jtoktype_t type;     /* the type of token                   */
    int        start;    /* start index of token in json string */
    int        end;      /* end index of token in json string   */
    int        size;     /* number of sub-tokens in the token   */
#if defined(JTOK_PARENT_LINKS)
    int parent;          /* index of parent in the token array  */
#endif /* #if defined(JTOK_PARENT_LINKS) */
};


/**
 * JTOK parser. Contains an array of token blocks available. Also stores
 * the string being parsed now and current position in that string
 */
typedef struct
{
    char *       json;     /* ptr to start of json string */
    unsigned int pos;      /* current parsing index in json string */
    unsigned int toknext;  /* index of next token to allocate */
    int          toksuper; /* superior token node, e.g parent object or array */
} jtok_parser_t;


/**
 * @brief construct jtok parser over and array of tokens
 *
 * @return the constructed jtok parser
 */
jtok_parser_t jtok_new_parser(void);


/**
 * @brief Parse a jtok
 *
 * @param parser jtok parser
 * @param js jtok string
 * @param len length of jtok string
 * @param tokens array of jtoktok (provided by caller)
 * @param num_tokens max number of tokens to parse
 * @return jtokerr_t parse status. Errors are indicated by return values < 0, otherwise the number of parsed tokens is returned
 */
jtokerr_t jtok_parse(jtok_parser_t *parser, const char *js, size_t len,
                     jtoktok_t *tokens, unsigned int num_tokens);


/**
 * @brief get the token length of a jtoktok_t;
 *
 * @param tok
 * @return uint32_t the length of the token
 */
uint_least16_t jtok_toklen(const jtoktok_t *tok);


#if defined(JTOK_STANDALONE_TOKENS)

/**
 * @brief Compare a jtok token with a nul-terminated string
 *
 * @param str char array
 * @param tok the jtoktok to compare against
 * @return true if equal
 * @return false if not equal
 */
bool jtok_tokcmp(const char *str, const jtoktok_t *tok);


/**
 * @brief Compare no more than n bytes between a string and a json token
 *
 * @param str the string to compare against
 * @param tok the token to compare against
 * @param n max number of bytes to compare
 * @return true if equal within bytecount
 * @return false if not equal within bytecount
 */
bool jtok_tokncmp(const char *str, const jtoktok_t *tok, uint_least16_t n);


/**
 * @brief Copy a jtoktok_t into a buffer
 *
 * @param dst the destination byte buffer
 * @param bufsize size of desintation buffer
 * @param tkn jtok token to copy
 * @return char* NULL on error, otherwise, address of destination
 */
char *jtok_tokcpy(char *dst, uint_least16_t bufsize, const jtoktok_t *tkn);


/**
 * @brief Copy a jtoktok_t into a buffer
 *
 * @param dst the destination byte buffer
 * @param bufsize size of desintation buffer
 * @param tkn jtok token to copy
 * @return char* NULL on error, otherwise, address of destination
 */
char *jtok_tokncpy(char *dst, uint_least16_t bufsize, const jtoktok_t *tkn,
                   uint_least16_t n);

#else /* JTOK_STANDALONE_TOKENS not defined */

/**
 * @brief Compare a token with a nul-terminated string
 *
 * @param str char array
 * @param json the json string
 * @param tok  instance of a jtok token parsed from the json string
 * @return true if equal
 * @return false if not equal
 */
bool jtok_tokcmp(const char *str, const uint8_t *json, const jtoktok_t *tok);


/**
 * @brief Compare no more than n bytes between a string and a jtoktok
 *
 * @param str the string to compare against
 * @param json the json string referenced by token tok
 * @param tok the token to compare against
 * @param n max number of bytes to compare
 * @return true if equal within bytecount
 * @return false if not equal within bytecount
 */
bool jtok_tokncmp(const uint8 *str, const uint8_t *json, const jtoktok_t *tok,
                  uint_least16_t n);

/**
 * @brief Copy a jtoktok_t json token into a buffer
 *
 * @param buf the destination byte buffer
 * @param bufsize size of destination buffer
 * @param json json string
 * @param tkn jtoktok to copy
 * @return char* NULL on error, else, the destination
 */
char *jtok_tokcpy(char *dst, uint_least16_t bufsize, const uint8_t *json,
                  const jtoktok_t *tkn);

/**
 * @brief Copy a no more than n bytes from a jtoktok_t json token into a buffer
 *
 * @param buf the destination byte buffer
 * @param bufsize size of destination buffer
 * @param json the json string
 * @param tkn jtoktok to copy
 * @return char* NULL on error, else, the destination
 */
char *jtok_tokncpy(char *dst, uint_least16_t bufsize, const uint8_t *json,
                   const jtoktok_t *tkn, uint_least16_t n);

#endif /* #if defined(JTOK_STANDALONE_TOKENS) */


/**
 * @brief Check if a jtoktok array constitutes a valid jtok structure
 *
 * @param tokens token array
 * @param tcnt number of tokens parsed from some arbitrary jtok string
 * @return true valid
 * @return false invalid
 */
bool isValidJson(const jtoktok_t *tokens, uint_least8_t tcnt);


/**
 * @brief Utility wrapper for printing the type name of a jtoktok as a string
 *
 * @param type the type to stringify
 * @return char* the type name as a string
 */
char *jtok_toktypename(jtoktype_t type);


/**
 * @brief Utility wrapper for printing a string corresponding to a jtokerr_t
 *
 * @param err the error code
 * @return char* the error message
 */
char *jtok_jtokerr_messages(jtokerr_t err);


/**
 * @brief Test if a token is a key
 * 
 * @param token the token to check
 * @return true if the token is a json key
 * @return false otherwise
 */
bool jtok_tokenIsKey(jtoktok_t token);



#ifdef __cplusplus
}
#endif
#endif /* __JTOK_H_ */

#endif /* JTOK_H_ */