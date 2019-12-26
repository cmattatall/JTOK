#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "jtok.h"

#define JTOK_VERBOSE

enum tokstate
{
  IN_OBJECT, //in the middle of parsing an object
  KEY,       //in the middle of parsing a key
  VALUE,     //in the middle of parsing a value
  END_OBJECT,
};

static void transmit(const char const *c);
static parseRetval_t retokenize(jparser_t *parser, const char *json, uword_t num_tokens);

static void fill_token(jtok_t *token, jtokType_t type, word_t start, word_t end);

static jtok_t *alloc_token(jparser_t *parser, uword_t num_tokens);

// for some reason the heap implementation isnt working on stm despite allocating 8192 heap addresses.
// I'm going to use a stack based implementation for now.
parseRetval_t jtokenize(jparser_t *parser, const char *json)
{
  parseRetval_t result;

#ifdef TOKEN_HEAP
  parser->tokens = NULL; // init as precaution
#endif

  if (json == NULL) // for when some idiot passes a null pointer as a string param
  {
    result.status = JPARSE_NULLSTR;
  }
  else // normal usecase
  {
#ifdef TOKEN_HEAP
    const uword_t max_safe_loops = 20;
    word_t num_loops = max_safe_loops;
    word_t num_tokens = BASE_TCNT_GUESS;

    result = retokenize(parser, json, num_tokens); // attempt to parse again

    // if failed due to alloc, retry several times with larger array size
    while (result.status == JPARSE_NOMEM)
      ;
    {
      if (parser->tokens != NULL)
      {
        free(parser->tokens);
      }
      parser->tokens = calloc((num_tokens = num_tokens * 2 + 1), sizeof(jtok_t));
      result = retokenize(parser, json, num_tokens);
      if (!--num_loops) // safety check
      {
        result.status = JPARSE_MAXHEAP_ATTEMPTS; // safety condition exceeded
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
static parseRetval_t retokenize(jparser_t *parser, const char *json, uword_t num_tokens)
{
  parseRetval_t result;

#if defined(JTOK_DEBUG) || defined(JTOK_VERBOSE)
  uint8_t debug_msg[250];
#endif

  //the parsing state
  enum tokstate state = IN_OBJECT;

  //temp vars for comparisons
  jtok_t *token;
  jtokType_t type;

  // initialize the parser
  parser->pos = 0;
  parser->start = 0;
  parser->toknext = 0;
  parser->toksuper = -1;

  if (json != NULL)
  {
    for (; result.status == JPARSE_OK && json[parser->pos] != '\0'; parser->pos += 1)
    {

#if defined(JTOK_VERBOSE) || defined(JTOK_DEBUG)
      if (parser->toksuper != -1 && parser->tokens[parser->toksuper].type == JTOK_VAL)
      {
        snprintf((char *)debug_msg, sizeof(debug_msg),
                 "assertion of tokens[toksuper].type != JTOK_VAL failed. "
                 "critical error! pos = %d."
                 "possible error with source code logic\n",
                 parser->pos);
        transmit((const char *)debug_msg);
        break;
      }
#endif

      switch (state)
      {
      case IN_OBJECT:

        switch (json[parser->pos])
        {
        //WHITESPACE : skip when in object mode.
        case '\v': //vertical tab
        case '\t':
        case ' ':
        case '\r': //carriage return
        case '\n': //lf or nl
          break;
        //start of new object
        case '[':
        case '{':
          result.cnt += 1;

          //create new token (and check for error condition)
          if ((token = alloc_token(parser, num_tokens)) == NULL)
          {
            result.status = JPARSE_NOMEM;
          }
          else
          {
            //if object has a parent node
            if (parser->toksuper != -1)
            {
              token->parent = parser->toksuper; //this token's parent is the superior token index
              switch (parser->tokens[token->parent].type)
              {
              //if the previous token type was a key and we're not at top level node,
              //then increase that token's size. any other type is an error
              case JTOK_KEY:
                parser->tokens[token->parent].size += 1;
                break;
              default:
                result.status = JPARSE_INVAL;
                break;
              }
            }

            //validate token type
            token->type = json[parser->pos] == '{' ? JTOK_OBJ : JTOK_ARR;

            //start of this token is the current character index
            token->start = parser->pos;

            //store the current token index because we're inside a new child object
            //(toknext - 1 is current index because alloc token incremented toknext)
            parser->toksuper = parser->toknext - 1;
          }
          break;
        case ']':
        case '}':
          state = END_OBJECT;
          break;
        case ':':
          //update the superior token index. The next token will belong to the current token.
          parser->toksuper = parser->toknext - 1;
          break;
        case ',':
          if (parser->toksuper != -1)
          {
            switch (parser->tokens[parser->toksuper].type)
            {
            //don't need to restore the superior node because it's already the array token
            case JTOK_ARR:
              break;
            case JTOK_KEY:
              //the superior node is restored to the one to which the key belongs
              parser->toksuper = parser->tokens[parser->toksuper].parent;
              break;
            case JTOK_OBJ: //superior node should never be an object if we see a comma.
            case JTOK_VAL: //values cant be parent tokens
              result.status = JPARSE_INVAL;
              break;
            default:
#if defined(JTOK_VERBOSE) || defined(JTOK_DEBUG)
              snprintf((char *)debug_msg, sizeof(debug_msg),
                       "found >,< in object mode with toksuper != -1. "
                       "but switch on tokens[toksuper].type is an invalid jtok type. "
                       "critical error! pos = %d.\n",
                       parser->pos);
              transmit((const char *)debug_msg);
#endif
              result.status = JPARSE_INVAL;
              break;
            }
          }
          else
          {
            result.status = JPARSE_INVAL;
          }
          break;
        case '\"': //double quote (start of string), (unsure if it's key or value yet though)
          if (parser->toksuper != -1)
          {
            switch (parser->tokens[parser->toksuper].type)
            {
            case JTOK_OBJ: //this case only occurs for very first object key or when we've finished parsing a value
              state = KEY;
              break;
            case JTOK_ARR: //arrays are comma delimited values each belonging to the array token
              state = VALUE;
              break;
            case JTOK_KEY:
              //we look at what token type the current superior token belongs to.
              //if current superior token is a key and it belongs to an object, then this token must be a value
              if (parser->tokens[parser->tokens[parser->toksuper].parent].type == JTOK_OBJ)
              {
                state = VALUE;
              }
              else //if it belongs to anything else there is an error in the json (since arrays cant have keys)
              {
                result.status = JPARSE_INVAL;
              }
              break;
            case JTOK_VAL: //values cant own tokens.
              result.status = JPARSE_INVAL;
              break;
            default:
#if defined(JTOK_VERBOSE) || defined(JTOK_DEBUG)
              snprintf((char *)debug_msg, sizeof(debug_msg),
                       "found start of string in object mode with toksuper != -1. "
                       "but switch on tokens[toksuper].type is an invalid jtok type. "
                       "critical error! pos = %d.\n",
                       parser->pos);
              transmit((const char *)debug_msg);
#endif
              result.status = JPARSE_INVAL;
              break;
            }

            //save current position in case of incorrect format later in the string.
            //only once we find terminating quote will we assign that token::start = parser->start
            parser->start = parser->pos;
          }
          else //case wherein very first token is not an object type
          {
            result.status = JPARSE_INVAL;
          }
          break;
        case '+': //SIGNS
        case '-':
          //fall through

        case '0': //NUMERICS
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        //fall through

        //true/false/null. we dont include capitalized versions
        //because json key convention dictates that keys must start with lower case
        case 't': //true
        case 'f': //false
        case 'n': //null
          if (parser->toksuper != -1)
          {
            switch (parser->tokens[parser->toksuper].type)
            {
            case JTOK_ARR:
            case JTOK_KEY:
              //save current position in case of incorrect format later in the string.
              //only once we find terminating character will we assign that token::start = parser->start
              parser->start = parser->pos;
              state = VALUE;
              break;
            case JTOK_OBJ: //keys must be quoted and values can't belong to an object
            case JTOK_VAL: //values cant belong to values
              result.status = JPARSE_INVAL;
              break;
            default:
#if defined(JTOK_VERBOSE) || defined(JTOK_DEBUG)
              snprintf((char *)debug_msg, sizeof(debug_msg),
                       "found start of numeric (or +/- sign) in object mode with toksuper != -1. "
                       "but switch on tokens[toksuper].type is an invalid jtok type. "
                       "critical error! pos = %d.\n",
                       parser->pos);
              transmit((const char *)debug_msg);
#endif
              break;
            }
          }
          else
          {
            result.status = JPARSE_INVAL;
          }
          state = VALUE;
          break;
        default:
#if defined(JTOK_DEBUG) || defined(JTOK_VERBOSE)
          snprintf((char *)debug_msg, sizeof(debug_msg), "default case in object mode. invalid char. "
                                                         "state = %d, parser->pos = %d. "
                                                         "character = %d\n",
                   state, parser->pos, json[parser->pos]);
          transmit((const char *)debug_msg);
#endif
          result.status = JPARSE_INVAL; //IN IN_OBJECT MODE, ALPHABETICS, CONTROL CHARACTERS, ETC MUST BE QUOTED
          break;
        }
        break;
      case KEY:
        switch (json[parser->pos])
        {
        case '\"': //double quote. formal end of key
          if ((token = alloc_token(parser, num_tokens)) == NULL)
          {
            //restore the parser position so we can issue a more accurate error
            parser->pos = parser->start;
            result.status = JPARSE_NOMEM;
          }
          else
          {
            // do start + 1 to skip the beginning quote
            fill_token(token, JTOK_KEY, parser->start + 1, parser->pos);
            token->parent = parser->toksuper; //link to parent node

            result.cnt += 1;
            parser->tokens[token->parent].size += 1; //increase the size of the parent token
            state = IN_OBJECT;
          }
          break;

        //SPECIAL CONTROL CHARACTERS
        //we may only receive a partial json in which case it would be terminated by \r or \n.
        //the error in that case should not be invalid, but instead partial.
        //therefore, we must skip over \r and \n and keep looking for the ending quote.
        case '\r': //carriage return
        case '\n': //lf or nl
          break;
        default:
          //if the character isnt underscore or an alphanumeric its invalid
          if (isalnum(json[parser->pos]) == 0 && json[parser->pos] != '_')
#if defined(JTOK_DEBUG) || defined(JTOK_VERBOSE)
            snprintf((char *)debug_msg, sizeof(debug_msg),
                     "critical character case. "
                     "invalid character in key mode. "
                     "state = %d, parser->pos = %d. "
                     "character = %d\n",
                     state, parser->pos, json[parser->pos]);
          transmit((const char *)debug_msg);
#endif
          //restore the parser position so we can issue a more accurate error
          parser->pos = parser->start;
          result.status = JPARSE_INVAL;
          break;
        }
        break;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////

      case VALUE:
        switch (json[parser->pos])
        {

        //chars that would delimit a value
        case '\"': //double quote
          if (json[parser->start] != '\"')
          {
#if defined(JTOK_DEBUG) || defined(JTOK_VERBOSE)
            snprintf((char *)debug_msg, sizeof(debug_msg),
                     "terminating quote present on UNQUOTED value"
                     "state = %d, parser->pos = %d. "
                     "current character = %d\n",
                     state, parser->pos, json[parser->pos]);
            transmit((const char *)debug_msg);
#endif
            parser->pos = parser->start;
            result.status = JPARSE_INVAL;
          }
          else //we have completed a quoted value
          {
            if ((token = alloc_token(parser, num_tokens)) == NULL)
            {
              parser->pos = parser->start;
              result.status = JPARSE_NOMEM;
            }
            else
            {
              fill_token(token, JTOK_VAL, parser->start, parser->pos);
              token->parent = parser->toksuper; //link to parent

              //restore parent token index.
              parser->toksuper = parser->tokens[token->parent].parent;
              result.cnt += 1;
              parser->tokens[token->parent].size += 1; //increase size of parent token
              state = IN_OBJECT;
            }
          }
          break;
        case '\v': //vertical tab
        case '\r': //carriage return
        case '\n': //lf or nl
        case '\t':
        case ' ':
        case ',':
        case '}':
        case ']':
          if (json[parser->start] != '\"')
          {
            if ((token = alloc_token(parser, num_tokens)) == NULL)
            {
              parser->pos = parser->start;
              result.status = JPARSE_NOMEM;
            }
            else
            {
              fill_token(token, JTOK_VAL, parser->start, parser->pos);
              token->parent = parser->toksuper; //link to parent

              //restore parent token index.
              parser->toksuper = parser->tokens[token->parent].parent;
              result.cnt += 1;
              parser->tokens[token->parent].size += 1; //increase size of parent token

              //check for case wherein value is terminated not by whitespace or comma 
              //but by end of token to which its key belongs
              state = (json[parser->pos] == '{' || json[parser->pos] == '[') ? END_OBJECT : IN_OBJECT;
            }
          }
          else //we are missing the terminating quote on a quoted value
          {
#if defined(JTOK_DEBUG) || defined(JTOK_VERBOSE)
            snprintf((char *)debug_msg, sizeof(debug_msg),
                     "terminating quote missing from quoted value! "
                     "state = %d, parser->pos = %d. "
                     "current character = %d\n",
                     state, parser->pos, json[parser->pos]);
            transmit((const char *)debug_msg);
#endif
            parser->pos = parser->start;
            result.status = JPARSE_INVAL;
          }
          break;
        case '+': //SIGNS
        case '-':
        case '0': //NUMERICS
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          //do nothing because these are always valid in values either quoted or unquoted
          break;
        case 'A': //ALPHABETICS
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
          //alphanumeric strings can only be values if they're quoted
          //this means we don't allow hex values (always parse as base 10)
          //in the future I'd like to allow hex but there are too many possible bugs
          //for an initial release
          if (json[parser->start] == '\"')
          {
            //do nothing
            break;
          }
          //else fallthrough to default case (which indicates error)
        default:
#if defined(JTOK_DEBUG) || defined(JTOK_VERBOSE)
          snprintf((char *)debug_msg, sizeof(debug_msg),
                   "invalid character case in value mode. "
                   "state = %d, parser->pos = %d. "
                   "character = %d\n",
                   state, parser->pos, json[parser->pos]);
          transmit((const char *)debug_msg);
#endif
          parser->pos = parser->start;
          result.status = JPARSE_INVAL;
          break;
        }
        break;
      case END_OBJECT:
        //store current type
        type = (json[parser->pos] == '}' ? JTOK_OBJ : JTOK_ARR);

        //check for case where we have >}< without { earlier in string
        if (parser->toknext < 1)
        {
          result.status = JPARSE_INVAL;
        }
        else
        {
          //get the token that the {/] character terminates
          //(current idx is toknext - 1 because alloc_token increments toknext)
          token = &(parser->tokens[parser->toknext - 1]);

          //the entire purpose of this loop is to restore the value of parser->toksuper.
          //since we've completed an object, the next token we come across should belong to the same
          //top node as the just-completed object's KEY. Current object belongs to its key,
          //that key belongs to node level - 1. we will traverse the heiarchy back up to the top
          //to update the value of superior node.

          //normally, finding the >,< character in object mode would restore the superior node index.
          //but since last key-value pair of json doesnt necessarily have a , after the value, we
          //must rely on the object termination itself to restore the index
          word_t num_loops;

          //its impossible to have a token chain greater in length than the num tokens.
          //so that is the safety condition I have imposed on this iterative check.
          for (num_loops = 0; num_loops < num_tokens; num_loops += 1)
          {
            if (token->start != -1) //if we've found the start of the token,
            {
              if (token->end == -1) //but not the end
              {
                if (token->type == type) //and the token types match...
                                         //this is really a check for cases such as:
                                         //
                                         //{"key1" : { "key2" : "wokfeewf", "key3" : { .... ] }
                                         //                                                 ^
                                         //notice the bracket mismatch                      ^
                {
                  token->end = parser->pos + 1;
                  parser->toksuper = token->parent; //update the superior node.
                }
                else
                {
                  result.status = JPARSE_INVAL;
                }
                break;
              }
            }
            //jump to parent tkn, repeating the checks until we reach top level token
            if (token->parent != -1)
            {
              token = &(parser->tokens[token->parent]);
            }
            else
            {
              //consider breaking loop here.
              //
              //I'm really not certain if we should break or issue an error.
              //since technically, if we did not break at the previous check,
              //there is an uneven number of {/[ and }/] chars inside
              //the current lowest level child object that was just completed
            }
          }
        }
        state = IN_OBJECT;
        break;
      default:
#if defined(JTOK_DEBUG) || defined(JTOK_VERBOSE)
        snprintf((char *)debug_msg, sizeof(debug_msg),
                 "critical error. "
                 "default case occured on parse state. "
                 "state is an invalid value. "
                 "possible programmer error . "
                 "state = >%d<\n",
                 state);
        transmit((char *)debug_msg);
#endif
        break;
      }
      break;
    }
  }
  else
  {
    result.status = JPARSE_NULLSTR;
  }
  return result;
}

static void transmit(const char const *c)
{
  // user adds their own code for their platform:
  //ex:
  //uart_transmit..
  //usb_cdc_msg
  //etc..

  printf("%s", c);
}

static jtok_t *alloc_token(jparser_t *parser, uword_t num_tokens)
{
  jtok_t *tok;
  if (parser->toknext >= num_tokens)
  {
    tok = NULL;
  }
  else
  {
    tok = &parser->tokens[parser->toknext];
    tok->start = tok->end = -1;
    tok->size = 0;
    tok->parent = -1;
    parser->toknext += 1;
  }
  return tok;
}

/**
 * Fills token type and various fields
 */
static void fill_token(jtok_t *token, jtokType_t type, word_t start, word_t end)
{
  token->type = type;
  token->start = start;
  token->end = end;
  token->size = 0;
}
