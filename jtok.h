#ifndef JTOK
#define JTOK
#include <stdint.h>

typedef uint32_t uword_t;
typedef int32_t  word_t;


#define BASE_TKN_GUESS 20 //start guess at 20 tokens

typedef enum
{
  JTOK_UNK,
  JTOK_OBJ,
  JTOK_ARR,
  JTOK_STR,
  JTOK_NUM,
} jtokType_t;

typedef enum
{
  JPARSE_OK,
  JPARSE_NOHEAP,
  JPARSE_INVAL,
  JPARSE_PARTIAL,
  JPARSE_NULLSTR,
  JPARSE_NULLTKNS,
} jparseCode_t;

typedef struct
{
  jtokType_t type; // token type
  word_t start;       // start idx in json
  word_t end;         // end idx in json
  uword_t size;        // number of elements in the token
  uword_t parent;      // index of parent token
} jtok_t;


typedef struct
{ 
  uword_t start;   /* offset backup when we start parsing a token in case of error */
  uword_t pos;     /* current offset in the JSON string */
  uword_t toknext; /* next token to allocate */
  word_t toksuper; /* superior token node, tracks current parent object or array */
  jtok_t *tokens;  /* ptr to tokens allocated on heap */
} jparser_t;

typedef struct
{
  jparseCode_t status;  /* parse status */
  uword_t cnt; /* number of parsed tokens */
} parseRetval_t;


parseRetval_t jtokenize(jparser_t *parser,const char *json);


#endif