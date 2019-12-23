/* 
CARLPARSER.c

###
A state-machine-based JSON parsing utility for embedded platforms.
Author: Carl Mattatall
###
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "jtokens.h"

typedef uint32_t uword_t;
typedef int32_t word_t;

#define BASE_NUM_TOKENS 10 /* the initial guess of how many tkns to allocate on the heap */


/*
 *  @func   : json_tokenize
 *  @brief  : wrapper for parse_tokens that tokenizes a json 
 *            string word_to an array of jsmn tokens on the heap.
 *  @params : char *str, jsmntok *tkns, strlen of json, number of tokens
 *  @retval : parseRetval_t indicating the tokenization status
 *  @notes  : 
 *  User is responsible for deallocating *tkns if parse is successful.
 *  In case of error, tkns is automatically deallocated and set as NULL.
 * 
 *  param4 is an initial guess for expected number of tokens. If passed as 0,
 *  a default vaulue is used and increased until the tokenized array can
 *  fully reside on the heap.
 */
parseRetval_t json_tokenize(char *json, jtok_t *tokens, size_t strlen, uword_t numTkns)
{
  /* return value */
  tokenizationResult_t result;

  /* initial error checks */
  if (json == NULL)
  {
    result.status = TOKERR_NULLJSON;
  }
  else if (tokens == NULL)
  {
    result.status = TOKERR_NULLTOKEN;
  }
  else /* ok to proceed */
  {
    token_parser parser;
    jtok_init(&parser);

    /* number of tokens to allocate as an initial guess if one isnt provided */
    uword_t n = numTkns == 0 ? BASE_NUM_TOKENS : numTkns;

    /* token array (on heap) */
    tokens = malloc(sizeof(jtok_t) * n);

    /* attempt to parse the json */

    result = jtokenize(&parser, json, strlen, tokens, n);
    
    printf("%s\n",parseStatusMsg[result.status]);

    /* make sure we have it all on the heap before proceeding */
    const uword_t max_LoopCnt = 20; /* safety measure */
    uword_t loops = 0;


    while (result.status == TOKERR_NOMEM && ++loops < max_LoopCnt)
    {
      /* increase size and get more heap. Then reattempt parse */
      tokens = realloc(tokens, sizeof(jtok_t) * (n = ((n * 2) + 1)));
      result = jtokenize(&parser, json, strlen, tokens, n);
    }


    /* check for errors that occurred after we claimed heap mem */
    if (result.status != PARSE_OK)
    {
      free(tokens);  /* dealloc */
      tokens = NULL; /* set as NULL to prevent user-based poword_ter mistakes */
    }
  }
  return result.status;
}

//todo: probably better to make this a macro since not all compilers will inline it...
static inline uint32_t json_token_streq(char *json, jtok_t tkn, char *str)
{
  /* convert to 0 or 1 using !! */
  return !!((0 == strncmp(json + tkn.start, str, tkn.end - tkn.start)) &&
            ((size_t)(tkn.end - tkn.start) == strlen(str)));
}

static word_t isKeyValid(char *json, jtok_t tkn, uword_t table_idx)
{
  word_t retval = -1; // -1 is an invalid array index

  /*
  word_t key_idx;     // index through the key table
  while (keyTable[table_idx][key_idx++].str != NULL)
  {
    printf("key_idx = %d\n", key_idx); //todo remove after debug
    if (json_token_streq(json, tkn, configKeys[key_idx]))
    {
      printf("FOUNDKEY: >%s<\n", configKeys[key_idx]);
      retval = key_idx;
      break;
    }
  }
  */

  return retval;
}

static word_t isValueTypeValid(jtokType_t type, uword_t key_idx, word_t tbl_idx)
{
  word_t retval = 0;

  //todo: go get the key table from the keytable table and check if type matches the list of accepted types for that matched key.

  return retval;
}

static void tprint(char *json, jtok_t tkn)
{
  printf("token: ");
  uword_t c;
  for (c = tkn.start; c < tkn.end; c++)
  {
    printf("%c", *(json + c));
  }
  printf("\nstart idx = %d", tkn.start);
  printf("\nend idx = %d", tkn.end);
  printf("\ntype = %d (0 = PRIMITIVE, 1 = OBJECT, 2 = ARRAY, 3 = STRING)", tkn.type);
  printf("\nsize = %d", tkn.size);
  printf("\nparentIdx = %d", tkn.parent);
  printf("\n\n");
}

static parseRetval_t parse(char *json)
{
  parseRetval_t retval;
  jtok_t tkns_val;
  jtok_t *tkns = &tkns_val;

  /* parsing states */
  enum
  {
    START,      /* start of parsing */
    KEY,        /* we're at a key */
    KEYMATCH,   /* we're at the value for a matched key */
    NOKEYMATCH, /* we didn't find a match with the preceeding key */
    STOP        /* end of parsing */
  } state;
  state = START;

  printf("\n\ntokenizing json:\n%s\n", json);

  if ((retval = json_tokenize(json, tkns, strlen(json), 0)) == PARSE_OK)
  {
    uword_t tkn_i;  /* the token index */
    word_t tkn_cnt; /* the number of tokens left to process */

    /* go through each token */
    for (tkn_i = 0, tkn_cnt = 1; tkn_cnt > 0 && retval == PARSE_OK; tkn_i++, tkn_cnt--)
    {
      //todo: remove these after debug
      tprint(json, tkns[tkn_i]);
      printf("tkn_i = %d, tkn_cnt = %d, state = %d (START,KEY,VAL,NOVAL,STOP)\n", tkn_i, tkn_cnt, state);

      switch (state)
      {
      case START:

        break;
      case STOP:

        break;
      default:

        break;
      }
    }
  }

  if (tkns != NULL)
  {
    free(tkns);
  }
  return retval;
}

/* test the parsing services */
word_t main(void)
{

  /* some JSONS to test the code on */
  const char *jsons[8] = {
      "{\"GPIO_PIN_CONFIG\":{\"id\":1, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
      "{\"GPIO_PIN_CONFIG\":{\"id\":2, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
      "{\"GPIO_PIN_CONFIG\":{\"id\":3, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
      "{\"GPIO_PIN_CONFIG\":{\"id\":4, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
      "{\"GPIO_PIN_CONFIG\":{\"id\":5, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
      "{\"GPIO_PIN_CONFIG\":{\"id\":6, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
      "{\"GPIO_PIN_CONFIG\":{\"id\":7, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
      "{\"GPIO_PIN_CONFIG\":{\"id\":8, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
  };

  char* test[] = 
  {
    "{\"keyStr1\":\"valStr1\"}",
    "{\"keyStr1\":\"valStr1\",\"keyStr2\":\"valStr2\"}",
    "{\"keyStr1\":111,\"keyStr2\":2}",
    "{\"keyStr1\":\"1\"}",
    "{\"keyStr1\":\"false\"}",
    "{\"objKey1\":{\"childKey1\":\"childVal1\",\"childKey2\":\"childVal2\"}}",
  };

  word_t result = parse(test[0]);
  printf("return value is %d\n", result);
  return result;
}