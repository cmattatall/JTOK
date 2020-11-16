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

#define MAX_JTOK_STRLEN 255

/**
 * JTOK type identifier. Basic types are:
 *  - Object
 *  - Array
 *  - String
 *  - Other primitive: number, boolean (true/false) or null
 */
typedef enum
{   
    JTOK_UNASSIGNED_TOKEN = 0,
    JTOK_PRIMITIVE        = 1,
    JTOK_OBJECT           = 2,
    JTOK_ARRAY            = 3,
    JTOK_STRING           = 4,
} jtoktype_t;

typedef enum
{   
    JTOK_PARSE_STATUS_PARSE_OK = 0,

    /* For errors that have not yet been classified in source code */
    JTOK_PARSE_STATUS_UNKNOWN_ERROR = -1,

    /* Not enough tokens were provided */
    JTOK_PARSE_STATUS_NOMEM = -2,

    /* Invalid character inside JTOK string */
    JTOK_PARSE_STATUS_INVAL = -3,

    /* The string is not a full JTOK packet, more bytes expected */
    JTOK_PARSE_STATUS_PARTIAL_TOKEN = -4,

    /* key is missing value. ex: {"key\"}*/
    JTOK_PARSE_STATUS_KEY_NO_VAL = -5, 

    /* something like { , "key" : 123, } */
    JTOK_PARSE_STATUS_COMMA_NO_KEY = -6,

    /* Aggregate types must have parent 
    types of string (other than top-level object) */
    JTOK_PARSE_STATUS_OBJECT_INVALID_PARENT = -7,

    /* eg : { key : 123} */
    JTOK_PARSE_STATUS_INVALID_PRIMITIVE = -8, 

    /* eg: "key" : 123 (literally missing the top-level object braces) */
    JTOK_PARSE_STATUS_NON_OBJECT = -9,

    /* Token had an invalid start index */
    JTOK_PARSE_STATUS_INVALID_START = -10,

    /* Token had an invalid end index */
    JTOK_PARSE_STATUS_INVALID_END = -11,
} JTOK_PARSE_STATUS_t;


typedef enum 
{   
    JTOK_VALUE_TYPE_unk, /* default value, assume parsing error */
    JTOK_VALUE_TYPE_uint,
    JTOK_VALUE_TYPE_int,
    JTOK_VALUE_TYPE_real,
    JTOK_VALUE_TYPE_boolean,
    JTOK_VALUE_TYPE_empty,
    JTOK_VALUE_TYPE_null,
    JTOK_VALUE_TYPE_str,
}   JTOK_VALUE_TYPE_t;

typedef enum
{   
    JTOK_VALUE_TYPE_PRIMITIVE_empty,
    JTOK_VALUE_TYPE_PRIMITIVE_boolean,
    JTOK_VALUE_TYPE_PRIMITIVE_null
}   JTOK_VALUE_TYPE_PRIMITIVE_t;


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
    int parent;          /* index of parent in the token array  */
};


/**
 * JTOK parser. Contains an array of token blocks available. Also stores
 * the string being parsed now and current position in that string
 */
typedef struct
{
    char *       json;     /* ptr to start of json string */
    unsigned int json_len; /* max length of json string   */
    unsigned int pos;      /* current parsing index in json string */
    unsigned int toknext;  /* index of next token to allocate */
    int          toksuper; /* superior token node, e.g parent object or array */
} jtok_parser_t;





typedef struct 
{
    JTOK_VALUE_TYPE_t type;
    union 
    {
        unsigned int as_uinteger;
        int as_integer;
        #if defined(JTOK_REAL_SINGLE_PRECISION)
        float as_real;
        #else
        double as_real;
        #endif /* if defined(JTOK_REAL_SINGLE_PRECISION) */
        char as_str[MAX_JTOK_STRLEN];
    } value; 
}   jtok_value_t;



typedef struct 
{
    jtoktok_t *token;
    jtok_value_t value;
}   jtok_value_map_t;


/**
 * @brief construct jtok parser over and array of tokens
 *
 * @param json the json string that will be parsed
 * @return the constructed jtok parser
 * 
 * @note json string must be nul-terminated!!
 */
jtok_parser_t jtok_new_parser(const char *nul_terminated_json);


/**
 * @brief Parse a jtok
 *
 * @param parser jtok parser
 * @param tokens array of jtoktok (provided by caller)
 * @param num_tokens max number of tokens to parse
 * @param final_idx pointer that is updated to the total number of tokens parsed from the json on success
 */
JTOK_PARSE_STATUS_t jtok_parse(jtok_parser_t *parser, jtoktok_t *tokens, unsigned int num_tokens);


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
bool jtok_tokncmp(const uint8_t *str, const uint8_t *json, const jtoktok_t *tok,
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
 * @brief Utility wrapper for printing a string corresponding to a JTOK_PARSE_STATUS_t
 *
 * @param err the error code
 * @return char* the error message
 */
char *jtok_jtokerr_messages(JTOK_PARSE_STATUS_t err);


/**
 * @brief Test if a token is a key
 * 
 * @param token the token to check
 * @return true if the token is a json key
 * @return false otherwise
 */
bool jtok_tokenIsKey(jtoktok_t token);





/**
 * @brief Load a buffer with the fields of a jtoktok_t token so it can be printed
 * 
 * @param buf the buffer to load
 * @param size size of the buffer
 * @param json the original json string
 * @param token the jtoktok token
 * @return int number of bytes written to buffer, else -1
 */
int jtok_token_tostr(char * buf, unsigned int size, const char * json, jtoktok_t token);



jtok_value_map_t parse_value(const jtoktok_t *token);


#ifdef __cplusplus
}
#endif
#endif /* __JTOK_H_ */

#endif /* JTOK_H_ */