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
#include <float.h>


#include "jtok_primitive.h"
#include "jtok_shared.h"


JTOK_PARSE_STATUS_t jtok_parse_primitive(jtok_parser_t *parser,
                                         jtok_tkn_t *tokens, size_t num_tokens)
{
    jtok_tkn_t * token;
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

                /* Go back 1 spot so when we return from current function, the
                 * calling context can look at the current character
                 *
                 * This is because if the token terminates on a '}', it
                 * may also terminate a superior token as well (such as a
                 * higher level object in the given example)
                 */
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

    /* We didn't reach a terminating character
     * so the json we recieved was incomplete */
    return JTOK_PARSE_STATUS_PARTIAL_TOKEN;
}


bool jtok_toktokcmp_primitive(const jtok_tkn_t *tkn1, const jtok_tkn_t *tkn2)
{
    /** @todo really not proud of this function. SUPER inefficient
     *  But it works for now. Really we should be storing some sort of
     * "value subtype" as part of the parsing process and checking that here
     *
     * Currently we're pretty much re-parsing each token to determine a subtype
     */

    bool           is_equal = false;
    uint_least16_t len1     = jtok_toklen(tkn1);
    uint_least16_t len2     = jtok_toklen(tkn2);
    uint_least16_t min_len  = len1;
    if (min_len > len2)
    {
        min_len = len2;
    }


    const char *start1 = &tkn1->json[tkn1->start];
    const char *start2 = &tkn2->json[tkn2->start];

    if (0 == strncmp("true", start1, min_len) &&
        0 == strncmp("true", start2, min_len))
    {
        is_equal = true;
    }
    else if (0 == strncmp("false", start1, min_len) &&
             0 == strncmp("false", start2, min_len))
    {
        is_equal = true;
    }
    else if (0 == strncmp("null", start1, min_len) &&
             0 == strncmp("null", start2, min_len))
    {
        is_equal = true;
    }
    else
    {
        /* Token isn't 'true', 'false', or 'null', need to parse it as a number
         *
         * Again, since we don't store the subtypes, we have to assume
         * its a real number and do all the operations with ungodly
         * floating precision arithmetic
         */
        const char *end1 = &tkn1->json[tkn1->end];
        const char *end2 = &tkn2->json[tkn2->end];

        float val1;
        float val2;
        char *endptr1 = (char *)start1;
        char *endptr2 = (char *)start2;

        val1 = strtof(start1, &endptr1);
        val2 = strtof(start2, &endptr2);
        if (endptr2 == end2) /* if first tkn was parsed correctly */
        {
            if (endptr1 == end1) /* if second tkn was parsed correctly */
            {
                if ((val1 - val2) < FLT_EPSILON)
                {
                    is_equal = true;
                }
            }
        }
    }

    return is_equal;
}