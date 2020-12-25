/**
 * @file jtok_primitive.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to handle jtok primitive type operations
 * @version 0.1
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 * @note
 */

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "jtok_primitive.h"
#include "jtok_shared.h"


JTOK_PARSE_STATUS_t jtok_parse_primitive(jtok_parser_t *parser,
                                         jtoktok_t *tokens, size_t num_tokens)
{
    jtoktok_t *  token;
    int          start = parser->pos;
    const char * js    = (const char *)parser->json;
    unsigned int len   = parser->json_len;

    enum
    {
        NUMBER,
        BOOLEAN,
        ERROR
    } primitive_type = ERROR;

    bool exponent             = false;
    bool found_exponent_power = false;
    bool decimal              = false;
    bool found_decimal_places = false;
    for (start = parser->pos; parser->pos < len && js[parser->pos] != '\0';
         parser->pos++)
    {
        switch (js[parser->pos])
        {
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
            {
                if (parser->pos == start)
                {
                    primitive_type = NUMBER;
                }
                else
                {
                    if (primitive_type == NUMBER)
                    {
                        if (decimal)
                        {
                            found_decimal_places = true;
                        }
                    }
                    else
                    {
                        return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                    }

                    if (exponent)
                    {
                        if (!found_exponent_power)
                        {
                            found_exponent_power = true;
                        }
                    }
                }
            }
            break;
            case '+':
            case '-':
                /* signs must come at beginning, or as an exponent */
                if (start == parser->pos)
                {
                    primitive_type = NUMBER;
                }
                else if (exponent)
                {
                    /* do nothing */
                }
                else
                {
                    parser->pos = start;
                    return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                }
                break;
            case '.': /* decimal */
            {
                if (parser->pos == start)
                {
                    /* {"key" : .123} is invalid */
                    parser->pos = start;
                    return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                }
                else
                {
                    if (primitive_type == NUMBER)
                    {
                        if (decimal)
                        {
                            /* 123.0.1 is invalid primitive */
                            if (found_decimal_places)
                            {
                                return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                            }
                        }
                        else
                        {
                            decimal = true;
                        }

                        if (exponent)
                        {
                            /* { "key" : 123e+9.01} is invalid */
                            parser->pos = start;
                            return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                        }
                    }
                    else
                    {
                        return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                    }
                }
            }
            break;
            case 'e':
            case 'E':
            {
                if (start == parser->pos)
                {
                    /* {"key" : e9"} is invalid */
                    parser->pos = start;
                    return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                }
                else
                {
                    if (primitive_type == NUMBER)
                    {
                        /* previous char has to be a digit eg: 10e9 */
                        if (isdigit((int)js[parser->pos - 1]))
                        {
                            exponent             = true;
                            found_exponent_power = false;
                            continue;
                        }
                        else /* { "key" : -e9 } is invalid */
                        {
                            parser->pos = start;
                            return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                        }
                    }
                    else
                    {
                        return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                    }
                }
            }
            break;
            case '\t':
            case '\r':
            case '\n':
            case ' ':
            case ',':
            case ']':
            case '}':
            {
                if (tokens == NULL) /* caller provided no tokens at all */
                {
                    parser->pos--;
                    return JTOK_PARSE_STATUS_PARSE_OK;
                }
                char last = js[parser->pos - 1];

                if (exponent)
                {
                    if (!found_exponent_power)
                    {
                        parser->pos = start;
                        return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                    }
                }

                if (decimal && last == '.')
                {
                    parser->pos = start;
                    return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                }

                token = jtok_alloc_token(parser, tokens, num_tokens);
                if (token == NULL) /* not enough tokens provided by caller */
                {
                    parser->pos = start;
                    return JTOK_PARSE_STATUS_NOMEM;
                }
                jtok_fill_token(token, JTOK_PRIMITIVE, start, parser->pos);
                token->parent = parser->toksuper;

                /* go back 1 spot so when we return from current function, the
                calling context can look at the current character */
                parser->pos--;
                return JTOK_PARSE_STATUS_PARSE_OK;
            }
            break;
            default:
            {
                if (parser->pos == start)
                {
                    if (0 == strncmp(&js[start], "true", strlen("true")))
                    {
                        /* subtract 1 so we don't end up at character
                                  AFTER the final char in token */
                        parser->pos += strlen("true") - 1;
                        break;
                    }
                    else if (0 == strncmp(&js[start], "false", strlen("false")))
                    {
                        /* subtract 1 so we don't end up at character
                                  AFTER the final char in token */
                        parser->pos += strlen("false") - 1;
                        break;
                    }
                    else if (0 == strncmp(&js[start], "null", strlen("null")))
                    {
                        /* subtract 1 so we don't end up at character
                                  AFTER the final char in token */
                        parser->pos += strlen("null") - 1;
                        break;
                    }
                    else
                    {
                        parser->pos = start;
                        return JTOK_PARSE_STATUS_INVALID_PRIMITIVE;
                    }
                }
                parser->pos = start;
                return JTOK_PARSE_STATUS_PARTIAL_TOKEN;
            }
            break;
        }
    }
    return JTOK_PARSE_STATUS_PARTIAL_TOKEN;
}


bool jtok_toktokcmp_primitive(const jtoktok_t *tkn1, const jtoktok_t *tkn2)
{
    bool           is_equal = false;
    uint_least16_t len      = jtok_toklen(tkn1);


    {
        const char *start1 = &tkn1->json[tkn1->start];
        const char *start2 = &tkn2->json[tkn2->start];
    }


    return is_equal;
}