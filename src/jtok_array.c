/**
 * @file jtok_array.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Internal module for handling jtok arrays
 * @version 0.1
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 */

#include <assert.h>

#include "jtok_array.h"
#include "jtok_object.h"
#include "jtok_shared.h"
#include "jtok_string.h"
#include "jtok_primitive.h"

JTOK_PARSE_STATUS_t jtok_parse_array(jtok_parser_t *parser, int depth)
{
    JTOK_PARSE_STATUS_t status             = JTOK_PARSE_STATUS_OK;
    jtok_tkn_t *        tokens             = parser->tkn_pool;
    unsigned int        start              = parser->pos;
    const char *        json               = parser->json;
    bool                element_type_found = false;
    JTOK_TYPE_t         element_type       = JTOK_UNASSIGNED_TOKEN;
    enum
    {
        ARRAY_START,
        ARRAY_VALUE,
        ARRAY_COMMA
    } expecting = ARRAY_START;

    if (depth > JTOK_MAX_RECURSE_DEPTH)
    {
        status = JTOK_PARSE_STATUS_NEST_DEPTH_EXCEEDED;
        return status;
    }

    if (json[parser->pos] != '[')
    {
        return JTOK_PARSE_STATUS_NON_ARRAY;
    }

    jtok_tkn_t *token = jtok_alloc_token(parser);
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

    /* Index of current top level token is the current superior token */
    /* We need to preserve the index of top level token in the currenet
     * stack frame because parsing a sub-object changes the value of
     * parser->toksuper
     */
    int array_token_index = parser->toksuper;

    /* end of token will be populated when we find the closing brace */
    jtok_fill_token(token, JTOK_ARRAY, parser->pos, INVALID_ARRAY_INDEX);

    /* go inside the object */
    parser->pos++;

    /* all arrays start with no children (since they can be empty) */
    parser->last_child = NO_CHILD_IDX;

    for (; parser->pos < parser->json_len && json[parser->pos] != '\0' &&
           status == JTOK_PARSE_STATUS_OK;
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

                        int parent_array_idx = parser->toksuper;
                        status = jtok_parse_object(parser, depth + 1);
                        if (status == JTOK_PARSE_STATUS_OK)
                        {
                            if (parser->last_child != NO_CHILD_IDX)
                            {
                                /* Link previous child to current child */
                                tokens[parser->last_child].sibling =
                                    parser->toknext - 1;
                            }

                            /* Update last child and increase parent size */
                            parser->last_child = parser->toknext - 1;
                            tokens[parent_array_idx].size++;

                            expecting = ARRAY_COMMA;

                            /* Restore superior token node */
                            parser->toksuper = parent_array_idx;
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
            case '[':
            {
                switch (expecting)
                {
                    case ARRAY_START:
                    case ARRAY_VALUE:
                    {
                        if (element_type_found)
                        {
                            if (element_type != JTOK_ARRAY)
                            {
                                status = JTOK_STATUS_MIXED_ARRAY;
                            }
                        }
                        else
                        {
                            element_type_found = true;
                            element_type       = JTOK_ARRAY;
                        }

                        int parent_array_idx = parser->toksuper;
                        status = jtok_parse_array(parser, depth + 1);
                        if (status == JTOK_PARSE_STATUS_OK)
                        {
                            if (parser->last_child != NO_CHILD_IDX)
                            {
                                /* Link previous child to current child */
                                tokens[parser->last_child].sibling =
                                    parser->toknext - 1;
                            }

                            /* Update last child and increase parent size */
                            parser->last_child = parser->toknext - 1;
                            tokens[parent_array_idx].size++;

                            expecting = ARRAY_COMMA;

                            /* Restore superior token node */
                            parser->toksuper = parent_array_idx;
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
            case ']':
            {
                switch (expecting)
                {
                    case ARRAY_COMMA:
                    case ARRAY_START:
                    {
                        jtok_tkn_t *parent_arr = &tokens[array_token_index];
                        if (parent_arr->type != JTOK_ARRAY ||
                            parser->toknext == 0)
                        {
                            parser->pos = start;
                            status      = JTOK_PARSE_STATUS_INVAL;
                        }
                        else
                        {
                            parent_arr->end  = parser->pos + 1;
                            parser->toksuper = parent_arr->parent;
                        }

                        return status;
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
                        status    = jtok_parse_string(parser);
                        if (status == JTOK_PARSE_STATUS_OK)
                        {
                            if (parser->last_child != NO_CHILD_IDX)
                            {
                                /* Link previous child to current child */
                                tokens[parser->last_child].sibling =
                                    parser->toknext - 1;
                            }

                            /* Update last child and increase parent size */
                            parser->last_child = parser->toknext - 1;
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

                        if (status == JTOK_PARSE_STATUS_OK)
                        {
                            int super = parser->toksuper;
                            status    = jtok_parse_primitive(parser);
                            if (status == JTOK_PARSE_STATUS_OK)
                            {
                                if (parser->last_child != NO_CHILD_IDX)
                                {
                                    /* Link previous child to current child
                                     */
                                    tokens[parser->last_child].sibling =
                                        parser->toknext - 1;
                                }

                                /* Update last child and increase parent
                                 * size */
                                parser->last_child = parser->toknext - 1;
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

    if (status == JTOK_PARSE_STATUS_OK)
    {
        parser->pos = start;
        status      = JTOK_PARSE_STATUS_PARTIAL_TOKEN;
    }

    return status;
}


bool jtok_toktokcmp_array(const jtok_tkn_t *arr1, const jtok_tkn_t *arr2)
{
    bool                    is_equal = true;
    const jtok_tkn_t *const pool1    = arr1->pool;
    const jtok_tkn_t *const pool2    = arr2->pool;
    assert(pool1->type == JTOK_OBJECT);
    assert(pool2->type == JTOK_OBJECT);
    if (arr1->type != JTOK_ARRAY || arr2->type != JTOK_ARRAY)
    {
        is_equal = false;
    }
    else
    {
        if (arr1->size != arr2->size)
        {
            is_equal = false;
        }
        else if (arr1->size > 0)
        {
            /* Arrays have equal size but they aren't empty,
             * so we'll have to compare their elements */
            jtok_tkn_t *child1 = (jtok_tkn_t *)&arr1[1];
            jtok_tkn_t *child2 = (jtok_tkn_t *)&arr2[1];
            int         i;
            for (i = 0; i < arr1->size && is_equal; i++)
            {
                /* Check that the Ith element of
                 * first array is equal to Ith
                 * eleemnt of second array */
                if (!jtok_toktokcmp(child1, child2))
                {
                    is_equal = false;
                }

                if (child1->sibling == NO_SIBLING_IDX)
                {
                    /* If this assertion fails there is a logic error
                     * in the actual parser itself */
                    assert(child2->sibling == NO_SIBLING_IDX);
                    break;
                }
                else
                {
                    /* Go to next array element */
                    child1 = (jtok_tkn_t *)&pool1[child1->sibling];
                    child2 = (jtok_tkn_t *)&pool2[child2->sibling];
                }
            }

            if (i == arr1->size)
            {
                is_equal = false;
            }
        }
    }


    return is_equal;
}
