/**
 * @file jtok.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Statically allocated JTOK parser for embedded systems
 * @version 0.4
 * @date 2020-11-13
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 */

#include "jtok.h"

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define JTOK_ASCII_CHAR_LOWEST_VALUE 32   /* ' ' space */
#define JTOK_ASCII_CHAR_HIGHEST_VALUE 127 /* DEL */


/**
 * @brief Allocate fresh token from the token pool
 *
 * @param parser
 * @param tokens
 * @param num_tokens
 * @return jtoktok_t*
 */
static jtoktok_t *jtok_alloc_token(jtok_parser_t *parser, jtoktok_t *tokens,
                                   size_t num_tokens);


/**
 * @brief Fill jtok_token type and boundaries
 *
 * @param token the jtok token to populate
 * @param type the token type
 * @param start stard index
 * @param end end index
 *
 * @return 0 on success, 1 on failure
 */
static int jtok_fill_token(jtoktok_t *token, jtoktype_t type, int start,
                           int end);


/**
 * @brief Parse and fill next available token as jtok primitive
 *
 * @param parser the jtok parser
 * @param js the jtok string
 * @param len length of jtok string
 * @param tokens token array (caller provided)
 * @param num_tokens maximum number of tokens to parse
 * @return jtokerr_t parse status
 */
static jtokerr_t jtok_parse_primitive(jtok_parser_t *parser, const char *js,
                                      size_t len, jtoktok_t *tokens,
                                      size_t num_tokens);


/**
 * @brief Parse and fill next available jtok token as a jtok string
 *
 * @param parser the jtok parser
 * @param js jtok string
 * @param len length of jtok string
 * @param tokens token array (caller provided)
 * @param num_tokens max number of tokens to parse
 * @return jtokerr_t parse status
 */
static jtokerr_t jtok_parse_string(jtok_parser_t *parser, const char *js,
                                   size_t len, jtoktok_t *tokens,
                                   size_t num_tokens);


char *jtok_toktypename(jtoktype_t type)
{
    static const char *jtoktok_type_names[] = {
        [JTOK_PRIMITIVE] = "JTOK_PRIMITIVE",
        [JTOK_OBJECT]    = "JTOK_OBJECT",
        [JTOK_ARRAY]     = "JTOK_ARRAY",
        [JTOK_STRING]    = "JTOK_STRING",
    };
    char * retval = NULL;
    switch (type)
    {
        case JTOK_PRIMITIVE:
        case JTOK_OBJECT:
        case JTOK_ARRAY:
        case JTOK_STRING:
        {
            retval = (char*)jtoktok_type_names[type];
        }
        break;
    }
    return retval;
}


char *jtok_jtokerr_messages(jtokerr_t err)
{
    static const char *jtokerr_messages[] = {
        [0] = "Not enough jtoktok_t tokens were provided",
        [1] = "Invalid character inside JTOK string",
        [2] = "The string is not a full JTOK packet, more bytes expected",
    };
    char *retval = NULL;
    switch (err)
    {
        case JTOK_ERROR_NOMEM:
        {
            retval =  (char*)jtokerr_messages[0];
        }
        break;
        case JTOK_ERROR_INVAL:
        {
            retval =  (char*)jtokerr_messages[1];
        }
        break;
        case JTOK_ERROR_PART:
        {
            retval =  (char*)jtokerr_messages[2];
        }
        break;
    }
    return retval;
}

uint_least16_t jtok_toklen(const jtoktok_t *tok)
{
    uint_least16_t len = 0;
    if (tok != NULL)
    {
        uint_least64_t tokstart = tok->start;
        uint_least64_t tokend   = tok->end;
        if (tokend - tokstart < UINT16_MAX)
        {
            len = tokend - tokstart;
        }
    }
    return len;
}


#if defined(JTOK_STANDALONE_TOKENS)

bool jtok_tokcmp(const char *str, const jtoktok_t *tok)
{
    bool result = false;
    if (str == NULL)
    {
        if (tok != NULL && tok->json == NULL)
        {
            result = true;
        }
    }
    else if (tok != NULL && tok->json == NULL)
    {
        if (str == NULL)
        {
            result = true;
        }
    }
    else
    {
        uint_least16_t least_size = jtok_toklen(tok);
        uint_least16_t slen       = strlen(str);
        if (least_size < slen)
        {
            least_size = slen;
        }

        /* actually compare them */
        if (strncmp((const char *)str, (char *)&tok->json[tok->start],
                    least_size) == 0)
        {
            result = true;
        }
        else
        {
            result = false;
        }
    }
    return result;
}

bool jtok_tokncmp(const char *str, const jtoktok_t *tok, uint_least16_t n)
{
    bool result = false;
    if (str != NULL && tok != NULL && tok->json != NULL)
    {
        uint_least16_t least_size = jtok_toklen(tok);
        uint_least16_t slen       = strlen(str);
        if (least_size < slen)
        {
            least_size = slen;
        }

        if (least_size < n)
        {
            least_size = n;
        }

        /* actually compare them */
        if (strncmp((const char *)str, (char *)&tok->json[tok->start],
                    least_size) == 0)
        {
            result = true;
        }
        else
        {
            result = false;
        }
    }
    return result;
}


char *jtok_tokcpy(char *dst, uint_least16_t bufsize, const jtoktok_t *tkn)
{
    char *result = NULL;
    if (dst != NULL && tkn != NULL && tkn->json != NULL)
    {
        uint_least16_t copy_count = jtok_toklen(tkn);
        if (copy_count > bufsize)
        {
            copy_count = bufsize;
        }
        result = strncpy(dst, (char *)&tkn->json[tkn->start], copy_count);
    }
    return result;
}

char *jtok_tokncpy(char *dst, uint_least16_t bufsize, const jtoktok_t *tkn,
                   uint_least16_t n)
{
    char *         result = NULL;
    uint_least16_t count  = bufsize;
    if (bufsize > n)
    {
        count = n;
    }
    result = jtok_tokcpy(dst, count, tkn);
    return result;
}

#else /* JTOK_STANDALONE_TOKENS is not defined */


bool jtok_tokncmp(const uint8_t *str, const uint8_t *jtok, const jtoktok_t *tok,
                  uint_least16_t n)
{
    bool result = false;
    if (str != NULL && jtok != NULL && tok != NULL)
    {
        uint_least16_t least_size = jtok_toklen(tok);
        uint_least16_t slen       = strlen(str);
        if (least_size < slen)
        {
            least_size = slen;
        }

        if (least_size < n)
        {
            least_size = n;
        }

        /* actually compare them */
        if (strncmp((const char *)str, (char *)&jtok[tok->start], least_size) ==
            0)
        {
            result = true;
        }
        else
        {
            result = false;
        }
    }
}


bool jtok_tokcmp(const char *str, const uint8_t *jtok, const jtoktok_t *tok)
{
    bool result = false;
    if (str == NULL)
    {
        if (jtok == NULL)
        {
            result = true;
        }
        /* Fall to end, default return is false */
    }
    else if (jtok == NULL)
    {
        if (str == NULL)
        {
            result = true;
        }
        /* Fall to end, default return is false */
    }
    else
    {
        uint_least16_t least_size = jtok_toklen(tok);
        uint_least16_t slen       = strlen(str);
        if (least_size < slen)
        {
            least_size = slen;
        }

        /* actually compare them */
        if (strncmp((const char *)str, (char *)&jtok[tok->start], least_size) ==
            0)
        {
            result = true;
        }
        else
        {
            result = false;
        }
    }
    return result;
}

char *jtok_tokcpy(char *dst, uint_least16_t bufsize, const uint8_t *jtok,
                  const jtoktok_t *tkn)
{
    char *result = NULL;
    if (dst != NULL && jtok != NULL && tkn != NULL)
    {
        uint_least16_t copy_count = jtok_toklen(tkn);
        if (copy_count > bufsize)
        {
            copy_count = bufsize;
        }
        result = strncpy(dst, (char *)&jtok[tkn->start], copy_count);
    }
    return result;
}


char *jtok_tokncpy(char *dst, uint_least16_t bufsize, const uint8_t *jtok,
                   const jtoktok_t *tkn, uint_least16_t n)
{
    char *         result = NULL;
    uint_least16_t count  = bufsize;
    if (bufsize > n)
    {
        count = n;
    }
    result = jtok_tokcpy(dst, count, jtok, tkn);
    return result;
}

#endif /* #if defined(JTOK_STANDALONE_TOKENS) */


bool isValidJson(const jtoktok_t *tokens, uint_least8_t tcnt)
{
    bool isValid = false;
    if (tokens != NULL)
    {
        if (tcnt > 1)
        {
            if (tokens[0].type == JTOK_OBJECT)
            {
                isValid = true;
            }

            if (tcnt == 2) /* Cannot have something like { "myKey" } */
            {
                if (tokens[1].type == JTOK_ARRAY)
                {
                    /* however, { [ ] } is still technically valid */
                    isValid = true;
                }
            }
            else
            {
                /* First key in a jtok must be a "string" */
                if (tcnt > 2)
                {
                    if (tokens[1].type == JTOK_STRING)
                    {
                        isValid = true;
                    }
                }
            }
        }
    }
    return isValid;
}


static jtoktok_t *jtok_alloc_token(jtok_parser_t *parser, jtoktok_t *tokens,
                                   size_t num_tokens)
{
    jtoktok_t *tok;
    if (parser->toknext >= num_tokens)
    {
        return NULL;
    }
    tok        = &tokens[parser->toknext++];
    tok->start = tok->end = -1;
    tok->size             = 0;
#if defined(JTOK_PARENT_LINKS)
    tok->parent = -1;
#endif /* #if defined(JTOK_PARENT_LINKS) */

#if defined(JTOK_STANDALONE_TOKENS)
    tok->json = parser->json;
#endif /* #if defined(JTOK_STANDALONE_TOKENS) */
    return tok;
}


static int jtok_fill_token(jtoktok_t *token, jtoktype_t type, int start,
                           int end)
{
    if (token != NULL)
    {
        token->type  = type;
        token->start = start;
        token->end   = end;
        token->size  = 0;
        return 0;
    }
    else
    {
        return 1;
    }
}


static jtokerr_t jtok_parse_primitive(jtok_parser_t *parser, const char *js,
                                      size_t len, jtoktok_t *tokens,
                                      size_t num_tokens)
{
    jtoktok_t *token;
    int        start;
    start = parser->pos;

    for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++)
    {
        switch (js[parser->pos])
        {
#ifndef JTOK_STRICT
            /* In strict mode primitive must be followed by "," or
             * "}" or "]" */
            case ':':
#endif
            case '\t':
            case '\r':
            case '\n':
            case ' ':
            case ',':
            case ']':
            case '}':
                goto found;
        }
        if (js[parser->pos] < JTOK_ASCII_CHAR_LOWEST_VALUE ||
            js[parser->pos] >= JTOK_ASCII_CHAR_HIGHEST_VALUE)
        {
            parser->pos = start;
            return JTOK_ERROR_INVAL;
        }
    }
#ifdef JTOK_STRICT
    /* In strict mode primitive must be followed by a
     * comma/object/array */
    parser->pos = start;
    return JTOK_ERROR_PART;
#endif

found:
    if (tokens == NULL)
    {
        parser->pos--;
        return (jtokerr_t)0;
    }
    token = jtok_alloc_token(parser, tokens, num_tokens);
    if (token == NULL)
    {
        parser->pos = start;
        return JTOK_ERROR_NOMEM;
    }
    jtok_fill_token(token, JTOK_PRIMITIVE, start, parser->pos);
#ifdef JTOK_PARENT_LINKS
    token->parent = parser->toksuper;
#endif
    parser->pos--;
    return (jtokerr_t)0;
}


static jtokerr_t jtok_parse_string(jtok_parser_t *parser, const char *js,
                                   size_t len, jtoktok_t *tokens,
                                   size_t num_tokens)
{
    jtoktok_t *token;

    int start = parser->pos;

    parser->pos++;

    /* Skip starting quote */
    for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++)
    {
        char c = js[parser->pos];

        /* Quote: end of string */
        if (c == '\"')
        {
            if (tokens == NULL)
            {
                return (jtokerr_t)0;
            }
            token = jtok_alloc_token(parser, tokens, num_tokens);
            if (token == NULL)
            {
                parser->pos = start;
                return JTOK_ERROR_NOMEM;
            }
            jtok_fill_token(token, JTOK_STRING, start + 1, parser->pos);
#ifdef JTOK_PARENT_LINKS
            token->parent = parser->toksuper;
#endif
            return (jtokerr_t)0;
        }

        /* Backslash: Quoted symbol expected */
        if (c == '\\' && parser->pos + 1 < len)
        {
            int i;
            parser->pos++;
            switch (js[parser->pos])
            {
                /* Allowed escaped symbols */
                case '\"':
                case '/':
                case '\\':
                case 'b':
                case 'f':
                case 'r':
                case 'n':
                case 't':
                {
                }
                break;
                /* Allows escaped symbol \uXXXX */
                case 'u':
                {
                    parser->pos++;
                    for (i = 0;
                         i < 4 && parser->pos < len && js[parser->pos] != '\0';
                         i++)
                    {
                        /* If it isn't a hex character we have an
                         * error */
                        if (!isxdigit(js[parser->pos]))
                        {
                            parser->pos = start;
                            return JTOK_ERROR_INVAL;
                        }
                        parser->pos++;
                    }
                    parser->pos--;
                }
                break;
                /* Unexpected symbol */
                default:
                    parser->pos = start;
                    return JTOK_ERROR_INVAL;
            }
        }
    }
    parser->pos = start;
    return JTOK_ERROR_PART;
}


jtokerr_t jtok_parse(jtok_parser_t *parser, jtoktok_t *tokens, unsigned int num_tokens)
{
    jtokerr_t  r;
    int        i;
    jtoktok_t *token;
    int        count = 0;

    unsigned int len = parser->json_len;
    char *js = parser->json;
    for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++)
    {
        char       c;
        jtoktype_t type;

        c = js[parser->pos];
        switch (c)
        {
            case '{':
            case '[':
                count++;
                if (tokens == NULL)
                {
                    break;
                }
                token = jtok_alloc_token(parser, tokens, num_tokens);
                if (token == NULL)
                    return JTOK_ERROR_NOMEM;
                if (parser->toksuper != -1)
                {
                    tokens[parser->toksuper].size++;
#ifdef JTOK_PARENT_LINKS
                    token->parent = parser->toksuper;
#endif
                }
                token->type      = (c == '{' ? JTOK_OBJECT : JTOK_ARRAY);
                token->start     = parser->pos;
                parser->toksuper = parser->toknext - 1;
                break;
            case '}':
            case ']':
                if (tokens == NULL)
                    break;
                type = (c == '}' ? JTOK_OBJECT : JTOK_ARRAY);
#ifdef JTOK_PARENT_LINKS
                if (parser->toknext < 1)
                {
                    return JTOK_ERROR_INVAL;
                }
                token = &tokens[parser->toknext - 1];
                for (;;)
                {
                    if (token->start != -1 && token->end == -1)
                    {
                        if (token->type != type)
                        {
                            return JTOK_ERROR_INVAL;
                        }
                        token->end       = parser->pos + 1;
                        parser->toksuper = token->parent;
                        break;
                    }
                    if (token->parent == -1)
                    {
                        break;
                    }
                    token = &tokens[token->parent];
                }
#else
                for (i = parser->toknext - 1; i >= 0; i--)
                {
                    token = &tokens[i];
                    if (token->start != -1 && token->end == -1)
                    {
                        if (token->type != type)
                        {
                            return JTOK_ERROR_INVAL;
                        }
                        parser->toksuper = -1;
                        token->end       = parser->pos + 1;
                        break;
                    }
                }
                /* Error if unmatched closing bracket */
                if (i == -1)
                    return JTOK_ERROR_INVAL;
                for (; i >= 0; i--)
                {
                    token = &tokens[i];
                    if (token->start != -1 && token->end == -1)
                    {
                        parser->toksuper = i;
                        break;
                    }
                }
#endif
                break;
            case '\"':
            {
                r = jtok_parse_string(parser, js, len, tokens, num_tokens);
                if (r < 0)
                {
                    return r;
                }
                count++;
                if (parser->toksuper != -1 && tokens != NULL)
                {
                    tokens[parser->toksuper].size++;
                }
            }
            break;
            case '\t':
            case '\r':
            case '\n':
            case ' ':
            {
            }
            break;
            case ':':
            {
                parser->toksuper = parser->toknext - 1;
            }
            break;
            case ',':
            {
                if (tokens != NULL &&
                    tokens[parser->toksuper].type != JTOK_ARRAY &&
                    tokens[parser->toksuper].type != JTOK_OBJECT)
                {
#ifdef JTOK_PARENT_LINKS
                    parser->toksuper = tokens[parser->toksuper].parent;
#else
                    for (i = parser->toknext - 1; i >= 0; i--)
                    {
                        if (tokens[i].type == JTOK_ARRAY ||
                            tokens[i].type == JTOK_OBJECT)
                        {
                            if (tokens[i].start != -1 && tokens[i].end == -1)
                            {
                                parser->toksuper = i;
                                break;
                            }
                        }
                    }
#endif
                }
            }
            break;
#ifdef JTOK_STRICT
            /* In strict mode primitives are: numbers and booleans
             */
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
                /* And they must not be keys of the object */
                if (tokens != NULL)
                {
                    jtoktok_t *t = &tokens[parser->toksuper];
                    if (t->type == JTOK_OBJECT ||
                        (t->type == JTOK_STRING && t->size != 0))
                    {
                        return JTOK_ERROR_INVAL;
                    }
                }
#else
            /* In non-strict mode every unquoted value is a
             * primitive */
            default:
#endif
                r = jtok_parse_primitive(parser, js, len, tokens, num_tokens);
                if (r < 0)
                    return r;
                count++;
                if (parser->toksuper != -1 && tokens != NULL)
                    tokens[parser->toksuper].size++;
                break;

#ifdef JTOK_STRICT
            /* Unexpected char in strict mode */
            default:
                return JTOK_ERROR_INVAL;
#endif
        }
    }

    for (i = parser->toknext - 1; i >= 0; i--)
    {
        /* Unmatched opened object or array */
        if (tokens[i].start != -1 && tokens[i].end == -1)
        {
            return JTOK_ERROR_PART;
        }
    }

    return (jtokerr_t)count;
}


jtok_parser_t jtok_new_parser(const char *nul_terminated_json)
{
    jtok_parser_t parser;
    parser.pos      = 0;
    parser.toknext  = 0;
    parser.toksuper = -1;
    parser.json     = (char*)nul_terminated_json;
    parser.json_len = strlen(nul_terminated_json);
    return parser;
}



bool jtok_tokenIsKey(jtoktok_t token)
{
    if(token.type == JTOK_STRING)
    {
        if(token.size == 1)
        {
            return true;
        }
    }
    return false;
}



int jtok_token_tostr(char * buf, unsigned int size, const char * json, jtoktok_t token)
{   
    if(buf != NULL)
    {
        int blen = 0;
        blen += snprintf(buf + blen, size - blen, "token : ");
        for(unsigned int k = token.start; k < token.end; k++)
        {   
            blen += snprintf(buf + blen, size - blen, "%c", json[k]);
        }
        blen += snprintf(buf + blen, size - blen, "\n");
        blen += snprintf(buf + blen, size - blen, "type: %s\n", jtok_toktypename(token.type));

        #ifdef DEBUG
        blen += snprintf(buf + blen, size - blen, "start : %u\n", token.start);
        blen += snprintf(buf + blen, size - blen, "end : %u\n", token.end);
        #endif /* ifdef DEBUG */

        return blen;
    }
    else
    {
        return -1;
    }
}


jtok_value_map_t parse_value(const jtoktok_t *token)
{


    /** @todo */
}
