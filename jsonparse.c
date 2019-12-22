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


/* some JSONS to test the code on */
static char *jsons[8] = {
    "{\"GPIO_PIN_CONFIG\":{\"id\":1, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
    "{\"GPIO_PIN_CONFIG\":{\"id\":2, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
    "{\"GPIO_PIN_CONFIG\":{\"id\":3, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
    "{\"GPIO_PIN_CONFIG\":{\"id\":4, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
    "{\"GPIO_PIN_CONFIG\":{\"id\":5, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
    "{\"GPIO_PIN_CONFIG\":{\"id\":6, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
    "{\"GPIO_PIN_CONFIG\":{\"id\":7, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
    "{\"GPIO_PIN_CONFIG\":{\"id\":8, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
};

typedef uint32_t uword_t;
typedef int32_t word_t;

#define BASE_NUM_TOKENS 10 /* the initial guess of how many tkns to allocate on the heap */

/* hang forever if not true */
#define myASSERT(x)                  \
  do                                 \
  {                                  \
    if (!(x))                        \
    {                                \
      for (;;)                       \
      {                              \
        printf("ASSERTIONFAILED\n"); \
      }                              \
    }                                \
  } while (0) /* user adds semicolon as if macro expands word_to a "function" */

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
  parseRetval_t parse_ret;

  /* initial error checks */
  if (json == NULL)
  {
    parse_ret = TOKERR_NULLJSON;
  }
  else if (tokens == NULL)
  { 
    parse_ret = TOKERR_NULLTOKEN;
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
    parse_ret = jtokenize(&parser, json, strlen, tokens, n);

    /* make sure we have it all on the heap before proceeding */
    const uword_t max_LoopCnt = 20; /* safety measure */
    uword_t loops = 0;
    while (parse_ret == TOKERR_NOMEM && ++loops < max_LoopCnt)
    {
      /* increase size and get more heap. Then reattempt parse */
      tokens = realloc(tokens, sizeof(jtok_t) * (n = ((n * 2) + 1)));
      parse_ret = jtokenize(&parser, json, strlen, tokens, n);
    }

    /* check for errors that occurred after we claimed heap mem */
    if (parse_ret != PARSE_OK)
    {
      free(tokens);  /* dealloc */
      tokens = NULL; /* set as NULL to prevent user-based poword_ter mistakes */
    }
  }
  return parse_ret;
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

  myASSERT(tkns != NULL);
  if ((retval = json_tokenize(json, tkns, strlen(json), 20)) == PARSE_OK)
  {
    uword_t tkn_i;  /* the token index */
    word_t tkn_cnt; /* the number of tokens left to process */

    /* go through each token */
    for (tkn_i = 0, tkn_cnt = 1; tkn_cnt > 0 && retval == PARSE_OK; tkn_i++, tkn_cnt--)
    {
      //todo: remove these after debug
      tprint(json, tkns[tkn_i]);
      printf("tkn_i = %d, tkn_cnt = %d, state = %d (START,KEY,VAL,NOVAL,STOP)\n", tkn_i, tkn_cnt, state);

      /* MAKE SURE WE ARENT DEALING WITH AN UNINITIALIZED TOKEN */
      myASSERT(tkns[tkn_i].start != -1 && tkns[tkn_i].end != -1);

      word_t matchedKeyIndex   = -1; /* if a key is matched, this stores its lookup table index  */
      word_t matchedTableIndex = -1; /* the index of the key lookup table in the table table     */
      switch (state)
      {
        
      case START:
        switch (tkns[tkn_i].type) /* validate start condition (FIRST TYPE MUST BE OBJECT) */
        {
        case JSMN_STRING:                                      /* fall through (first token must be a json object) */
        case JSMN_PRIMITIVE:                                   /* fall through (first token must be a json object) */
        case JSMN_ARRAY:                                       /* fall through (first token must be a json object) */
          printf("THE FIRST TOKEN MUST BE A JSON OBJECT!!\n"); //todo: move to error message lookup table
          retval = PARSE_EXPECT_JSON;
          break;
        case JSMN_OBJECT:
          printf("tkn_i == %d, before the object, tknCnt == %d\n", tkn_i, tkn_cnt); //todo: remove post debug
          tkn_cnt += tkns[tkn_i].size;                                              /* update number of tokens left to parse */
          printf("tkn_i == %d, after the object, tknCnt == %d\n", tkn_i, tkn_cnt);  //todo: remove post debug
          break;
        default:
          retval = PARSE_START_TYPE_CRITICAL; /* critical error */
          break;
        }
        break;
      case KEY:
        switch (tkns[tkn_i].type)
        {
        case JSMN_STRING:
          /* update state machine (matchedKeyIndex will hold key table idx if match)*/
          matchedKeyIndex = isKeyValid(json, tkns[tkn_i], 0);
          state = (matchedKeyIndex != -1) ? KEYMATCH : NOKEYMATCH;
          break;
        case JSMN_PRIMITIVE:
          retval = PARSE_KEY_IS_PRIM;
          break;
        case JSMN_OBJECT:
          retval = PARSE_KEY_IS_OBJ;
          break;
        case JSMN_ARRAY:
          retval = PARSE_KEY_IS_ARR;
          break;
        default:
          retval = PARSE_KEY_TYPE_CRITICAL;
          break;
        }

        /* check if key actually has its value */
        if (tkns[tkn_i].size == 0)
        {
          retval = PARSE_KEY_NO_VAL;
        }
        break;
      case KEYMATCH:

        //todo: remove these 2 check after development.
        //When the logic is designed correctly, we will never get here with either index equal to  -1
        if (matchedKeyIndex == -1)
        { 
          printf("check for keyindex == -1 is true\n");
          break;
        }
        if (matchedTableIndex == -1)
        {
          printf("check for tableindex == -1 is true\n");
          break;
        }

        if (isValueTypeValid(tkns[tkn_i].type, matchedKeyIndex, matchedTableIndex) == 0)
        { 

          //todo move this switchcase into its own function that handles the value token conversion
          switch (tkns[tkn_i].type)
          {
          case JSMN_OBJECT:

          case JSMN_ARRAY:

            printf("tkn %d is a child object. before increasing cnt, tkn_cnt = %d\n", tkn_i, tkn_cnt); //todo: remove post debug
            tkn_cnt += tkns[tkn_i].size;
            printf("tkn %d is a child object. after increasing cnt, tkn_cnt = %d\n", tkn_i, tkn_cnt); //todo: remove post debug
            break;
          case JSMN_STRING:

            //todo: remove post debug
            printf("JSMN_STRING CASE INSIDE STATE == KEYMATCH. tkn_i == %d. "
            "DONT FORGET TO CHANGE THIS TO PARSING AND STORING THE VALUE\n", tkn_i);

            //check if it's "true", "false", "null",

            //check if the string is simply a number that's been quoted eg: "123645"

            //go through lookup table of valid strings for the matched key (tkn_i - 1) and find match

            //if none of these match checks produce a match, then error

            break;
          case JSMN_PRIMITIVE:

            //todo: remove post debug
            printf("JSMN_PRIMITIVE CASE INSIDE STATE == KEYMATCH. tkn_i == %d. "
            "DONT FORGET TO CHANGE THIS TO PARSING AND STORING THE VALUE\n", tkn_i);


            //todo convert the string to a value

            break;
          default:
            retval = PARSE_KEYMATCH_TYPE_CRITICAL;
            break;
          }
        }
        else
        {
          /* the current token's type is not valid for the matched key */
          retval = PARSE_VALUE_TYPE_ERR;
        }

        //state = ((tkn_cnt == 1) ? STOP : KEY);
        break;
      case NOKEYMATCH:
        switch (tkns[tkn_i].type)
        {
        case JSMN_STRING:
        case JSMN_OBJECT:
        case JSMN_ARRAY:
        case JSMN_PRIMITIVE:
          retval = PARSE_NOKEYMATCH;
          break;
        default:
          retval = PARSE_NOKEYMATCH_TYPE_CRITICAL;
          break;
        }

        if (tkns[tkn_i].type != JSMN_STRING && tkns[tkn_i].type != JSMN_PRIMITIVE)
        {
          printf("NOKEYMATCH CASE.object values must be strings or primitives!!. Did not find a token match for ");
          tprint(json, tkns[tkn_i - 1]);
        }

        state = STOP; /* stop parsing since no match */
        break;
      case STOP:
        /* do nothing, parsing has finished */
        break;
      default:
        printf("critical error. default case!!\n");
        retval = PARSE_STATE_CRITICAL;
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
  uword_t json_i = 0;

  word_t result = parse(jsons[json_i]);
  printf("%s.\n", parseStatusMsg[result]);
  

  /*
  for (json_i = 0; json_i < ARRAYSIZE(jsons); json_i++)
  {
    word_t result = parse(jsons[json_i]);
    if (result != 0)
    {
      printf("ERROR PARSING THE JSON!!!!\n");
      //return -1;
    }
    else
    {
      printf("SUCCEEDED!!\n");
    }
  }
  */
  return result;
}