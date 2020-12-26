/**
 * @file jtok_object.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to handle jtok object operations
 * @version 0.1
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 * @note
 */

#include "jtok_object.h"
#include "jtok_array.h"
#include "jtok_primitive.h"
#include "jtok_string.h"
#include "jtok_shared.h"


JTOK_PARSE_STATUS_t jtok_parse_object(jtok_parser_t *parser, jtok_tkn_t *tokens,
                                      size_t num_tokens)
{
    JTOK_PARSE_STATUS_t status = JTOK_PARSE_STATUS_PARSE_OK;

    unsigned int start = parser->pos;
    const char * json  = parser->json;
    unsigned int len   = parser->json_len;
    enum
    {
        OBJECT_KEY,
        OBJECT_COLON,
        OBJECT_VALUE,
        OBJECT_COMMA,
    } expecting = OBJECT_KEY;

    if (tokens == NULL)
    {
        return status;
    }
    else if (json[parser->pos] != '{')
    {
        return JTOK_PARSE_STATUS_NON_OBJECT;
    }

    jtok_tkn_t *token = jtok_alloc_token(parser, tokens, num_tokens);
    if (token == NULL)
    {
        /*
         * Do not reset parser->pos because we want
         * caller to see which token maxed out the
         * pool
         */
        status = JTOK_PARSE_STATUS_NOMEM;
    }

    /* If the object has a parent key, increase that key's size */
    token->parent          = parser->toksuper;
    parser->toksuper       = parser->toknext - 1;
    int object_token_index = parser->toksuper;

    /* end of token will be populated when we find the closing brace */
    jtok_fill_token(token, JTOK_OBJECT, parser->pos, INVALID_ARRAY_INDEX);

    /* go inside the object */
    parser->pos++;


    for (; parser->pos < len && json[parser->pos] != '\0' &&
           status == JTOK_PARSE_STATUS_PARSE_OK;
         parser->pos++)
    {
        switch (json[parser->pos])
        {
            case '{':
            {
                switch (expecting)
                {
                    case OBJECT_KEY:
                    {
                        status = JTOK_PARSE_STATUS_OBJ_NOKEY;
                    }
                    break;
                    case OBJECT_COLON:
                    {
                        status = JTOK_PARSE_STATUS_VAL_NO_COLON;
                    }
                    break;
                    case OBJECT_VALUE:
                    {
                        int super = parser->toksuper;
                        status = jtok_parse_object(parser, tokens, num_tokens);
                        if (status == JTOK_PARSE_STATUS_PARSE_OK)
                        {
                            if (super != NO_PARENT_IDX)
                            {
                                tokens[super].size++;
                            }
                            parser->toksuper = super;
                            expecting        = OBJECT_COMMA;
                        }
                    }
                    break;
                    case OBJECT_COMMA:
                    default:
                    {
                        status = JTOK_PARSE_STATUS_INVAL;
                    }
                    break;
                }
            }
            break;
            case '[':
            {
                switch (expecting)
                {
                    case OBJECT_KEY:
                    {
                        status = JTOK_PARSE_STATUS_OBJ_NOKEY;
                    }
                    break;
                    case OBJECT_COLON:
                    {
                        status = JTOK_PARSE_STATUS_VAL_NO_COLON;
                    }
                    break;
                    case OBJECT_VALUE:
                    {
                        int super = parser->toksuper;
                        status = jtok_parse_array(parser, tokens, num_tokens);

                        if (status == JTOK_PARSE_STATUS_PARSE_OK)
                        {
                            if (super != NO_PARENT_IDX)
                            {
                                tokens[super].size++;
                            }
                            else
                            {
                                /* Keys must have a parent token */
                                status = JTOK_PARSE_STATUS_INVALID_PARENT;
                            }

                            expecting = OBJECT_COMMA;
                        }
                    }
                    break;
                    case OBJECT_COMMA:
                    {
                        status = JTOK_PARSE_STATUS_INVAL;
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
            case '}':
            {
                switch (expecting)
                {
                    /* Technically we should be checking if this is the very
                     * first token in the object, because
                     * {"key1" : "value1", "key2" : "value2",} is invalid
                     * from the trailing comma
                     * (transition to expecting==key only occurs when finding
                     * comma or when we START parsing the object)
                     *
                     * So in cases where we have transitioned from a comma,
                     * if we find '}' then it means we have a trailing
                     * comma inside the object
                     */
                    case OBJECT_KEY:
                    {
                        jtok_tkn_t *parent = &tokens[object_token_index];
                        if (parent->type != JTOK_OBJECT || parser->toknext == 0)
                        {
                            parser->pos = start;
                            status      = JTOK_PARSE_STATUS_INVAL;
                        }
                        else
                        {
                            parent->end      = parser->pos + 1;
                            parser->toksuper = parent->parent;
                        }
                        return status;
                    }
                    break;

                    /* instead of a comma, we can find end of object '}' */
                    case OBJECT_COMMA:
                    {
                        jtok_tkn_t *parent = &tokens[object_token_index];
                        if (parent->type != JTOK_OBJECT || parser->toknext == 0)
                        {
                            parser->pos = start;
                            status      = JTOK_PARSE_STATUS_INVAL;
                        }
                        else
                        {
                            parent->end      = parser->pos + 1;
                            parser->toksuper = parent->parent;
                        }
                        return status;
                    }
                    break;
                    case OBJECT_COLON:
                    {
                        status = JTOK_PARSE_STATUS_KEY_NO_VAL;
                    }
                    break;
                    case OBJECT_VALUE:
                    {
                        status = JTOK_PARSE_STATUS_KEY_NO_VAL;
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
            case '\"':
            {
                jtok_tkn_t *parent = &tokens[parser->toksuper];
                switch (expecting)
                {
                    case OBJECT_KEY:
                    {
                        if (parent->type == JTOK_OBJECT)
                        {
                            int super = parser->toksuper;
                            status =
                                jtok_parse_string(parser, tokens, num_tokens);
                            if (status == JTOK_PARSE_STATUS_PARSE_OK)
                            {
                                tokens[super].size++;
                            }
                            expecting = OBJECT_COLON;
                        }
                        else
                        {
                            status = JTOK_PARSE_STATUS_INVALID_PARENT;
                        }
                    }
                    break;
                    case OBJECT_VALUE:
                    {
                        if (parent->type == JTOK_STRING)
                        {
                            if (parent->size != 0)
                            {
                                /* an object key can only have 1 value */
                                status = JTOK_PARSE_STATUS_KEY_MULTIPLE_VAL;
                            }
                            else
                            {
                                int super = parser->toksuper;
                                status    = jtok_parse_string(parser, tokens,
                                                           num_tokens);
                                if (status == JTOK_PARSE_STATUS_PARSE_OK)
                                {
                                    tokens[super].size++;
                                }
                                expecting = OBJECT_COMMA;
                            }
                        }
                        else
                        {
                            status = JTOK_PARSE_STATUS_INVALID_PARENT;
                        }
                    }
                    break;
                    case OBJECT_COLON: /* found " when expecting ':' */
                    {
                        status = JTOK_PARSE_STATUS_VAL_NO_COLON;
                    }
                    break;
                    case OBJECT_COMMA: /* found " when expecting ',' */
                    {
                        status = JTOK_PARSE_STATUS_VAL_NO_COMMA;
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
            case ':':
            {
                if (expecting == OBJECT_COLON)
                {
                    expecting        = OBJECT_VALUE;
                    parser->toksuper = parser->toknext - 1;
                }
                else
                {
                    parser->pos = start;
                    status      = JTOK_PARSE_STATUS_INVAL;
                }
            }
            break;
            case ',':
            {
                if (expecting == OBJECT_COMMA)
                {
                    expecting          = OBJECT_KEY;
                    jtok_tkn_t *parent = &tokens[parser->toksuper];
                    switch (parent->type)
                    {
                        case JTOK_PRIMITIVE:
                        case JTOK_STRING:
                        {
                            /*
                             * Update the toksuper index to
                             * the KEY's parent (the current object)
                             */
                            parser->toksuper = tokens[parser->toksuper].parent;
                        }
                        break;
                        default:
                        {
                        }
                        break;
                    }
                }
                else
                {
                    status = JTOK_PARSE_STATUS_OBJ_NOKEY;
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
                /* We must be expecting a value */
                if (expecting == OBJECT_VALUE)
                {
                    /* We're at the start of a primitive so validate parent type
                     */
                    jtok_tkn_t *parent = &tokens[parser->toksuper];
                    switch (parent->type)
                    {
                        case JTOK_OBJECT:
                        {
                            /* primitives cannot be keys (they are not
                             * quoted)
                             */
                            parser->pos = start;
                            status      = JTOK_PARSE_STATUS_INVAL;
                        }
                        case JTOK_STRING:
                            if (parent->size != 0)
                            {
                                /* an object key can only have 1 value */
                                parser->pos = start;
                                status      = JTOK_PARSE_STATUS_INVAL;
                            }
                            break;
                        default:
                        {
                            /*
                             * If we're inside parse_object,
                             * other types cannot be parent tokens
                             */
                            status = JTOK_PARSE_STATUS_INVAL;
                        }
                        break;
                    }

                    if (status == JTOK_PARSE_STATUS_PARSE_OK)
                    {
                        status =
                            jtok_parse_primitive(parser, tokens, num_tokens);
                        if (status == JTOK_PARSE_STATUS_PARSE_OK)
                        {
                            if (parser->toksuper != NO_PARENT_IDX)
                            {
                                tokens[parser->toksuper].size++;
                            }
                            expecting = OBJECT_COMMA;
                        }
                    }
                }
                else
                {
                    /* move pos to start of the key that's missing the value */
                    parser->pos = tokens[parser->toksuper].start;
                    status      = JTOK_PARSE_STATUS_KEY_NO_VAL;
                }
            }
            break;
            default: /* unexpected character */
            {
                parser->pos = start;
                status      = JTOK_PARSE_STATUS_INVAL;
            }
            break;
        } /* end of character switch statement */
    }

    if (status == JTOK_PARSE_STATUS_PARSE_OK)
    {
        /* If we didnt find the } to close current object, we have partial JSON
         */
        parser->pos = start;
        status      = JTOK_PARSE_STATUS_PARTIAL_TOKEN;
    }

    return status;
}


bool jtok_toktokcmp_object(const jtok_tkn_t *obj1, const jtok_tkn_t *obj2)
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


    /** Right now, we aren't even able to easily compare objects because
     * they do not contain meta-information regarding the number of
     * jtok tokens INSIDE them
     *
     * The ownership tree is constructed from the leaves -> root in the
     * form of child -> parent.
     *
     * So if you're at a given node, you can always walk to the top level
     * node, but you cannot descend to child nodes (either with BFS or DFS)
     *
     * The meta-information simply is not contained in the current
     * conceptualization of the data structure
     *
     * @todo This function will require some deep thought...
     */
    return is_equal;
}