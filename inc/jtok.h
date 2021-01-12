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

#define INVALID_ARRAY_INDEX (-1)
#define NO_PARENT_IDX (INVALID_ARRAY_INDEX)
#define NO_SIBLING_IDX (INVALID_ARRAY_INDEX)
#define NO_CHILD_IDX (INVALID_ARRAY_INDEX)
#define JTOK_STRING_INDEX_NONE (INVALID_ARRAY_INDEX)

#define JTOK_MAX_RECURSE_DEPTH 25

/**
 * JTOK type identifier. Basic types are:
 *  - Object
 *  - Array
 *  - String
 *  - Other primitive: number, boolean (true/false) or null
 */
typedef enum
{
    JTOK_UNASSIGNED_TOKEN,
    JTOK_PRIMITIVE,
    JTOK_OBJECT,
    JTOK_ARRAY,
    JTOK_STRING,
} JTOK_TYPE_t;

typedef enum
{
    /* Parsed successfully! */
    JTOK_PARSE_STATUS_OK,

    /* For errors that have not yet been classified in source code */
    JTOK_PARSE_STATUS_UNKNOWN_ERROR,

    /* Not enough tokens were provided */
    JTOK_PARSE_STATUS_NOMEM,

    /* Caller passed null parameter */
    JTOK_PARSE_STATUS_NULL_PARAM,

    /* Invalid character inside JTOK string */
    JTOK_PARSE_STATUS_INVAL,

    /* The string is not a full JTOK packet, more bytes expected */
    JTOK_PARSE_STATUS_PARTIAL_TOKEN,

    /* key is missing value. ex: {"key\"}*/
    JTOK_PARSE_STATUS_KEY_NO_VAL,

    /* something like { , "key" : 123, } */
    JTOK_PARSE_STATUS_COMMA_NO_KEY,

    /* Aggregate types must have parent
    types of string (other than top-level object) */
    JTOK_PARSE_STATUS_OBJECT_INVALID_PARENT,

    /* eg : { key : 123} */
    JTOK_PARSE_STATUS_INVALID_PRIMITIVE,

    /* eg: "key" : 123 (literally missing the top-level object braces) */
    JTOK_PARSE_STATUS_NON_OBJECT,

    /* Token had an invalid start index */
    JTOK_PARSE_STATUS_INVALID_START,

    /* Token had an invalid end index */
    JTOK_PARSE_STATUS_INVALID_END,

    /* { {...}}  jtok_string must be first token inside object */
    JTOK_PARSE_STATUS_OBJ_NOKEY,

    /* eg : { "key" : [123, "123"]} */
    JTOK_STATUS_MIXED_ARRAY,

    /* eg { "key" : [123 123]} */
    JTOK_PARSE_STATUS_ARRAY_SEPARATOR,

    /* eg: { "key" : [123,, 123]} */
    JTOK_PARSE_STATUS_STRAY_COMMA,

    /* eg : { "key" "value "} */
    JTOK_PARSE_STATUS_VAL_NO_COLON,

    /* eg { "key" : 123 : 456} */
    JTOK_PARSE_STATUS_KEY_MULTIPLE_VAL,

    /* eg { [1234] : :123 } */
    JTOK_PARSE_STATUS_INVALID_PARENT,

    /* eg { "key" : 123 "key2"...} */
    JTOK_PARSE_STATUS_VAL_NO_COMMA,

    JTOK_PARSE_STATUS_NON_ARRAY,

    /* eg {"" : "value"} */
    JTOK_PARSE_STATUS_EMPTY_KEY,

    JTOK_PARSE_STATUS_NEST_DEPTH_EXCEEDED,

} JTOK_PARSE_STATUS_t;


typedef enum
{
    JTOK_VALUE_TYPE_not_a_value_tkn,
    JTOK_VALUE_TYPE_uint,
    JTOK_VALUE_TYPE_int,
    JTOK_VALUE_TYPE_real,
    JTOK_VALUE_TYPE_boolean,
    JTOK_VALUE_TYPE_empty,
    JTOK_VALUE_TYPE_null,
    JTOK_VALUE_TYPE_str,
} JTOK_VALUE_TYPE_t;


typedef struct jtok_tkn_struct jtok_tkn_t;
struct jtok_tkn_struct
{

    char *      json;    /* json string into which the data structure inserts */
    jtok_tkn_t *pool;    /* Token pool */
    JTOK_TYPE_t type;    /* type (object, array, string etc.) */
    int         start;   /* start position in JTOK data string */
    int         end;     /* end position in JTOK data string */
    int         size;    /* number of child tokens */
    int         parent;  /* index of parent token in the token pool */
    int         sibling; /* index of next token that shares the same parent */
};

typedef struct
{
    char *       json;      /* ptr to start of json string */
    jtok_tkn_t * tkn_pool;  /* token pool */
    unsigned int pool_size; /* pool size */
    int          json_len;  /* max length of json string   */
    int          pos;       /* current parsing index in json string */
    int          toknext;   /* index of next token to allocate */
    int          toksuper; /* superior token node, e.g parent object or array */
    int          last_child; /* index of last sibling parsed */
} jtok_parser_t;


/**
 * @brief Parse a json string into its JTOK token representation
 *
 * @param json json string (nul-terminated) to parse
 * @param tkns caller-provided pool of tokens
 * @param size number of tokens in the token pool (max number of tokens that can
 * be parsed)
 * @return JTOK_PARSE_STATUS_t parse status. JTOK_PARSE_STATUS_OK == success
 */
JTOK_PARSE_STATUS_t jtok_parse(const char *json, jtok_tkn_t *tkns, size_t size);


/**
 * @brief get the token length of a jtok_tkn_t;
 *
 * @param tok
 * @return uint32_t the length of the token
 */
uint_least16_t jtok_toklen(const jtok_tkn_t *tok);


/**
 * @brief Compare a jtok token with a nul-terminated string
 *
 * @param str char array
 * @param tok the jtoktok to compare against
 * @return true if equal
 * @return false if not equal
 */
bool jtok_tokcmp(const char *str, const jtok_tkn_t *tok);


/**
 * @brief Compare no more than n bytes between a string and a json token
 *
 * @param str the string to compare against
 * @param tok the token to compare against
 * @param n max number of bytes to compare
 * @return true if equal within bytecount
 * @return false if not equal within bytecount
 */
bool jtok_tokncmp(const char *str, const jtok_tkn_t *tok, uint_least16_t n);


/**
 * @brief Copy a jtok_tkn_t into a buffer
 *
 * @param dst the destination byte buffer
 * @param bufsize size of desintation buffer
 * @param tkn jtok token to copy
 * @return char* NULL on error, otherwise, address of destination
 */
char *jtok_tokcpy(char *dst, uint_least16_t bufsize, const jtok_tkn_t *tkn);


/**
 * @brief Copy a jtok_tkn_t into a buffer
 *
 * @param dst the destination byte buffer
 * @param bufsize size of desintation buffer
 * @param tkn jtok token to copy
 * @return char* NULL on error, otherwise, address of destination
 */
char *jtok_tokncpy(char *dst, uint_least16_t bufsize, const jtok_tkn_t *tkn,
                   uint_least16_t n);


/**
 * @brief Check if a jtoktok array constitutes a valid jtok structure
 *
 * @param tokens token array
 * @param tcnt number of tokens parsed from some arbitrary jtok string
 * @return true valid
 * @return false invalid
 */
bool isValidJson(const jtok_tkn_t *tokens, uint_least8_t tcnt);


/**
 * @brief Utility wrapper for printing the type name of a jtoktok as a string
 *
 * @param type the type to stringify
 * @return char* the type name as a string
 */
char *jtok_toktypename(JTOK_TYPE_t type);


/**
 * @brief Utility wrapper for printing a string corresponding to a
 * JTOK_PARSE_STATUS_t
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
bool jtok_tokenIsKey(jtok_tkn_t token);


/**
 * @brief Load a buffer with the fields of a jtok_tkn_t token so it can be
 * printed
 *
 * @param buf the buffer to load
 * @param size size of the buffer
 * @param json the original json string
 * @param token the jtoktok token
 * @return int number of bytes written to buffer, else -1
 */
int jtok_token_tostr(char *buf, unsigned int size, const char *json,
                     jtok_tkn_t token);


/**
 * @brief Compare two jtok tokens for equality
 *
 * @param tkn1 first token
 * @param tkn2 second token
 * @return true if tokens are equal
 * @return false if not equal.
 *
 * @note Tokens with different types are never equal
 */
bool jtok_toktokcmp(const jtok_tkn_t *tkn1, const jtok_tkn_t *tkn2);


#if 0
typedef struct
{
    jtok_tkn_t *key;
    jtok_tkn_t *value;
} jtok_keyval_pair_t;


typedef struct
{
    jtok_keyval_pair_t *next;
    jtok_keyval_pair_t this;
} jtok_keyval_pair_node_t;

jtok_keyval_pair_node_t *jtok_get_children(const jtok_tkn_t *obj);
#endif


/**
 * @brief Get the
 *
 * @param obj
 * @param key
 * @return int
 */
int jtok_obj_has_key(const jtok_tkn_t *obj, const char *key_str);


#ifdef __cplusplus
}
#endif
#endif /* __JTOK_H_ */
#endif /* JTOK_H_ */
