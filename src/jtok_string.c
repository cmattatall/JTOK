/**
 * @file jtok_string.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to handle jtok string operations
 * @version 0.1
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 * @note
 */

#include <ctype.h>
#include <string.h>

#include "jtok_string.h"
#include "jtok_shared.h"


JTOK_PARSE_STATUS_t jtok_parse_string(jtok_parser_t *parser, jtok_tkn_t *tokens,
                                      size_t num_tokens)
{
    jtok_tkn_t * token;
    int          start;
    char *       js  = parser->json;
    unsigned int len = parser->json_len;
    if (js[parser->pos] == '\"')
    {
        parser->pos++; /* advance to inside of quotes */
        start =
            parser->pos; /* token start is first character after the quote */
        for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++)
        {
            /* Quote: end of string */
            if (js[parser->pos] == '\"')
            {
                if (parser->pos == start)
                {
                    token = &tokens[parser->toksuper];
                    if (token->type != JTOK_STRING)
                    {
                        return JTOK_PARSE_STATUS_EMPTY_KEY;
                    }
                }
                token = jtok_alloc_token(parser, tokens, num_tokens);
                if (token == NULL)
                {
                    parser->pos = start;
                    return JTOK_PARSE_STATUS_NOMEM;
                }
                jtok_fill_token(token, JTOK_STRING, start, parser->pos);
                token->parent = parser->toksuper;
                return JTOK_PARSE_STATUS_PARSE_OK;
            }

            if (js[parser->pos] == '\\')
            {
                if (parser->pos + sizeof((char)'\"') < len)
                {
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
                        case 'u': /* Allows escaped symbol \uXXXX */
                        {
                            parser->pos++; /* move to first escaped hex
                                              character */
                            unsigned int       i;
                            const unsigned int max_i = HEXCHAR_ESCAPE_SEQ_COUNT;
                            for (i = 0; i < max_i && parser->pos < len &&
                                        js[parser->pos] != '\0';
                                 i++)
                            {
                                if (!isxdigit((int)js[parser->pos]))
                                {
                                    /* reset parser position and return error */
                                    parser->pos = start;
                                    return JTOK_PARSE_STATUS_INVAL;
                                }
                                parser->pos++;
                            }
                            parser->pos--;
                        }
                        break;
                        default: /* Unexpected symbol */
                        {
                            parser->pos = start;
                            return JTOK_PARSE_STATUS_INVAL;
                        }
                        break;
                    }
                }
            }
        }
        parser->pos = start;
        return JTOK_PARSE_STATUS_PARTIAL_TOKEN;
    }
    else
    {
        /* parse_string was called on a non-string */
        return JTOK_PARSE_STATUS_UNKNOWN_ERROR;
    }
}


bool jtok_toktokcmp_string(const jtok_tkn_t *tkn1, const jtok_tkn_t *tkn2)
{
    bool           is_equal = false;
    uint_least16_t len      = jtok_toklen(tkn1);
    if (len == jtok_toklen(tkn2))
    {
        const char *start1 = &tkn1->json[tkn1->start];
        const char *start2 = &tkn2->json[tkn2->start];
        if (0 == strncmp(start1, start2, len))
        {
            is_equal = true;
        }
    }
    return is_equal;
}
