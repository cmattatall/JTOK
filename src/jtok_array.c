/**
 * @file jtok_array.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Internal module for handling jtok arrays
 * @version 0.1
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 * @note
 */

#include "jtok_array.h"
#include "jtok_object.h"
#include "jtok_shared.h"
#include "jtok_string.h"
#include "jtok_primitive.h"


JTOK_PARSE_STATUS_t jtok_parse_array(jtok_parser_t *parser, jtoktok_t *tokens,
                                     size_t num_tokens)
{
    JTOK_PARSE_STATUS_t status = JTOK_PARSE_STATUS_PARSE_OK;
    unsigned int        start  = parser->pos;
    const char *        json   = parser->json;

    bool        element_type_found = false;
    JTOK_TYPE_t element_type       = JTOK_UNASSIGNED_TOKEN;
    enum
    {
        ARRAY_START,
        ARRAY_VALUE,
        ARRAY_COMMA
    } expecting = ARRAY_START;

    if (tokens == NULL)
    {
        return status;
    }
    else if (json[parser->pos] != '[')
    {
        return JTOK_PARSE_STATUS_NON_ARRAY;
    }

    jtoktok_t *token = jtok_alloc_token(parser, tokens, num_tokens);
    if (token == NULL)
    {
        /*
         * Do not reset parser->pos because we want
         * caller to see which token maxed out the
         * pool
         */
        status = JTOK_PARSE_STATUS_NOMEM;
    }

    token->parent    = parser->toksuper;
    parser->toksuper = parser->toknext - 1;

    /* end of token will be populated when we find the closing brace */
    jtok_fill_token(token, JTOK_ARRAY, parser->pos, INVALID_ARRAY_INDEX);

    /* go inside the object */
    parser->pos++;

    for (; parser->pos < parser->json_len && json[parser->pos] != '\0' &&
           status == JTOK_PARSE_STATUS_PARSE_OK;
         parser->pos++)
    {
        switch (json[parser->pos])
        {
            case '{':
            {
                switch (expecting)
                {
                    case ARRAY_START:
                    case ARRAY_VALUE:
                    {
                        if (element_type_found)
                        {
                            if (element_type != JTOK_OBJECT)
                            {
                                status = JTOK_STATUS_MIXED_ARRAY;
                            }
                        }
                        else
                        {
                            element_type_found = true;
                            element_type       = JTOK_OBJECT;
                        }

                        int super = parser->toksuper;
                        status = jtok_parse_object(parser, tokens, num_tokens);
                        if (status == JTOK_PARSE_STATUS_PARSE_OK)
                        {
                            if (super != NO_PARENT_IDX)
                            {
                                tokens[super].size++;
                            }
                            expecting        = ARRAY_COMMA;
                            parser->toksuper = super;
                        }
                    }
                    break;
                    case ARRAY_COMMA:
                    {
                        status = JTOK_PARSE_STATUS_ARRAY_SEPARATOR;
                    }
                    break;
                    default:
                    {
                        status = JTOK_PARSE_STATUS_INVAL;
                    }
                    break;
                }
            }
            break;

#ifdef SUBARRAYS
#error SUBARRAYS ARE CURRENTLY NOT IMPLEMENTED
            case '[':
            {
            }
            break;
#endif
            case ']':
            {
                switch (expecting)
                {
                    case ARRAY_COMMA:
                    case ARRAY_START:
                    {
                        /* Do nothing */
                    }
                    break;
                    default:
                    {
                        status = JTOK_PARSE_STATUS_ARRAY_SEPARATOR;
                    }
                    break;
                }
                return status;
            }
            break;
            case '\"':
            {
                switch (expecting)
                {
                    case ARRAY_START:
                    case ARRAY_VALUE:
                    {
                        if (element_type_found)
                        {
                            if (element_type != JTOK_OBJECT)
                            {
                                status = JTOK_STATUS_MIXED_ARRAY;
                            }
                        }
                        else
                        {
                            element_type_found = true;
                            element_type       = JTOK_OBJECT;
                        }

                        int super = parser->toksuper;
                        status = jtok_parse_string(parser, tokens, num_tokens);
                        if (status == JTOK_PARSE_STATUS_PARSE_OK)
                        {
                            if (super != NO_PARENT_IDX)
                            {
                                tokens[super].size++;
                            }

                            expecting = ARRAY_COMMA;
                        }
                    }
                    break;
                    case ARRAY_COMMA:
                    {
                        status = JTOK_PARSE_STATUS_ARRAY_SEPARATOR;
                    }
                    break;
                    default:
                    {
                        status = JTOK_PARSE_STATUS_INVAL;
                    }
                    break;
                }
            }
            break;
            case '\t':
            case '\r':
            case '\n':
            case ' ':
                continue; /* skip whitespce */
            case ',':
            {
                switch (expecting)
                {
                    case ARRAY_COMMA:
                    {
                        expecting = ARRAY_VALUE;
                    }
                    break;
                    case ARRAY_START:
                    case ARRAY_VALUE:
                    {
                        status = JTOK_PARSE_STATUS_STRAY_COMMA;
                    }
                    break;
                    default:
                    {
                        status = JTOK_PARSE_STATUS_INVAL;
                    }
                    break;
                }
            }
            break;
            case '+':
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
            {
                switch (expecting)
                {
                    case ARRAY_START:
                    case ARRAY_VALUE:
                    {
                        if (element_type_found)
                        {
                            if (element_type != JTOK_PRIMITIVE)
                            {
                                status = JTOK_STATUS_MIXED_ARRAY;
                            }
                        }
                        else
                        {
                            element_type_found = true;
                            element_type       = JTOK_PRIMITIVE;
                        }

                        if (status == JTOK_PARSE_STATUS_PARSE_OK)
                        {
                            int super = parser->toksuper;
                            status    = jtok_parse_primitive(parser, tokens,
                                                          num_tokens);
                            if (status == JTOK_PARSE_STATUS_PARSE_OK)
                            {
                                if (super != NO_PARENT_IDX)
                                {
                                    tokens[super].size++;
                                }

                                expecting = ARRAY_COMMA;
                            }
                            parser->toksuper = super;
                        }
                    }
                    break;
                    case ARRAY_COMMA:
                    {
                        status = JTOK_PARSE_STATUS_STRAY_COMMA;
                    }
                    break;
                    default:
                    {
                        status = JTOK_PARSE_STATUS_INVAL;
                    }
                    break;
                }
            }
            break;
        }
    }

    if (status == JTOK_PARSE_STATUS_PARSE_OK)
    {
        parser->pos = start;
        status      = JTOK_PARSE_STATUS_PARTIAL_TOKEN;
    }

    return status;
}


bool jtok_toktokcmp_array(const jtoktok_t *tkn1, const jtoktok_t *tkn2)
{
    bool is_equal = false;

    /** @todo THIS IS GOING TO BE DIFFICULT... */

    /** How do we even conceptualize equality semantics for objects */

    /** For example, what if tkn1 has all the fields of tkn2 in ADDITION
     * to extra fields...
     * should tkn1 == tkn2 evaluate to true?
     *
     * Or should it be some sort of setwise comparison, where
     *      tkn1 "equal as subset" tkn2 evaluates to true,
     *      but
     *      tkn2 "equal as subset" tkn1 evaluates to false?
     *
     * What if the objects have all the same keys, but the values are
     * different?
     *
     * What if objects contain arrays that individually contain all the
     * same values, but are ordered differently.
     *      In that case, for an array of strings, it may not matter,
     *      but for an array of numbers, or even SUBOBJECTS, the
     *      ordering may matter greatly...
     */

    return is_equal;
}
