
/* JTOK.C
   State-machine based JSON parser for embedded systems inspired by JSMN.
   Author: Carl Mattatall
*/

#include "jtok.h"
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>

static parseRetval_t retokenize(jparser_t *parser,
                                const char *json,
                                uword_t max_len,
                                uword_t num_tokens);

static void fill_token(jtok_t *token,
                       jtokType_t type,
                       word_t start,
                       word_t end);

static jtok_t *alloc_token(jparser_t *parser,
                           uword_t num_tokens);

#include <stdio.h.>
#include <assert.h>

parseRetval_t jtokenize(jparser_t *parser,
                        const char *json,
                        uword_t max_len)
{
  parseRetval_t result;

  //initialize the parser
  parser->pos = 0;
  parser->start = 0;
  parser->toknext = 0;
  parser->toksuper = -1;

  assert(json != NULL);

  if (json == NULL)
  {
    result.status = JPARSE_NULLSTR;
  }
  else
  {
    uword_t num_tokens = BASE_TKN_GUESS;
    do
    {
      num_tokens = num_tokens * 2 + 1;
      parser->tokens = (jtok_t *)realloc(parser->tokens, sizeof(jtok_t) * num_tokens);
      result = retokenize(parser, json, max_len, num_tokens);
    } while (result.status == JPARSE_NOHEAP);
  }
  return result;
}

static parseRetval_t retokenize(jparser_t *parser,
                                const char *json,
                                uword_t max_len,
                                uword_t num_tokens)
{
  parseRetval_t result;

  // the parse state
  enum
  {
    OBJECT,
    STRING,
    PRIMTIVE,
  } state;

  jtok_t *token;

  result.status = JPARSE_OK;
  result.cnt = 0;
  state = OBJECT;

  if (json == NULL)
  {
    result.status = JPARSE_NULLSTR;
  }
  else
  {
    for (; result.status == JPARSE_OK && parser->pos < max_len && json[parser->pos] != '\0'; parser->pos++)
    {
      jtokType_t type;

      if (iscntrl(json[parser->pos]) == 0) //if current char is not a control character
      {
        switch (state)
        {
        case OBJECT:
          switch (json[parser->pos])
          {
          case '{': //start of parent object
          case '[':
            result.cnt++;
            token = alloc_token(parser, num_tokens);
            if (token == NULL)
            {
              result.status = JPARSE_NOHEAP;
            }
            else
            {
              //if object has a parent node, increment the parent node's size
              if (parser->toksuper != -1)
              {
                (parser->tokens[parser->toksuper].size)++;
                token->parent = parser->toksuper;
              }
              token->type = (json[parser->pos] == '{' ? JTOK_OBJ : JTOK_ARR);
              token->start = parser->pos;
              parser->toksuper = parser->toknext - 1;
            }
            break;
          case '}': //end of parent object
          case ']':
            type = (json[parser->pos] == '}' ? JTOK_OBJ : JTOK_ARR);
            if (parser->toknext < 1)
            {
              result.status = JPARSE_INVAL;
            }
            else
            {
              token = &(parser->tokens[parser->toknext - 1]);

              //todo: this loop can be cleaned up
              for (;;)
              {
                if (token->start != -1 && token->end == -1)
                {
                  if (token->type != type)
                  {
                    result.status = JPARSE_INVAL;
                    break;
                  }
                  else
                  {
                    token->end = parser->pos + 1;
                    parser->toksuper = token->parent;
                  }
                }
                if (token->parent == -1)
                {
                  break;
                }
                token = &(parser->tokens[token->parent]);
              }
            }
            break;
          case '\"':
            parser->start = parser->pos;
            state = STRING;
            break;
          case '\t': //skip whitespace when we're in object parsing mode
          case '\r':
          case '\n':
          case ' ':
            break;
          case ':':
            parser->toksuper = parser->toknext - 1;
            break;
          case ',':

            if (
                parser->tokens[parser->toksuper].type != JTOK_ARR &&
                parser->tokens[parser->toksuper].type != JTOK_OBJ)
            {
              parser->toksuper = parser->tokens[parser->toksuper].parent;
            }
            else //NUM OR STRING MUST COME BEFORE COMMAS
            {
              result.status = JPARSE_INVAL;
              break;
            }
            break;
          case '-': /* primitives are: numbers and booleans */
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
          case 't':
          case 'f':
          case 'n':
            /* And they must not be keys of a parent object (keys MUST be quoted)*/
            switch ((&parser->tokens[parser->toksuper])->type)
            {
            case JTOK_OBJ:
              result.status = JPARSE_INVAL;
              break;
            case JTOK_ARR:
              if ((&parser->tokens[parser->toksuper])->size != 0)
              {
                result.status = JPARSE_INVAL;
              }
              break;
            case JTOK_NUM:
            case JTOK_STR:
              parser->start = parser->pos;
              state = PRIMTIVE;
              break;
            default:
              //critical
              break;
            }
          default: //unexpected character
            result.status = JPARSE_INVAL;
            break;
          }
          break;
        case STRING:

          switch (json[parser->pos])
          {
          case '\"':
            token = alloc_token(parser, num_tokens);
            if (token == NULL)
            {
              parser->pos = parser->start;
              result.status = JPARSE_NOHEAP;
            }
            else
            {
              fill_token(token, JTOK_STR, parser->start + 1, parser->pos);
              token->parent = parser->toksuper;
            }
            break;
          case '\'':
            result.status = JPARSE_INVAL;
            break;
          }

          if (token != NULL && token->end != -1)
          {
            result.cnt++;
            if (parser->toksuper != -1)
            {
              (parser->tokens[parser->toksuper].size)++;
            }
            state = OBJECT;
          }
          break;
        case PRIMTIVE:
          switch (json[parser->pos])
          {
          //characters that would delimit a primitive token
          case '\t':
          case '\r':
          case '\n':
          case ' ':
          case ',':
          case ']':
          case '}':
            token = alloc_token(parser, num_tokens);
            if (token == NULL)
            {
              parser->pos = parser->start;
              result.status = JPARSE_NOHEAP;
            }
            else
            {
              fill_token(token, JTOK_NUM, parser->start, parser->pos);
              token->parent = parser->toksuper;

              parser->pos--; //NOT SURE WHY I NEED THIS HERE

              result.cnt++;
              if (parser->toksuper != -1)
              {
                (parser->tokens[parser->toksuper].size)++;
              }
            }
            break;
          case ':': //invalid characters
          case '{':
          case '[':
          case '\'':
            result.status = JPARSE_INVAL;
            break;
          }

          if (token != NULL && token->end != -1)
          {
            result.cnt++;
            if (parser->toksuper != -1)
            {
              parser->tokens[parser->toksuper].size++;
            }
            state = OBJECT;
          }
          break;
        default:
          //critical: somehow a pointer corrupted parsing state
          break;
        }
      }
      else //control characters are invalid
      {
        result.status = JPARSE_INVAL;
      }
    }

    word_t idx;
    for (idx = parser->toknext - 1; idx >= 0; idx--)
    {
      /* Unmatched opened object or array */
      if (parser->tokens[idx].start != -1)
      {
        if (parser->tokens[idx].end == -1)
        {
          result.status = JPARSE_PARTIAL;
          break;
        }
      }
    }
  }
  return result;
}

static jtok_t *alloc_token(jparser_t *parser,
                           uword_t num_tokens)
{
  jtok_t *tok;
  if (parser->toknext >= num_tokens)
  {
    return NULL;
  }
  tok = &(parser->tokens[parser->toknext++]);
  tok->start = tok->end = -1;
  tok->size = 0;
  tok->parent = -1;
  return tok;
}

/**
 * Fills token type and boundaries.
 */
static void fill_token(jtok_t *token,
                       jtokType_t type,
                       word_t start,
                       word_t end)
{
  token->type = type;
  token->start = start;
  token->end = end;
  token->size = 0;
}