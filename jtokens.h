#ifndef CARLSJSONPARSING
#define CARLSJSONPARING
#include <stdint.h>

typedef uint32_t uword_t;
typedef int32_t word_t;

/**
 * JSON parser. Contains an array of token blocks available. Also stores
 * the string being parsed now and current position in that string
 */
typedef struct
{
    uword_t pos;     /* index offset in the JSON string */
    uword_t toknext; /* next token to allocate */
    word_t toksuper; /* superior token node, e.g parent object or array */
} token_parser;

/* a token can be 1 of 4 types. will consider adding in a 5th type for UNK/initialization value */
typedef enum
{
    JSMN_PRIMITIVE = 0, /* number, bool, or null 															*/
    JSMN_OBJECT = 1,    /* object 																						*/
    JSMN_ARRAY = 2,     /* array 																							*/
    JSMN_STRING = 3     /* string. NOTE THAT IT MUST BE QUOTED (JSON strict )	*/
} jtokType_t;

/**
 * JSON token description.
 * @param		type	type (object, array, string etc.)
 * @param		start	start position in JSON data string
 * @param		end		end position in JSON data string
 */
typedef struct
{
    jtokType_t type;
    word_t start;
    word_t end;
    word_t size;
#ifdef PARENT_TOKEN_LINKS
    word_t parent;
#endif
} jtok_t;

typedef enum
{
    PARSE_OK, /* no errors YAAY */

    /* errors the occurred during tokenization 																								*/
    TOKERR_NOMEM,     /* Not enough space allocated for all the tokens 								*/
    TOKERR_INVAL,     /* Invalid character inside JSON string 													*/
    TOKERR_PART,      /* The string is not a full JSON packet, more bytes expected 		*/
    TOKERR_NULLJSON,  /* Attempted to parse a json that has nullptr to start of string */
    TOKERR_NULLTOKEN, /* Passed in nullptr as param for the start of token array 			*/
    TOKERR_CRITICAL,  /* some poword_ter errors have caused something terribly wrong */

    /* poword_ter errors have corrupted a token's type */
    PARSE_START_TYPE_CRITICAL,
    PARSE_KEY_TYPE_CRITICAL,
    PARSE_KEYMATCH_TYPE_CRITICAL,
    PARSE_NOKEYMATCH_TYPE_CRITICAL,

    /* poword_ter errors have corrupted the parse state */
    PARSE_STATE_CRITICAL,

    /* got a different type than was expected 																*/
    PARSE_NO_OBJ,        /* very first type must be a json object no matter what 			*/
    PARSE_EXPECT_JSON,   /* expected a json object but got a different type 	*/
    PARSE_EXPECT_ARRAY,  /* expected an array but got a different type 			*/
    PARSE_EXPECT_STRING, /* expected a string but got a different type 			*/
    PARSE_EXPECT_PRIM,   /* expected a primitive but got a different type 	*/

    /* key errors 																				*/
    PARSE_NOKEYMATCH,
    PARSE_KEY_IS_OBJ,  /* keys cannot be objects 			*/
    PARSE_KEY_IS_ARR,  /* keys cannot be arrays  			*/
    PARSE_KEY_IS_PRIM, /* keys cannot be primitives 		*/
    PARSE_KEY_NO_VAL,  /* the key is mising its value 	   */

    /* value errors (will only occur once a key has been matched) */
    PARSE_VALUE_TYPE_ERR, /* the value type is not permitted for the matched key    */
    PARSE_VALUE_STRCONV,  /* error converting the string to a value (VAL TYPE IS PRIM)   */
    PARSE_VALUE_NOMATCH   /* the value's string is not a valid value for the matched key  */
} parseRetval_t;

static const char *parseStatusMsg[] = {
    "PARSING SUCCEEDED",
    "TOKENIZING FAILED: COULDN'T ALLOCATE ENOUGH HEAP FOR ALL THE TOKENS",
    "TOKENIZING FAILED: INVALID CHARACTER IN JSON",
    "TOKENIZING FAILED: INCOMPLETE JSON.",
    "TOKENIZING FAILED: JSON STRING IS NULL",
    "TOKENIZING FAILED: TOKEN PTR IS NULL",
    "TOKENIZING FAILED: CRITICAL ERROR. INVALID RETVAL. POSSIBLE POINTER CORRUPTION.",
    "PARSE FAILED: CRITICAL ERROR WITH JSMNTYPE VALUE WHEN STATE == START",
    "PARSE FAILED: CRITICAL ERROR WITH JSMNTYPE VALUE WHEN STATE == KEY",
    "PARSE FAILED: CRIITCAL ERROR WITH JSMNTYPE VALUE WHEN STATE == KEYMATCH",
    "PARSE FAILED: CRITICAL ERROR WITH JSMNTYPE VALUE WHEN STATE == NOKEYMATCH",
    "PARSE FAILED: CRITICAL ERROR WITH PARSE STATE VALUE",
    "PARSE FAILED: FIRST JSON TOKEN MUST BE A JSON OBJECT",
    "PARSE FAILED: EXPECTED A JSON OBJECT",
    "PARSE FAILED: EXPECTED AN ARRAY",
    "PARSE FAILED: EXPECTED A STRING",
    "PARSE FAILED: EXPECTED A PRIMITIVE",
    "PARSE FAILED: NO VALUE FOUND AFTER A KEY (ALL KEYS MUST HAVE VALUES)",
    "PARSE FAILED: THE VALUE OF A KEY WAS NOT A PERMITTED TYPE",
    "PARSE FAILED: UNABLE TO PARSE A JSMN STRING OR JSMN PRIMITIVE INTO A VALUE",
    "PARSE FAILED: UNABLE TO MATCH A KEY",
};

/**
 * Create JSON parser over an array of tokens
 */
void jtok_init(token_parser *parser);

/**
 * Run JSON parser.
 *  
 * It parses a JSON data string word_to and array of tokens, each describing
 * a single JSON object.
 */
parseRetval_t jtokenize(token_parser *parser,
                        const char *js,
                        size_t len,
                        jtok_t *tokens,
                        uword_t num_tokens);
#endif