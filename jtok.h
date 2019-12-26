#ifndef JTOK
#define JTOK
#include <stdint.h>


typedef uint_fast16_t uword_t;
typedef int_fast16_t word_t;

#define BASE_TCNT_GUESS 20 //start guess at 20 tokens
#define MAX_STACK_TCNT 50

typedef enum
{
  JTOK_OBJ,
  JTOK_ARR,
  JTOK_STR,
  JTOK_NUM,
} jtokType_t;

typedef enum
{
  JPARSE_OK,
  JPARSE_NOMEM,
  JPARSE_INVAL,
  JPARSE_PARTIAL,
  JPARSE_NULLSTR,
  JPARSE_NULLTKNS,
  JPARSE_CRITICAL,
  #ifdef TOKEN_HEAP
  JPARSE_MAXHEAP_ATTEMPTS,
  #endif
} jparseCode_t;

typedef struct
{
  jtokType_t type; // token type
  word_t start;       // start idx in json
  word_t end;         // end idx in json
  uword_t size;        // number of elements in the token
  word_t parent;      // index of parent token
} jtok_t;


typedef struct
{ 
  uword_t start;   /* offset backup when we start parsing a token in case of error */
  uword_t pos;     /* current offset in the JSON string */
  uword_t toknext; /* next token to allocate */
  word_t toksuper; /* superior token node, tracks current parent object or array */

  /* ptr to array of tokens */
  #ifdef TOKEN_HEAP
  jtok_t *tokens;  
  #else
  jtok_t tokens[MAX_STACK_TCNT];  
  #endif
} jparser_t;

typedef struct
{
  jparseCode_t status;  /* parse status */
  uword_t cnt; /* number of parsed tokens */
} parseRetval_t;


parseRetval_t jtokenize(jparser_t *parser,const char *json);


#endif