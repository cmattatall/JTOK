
/* JTOK.C
   State-machine based JSON parser for embedded systems inspired by JSMN.
   Author: Carl Mattatall
*/

#define JTOK_VERBOSE

#include "jtok.h"
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


// user adds their own implementation for their embedded platform
static void transmit(const char *const c)
{
  printf("%s", c);
}

static parseRetval_t retokenize(jparser_t *parser, const char *json, uword_t num_tokens);

static void fill_token(jtok_t *token,
                       jtokType_t type,
                       word_t start,
                       word_t end);

static jtok_t *alloc_token(jparser_t *parser, uword_t num_tokens);


//for some reason the heap implementation isnt working on stm despite allocating 8192 heap addresses.
//I'm going to use a stack based implementation for now.
parseRetval_t jtokenize(jparser_t *parser, const char *json)
{
  parseRetval_t result;

#ifdef TOKEN_HEAP
  parser->tokens = NULL; //init as precaution
#endif

  if (json == NULL) //for when some idiot passes a null pointer as a string param
  {
    result.status = JPARSE_NULLSTR;
  }
  else //normal usecase
  {
#ifdef TOKEN_HEAP
    const uword_t max_safe_loops = 20;
    word_t num_loops = max_safe_loops;
    word_t num_tokens = BASE_TCNT_GUESS;

    result = retokenize(parser, json, num_tokens); //attempt to parse again

    //if failed due to alloc, retry several times with larger array size
    while (result.status == JPARSE_NOMEM)
      ;
    {
      if (parser->tokens != NULL)
      {
        free(parser->tokens);
      }
      parser->tokens = calloc((num_tokens = num_tokens * 2 + 1), sizeof(jtok_t));
      result = retokenize(parser, json, num_tokens);
      if (!--num_loops) //safety check
      {
        result.status = JPARSE_MAXHEAP_ATTEMPTS; //safety condition exceeded
      }
    }
    if (result.status != JPARSE_OK)
    {
      free(parser->tokens);
      parser->tokens = NULL;
    }
#else
    result = retokenize(parser, json, MAX_STACK_TCNT);
#endif
  }
  return result;
}

/* This hides the actual parsing functionality and memory management from the user */
static parseRetval_t retokenize(jparser_t *parser,
                                const char *json,
                                uword_t num_tokens)
{
  parseRetval_t result;
  result.status = JPARSE_OK;
  result.cnt = 0;

#if defined(JTOK_DEBUG) || defined(JTOK_VERBOSE)
  uint8_t debug_msg[250];
#endif

  // the parse state
  enum
  {
    OBJECT,
    STRING,
    PRIMTIVE,
  } state;
  state = OBJECT;

  //temporary variables
  jtok_t *token;

  //initialize the parser
  parser->pos = 0;
  parser->start = 0;
  parser->toknext = 0;
  parser->toksuper = -1;

  if (json == NULL)
  {
    result.status = JPARSE_NULLSTR;
  }
  else
  {
    for (; result.status == JPARSE_OK && json[parser->pos] != '\0'; parser->pos += 1)
    {
      jtokType_t type;
      switch (state)
      {
      case OBJECT:
        switch (json[parser->pos])
        {
        case '{': //start of parent object
        case '[':
          result.cnt += 1;
          if ((token = alloc_token(parser, num_tokens)) == NULL)
          {
            result.status = JPARSE_NOMEM;
          }
          else
          {
            //if object has a parent node, increment the parent node's size
            if (parser->toksuper != -1)
            {
              parser->tokens[parser->toksuper].size += 1;
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
              if (token->start != -1) //if we've found the start of the token,
              {
                if (token->end == -1) //but not the end
                {
                  if (token->type == type) //and the bracket types of start and end match
                  {
                    token->end = parser->pos + 1;
                    parser->toksuper = token->parent;
                  }
                  else
                  {
                    result.status = JPARSE_INVAL;
                  }
                  break;
                }
              }
              if (token->parent != -1)
              {
                token = &(parser->tokens[token->parent]);
              }
            }
          }
          break;
        case '\'': //single quote
#ifndef SINGLE_QUOTING_ALLOWED
          result.status = JPARSE_INVAL;
          break;
#endif
        case '\"': //double quote. start of string mode
          parser->start = parser->pos;
          state = STRING;
          break;

        //WHITESPACE
        case '\t':
        case ' ':
        case '\r':
        case '\n':
          break;

        //KEY -> VAL DELIMITER
        case ':':
          //the token after the >:< has current token as its superior node
          parser->toksuper = parser->toknext - 1;
          break;

        //VAL -> KEY OR VAL -> VAL DELIMITER
        //if current object is array, comma delimits list of values
        //if current object is object,comma delimits lists of key:values
        case ',':
          if (parser->tokens[parser->toksuper].type != JTOK_ARR &&
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

        //start of primitives.
        //primitives must be numbers or booleans.
        case '-':
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
            if (iscntrl(json[parser->pos]) == 0)
            {
              result.status = JPARSE_INVAL;
            }
            else //something weird is happening. likely problem with program logic
            {
              result.status = JPARSE_CRITICAL;
            }
            break;
          }
          break;
        default: //unexpected character
          result.status = JPARSE_INVAL;
          break;
        }
        break;
      case STRING:
        if (isalnum(json[parser->pos]) != 0)
        {
          //case where the character is alphanumeric
        }
        else
        {
          switch (json[parser->pos])
          {

          //we allow cr and lf because json could only be partially transmitted over serial.
          //treat these like whitespace as they aren't necessarily in the string intentionally
          case '\r':
          case '\n':
          case '!':
            break;
          case '\'':
#ifndef SINGLE_QUOTING_ALLOWED
            result.status = JPARSE_INVAL;
            break;
#endif
          case '\"':
            if ((token = alloc_token(parser, num_tokens)) == NULL)
            {
              parser->pos = parser->start;
              result.status = JPARSE_NOMEM;
            }
            else
            {
              // do start + 1 to skip the beginning quote
              fill_token(token, JTOK_STR, parser->start + 1, parser->pos);
              token->parent = parser->toksuper; //link to parent node

              result.cnt += 1;
              parser->tokens[token->parent].size += 1; //increase the size of the parent token
              state = OBJECT;
            }
            break;
          //TODO: ADD SUPPORT FOR JSONS WITH STRING : NUM key-val formatting..

          /*
        To properly do this, we need to use a "key" mode and "value" mode instead of just "string"
        mode.

        we also need to differentiate object mode from array mode. 
        This probably means storing the the type of the object we just left

        ex: { "key1" : {"subobjectKey : [ "arraystringval1", "arraystringval2", "arraystringval3"]}}
        ex: { "key1" : {"subobjectKey : [ arrayprimval1, arrayprimval2, arrayprimval3]}}
        */

          //invalid characters. CURRENTLY WE DO NOT SUPPORT STRING : STRING, ONLY STRING : NUM.
          case ':': //there should never be >:< in a string
          case ',': //there should never be >,< in a string
          case '}':
          case ']':
          case '[':
          case '{':

          //todo: this case falls into the realm of quoted values eg: "-123"
          //when in "key" mode, this is invalid but when in "value" mode, it isn't
          case '-':
            result.status = JPARSE_INVAL;
            break;
          default: //default for string mode
#if defined(JTOK_DEBUG) || defined(JTOK_VERBOSE)
            snprintf((char*)debug_msg, sizeof(debug_msg), "STRINGMODE default case triggered by character >%c<", json[parser->pos]);
            transmit((char*)debug_msg);
#endif
            break;
          }
        }
        break;
      case PRIMTIVE:
        switch (json[parser->pos])
        {
        //characters that would delimit a primitive token
        case '\t':
        case ' ':
        case ',':
        case ']':
        case '}':
          if ((token = alloc_token(parser, num_tokens)) == NULL)
          {
            parser->pos = parser->start;
            result.status = JPARSE_NOMEM;
          }
          else
          {
            fill_token(token, JTOK_NUM, parser->start, parser->pos);
            token->parent = parser->toksuper; //link to parent

            //restore parent object index.
            parser->toksuper = parser->tokens[token->parent].parent;

            result.cnt += 1;
            parser->tokens[token->parent].size += 1; //increase size of parent object
            state = OBJECT;
          }
          break;
        case ':': //invalid characters
        case '{':
        case '[':
        case '\'':
        case '\"':
          result.status = JPARSE_INVAL;
          break;
        default: //default for primitive mode

          break;
        }
        break;
      default:
        //critical: somehow a pointer corrupted the parse state or something...
        result.status = JPARSE_CRITICAL;
        break;
      }
    }
    //last check is if the parsed string is a complete json, or if a child/topnode object hasn't been completed yet.
    if (result.status == JPARSE_OK)
    {
      word_t idx;

      //for all populated tokens so far
      for (idx = parser->toknext - 1; idx >= 0; idx--)
      {
        if (parser->tokens[idx].start != -1) //if start index has been set
        {
          if (parser->tokens[idx].end == -1) //but end index hasn't
          {
            //then we don't have a complete json string
            result.status = JPARSE_PARTIAL;
            break;
          }
        }
      }
      result.status = JPARSE_INVAL;
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
    tok = NULL;
  }
  else
  {
    tok = &parser->tokens[parser->toknext];
    parser->toknext += 1;
    tok->start = tok->end = -1;
    tok->size = 0;
    tok->parent = -1;
  }
  return tok;
}

/**
 * Fills token type and various fields
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