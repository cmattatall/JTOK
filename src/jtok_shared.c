/**
 * @file jtok_shared.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module for private shared jtok functionality
 * @version 0.1
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 * @note
 */

#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>

#include "jtok_shared.h"


int jtok_fill_token(jtoktok_t *token, JTOK_TYPE_t type, int start, int end)
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


jtoktok_t *jtok_alloc_token(jtok_parser_t *parser, jtoktok_t *tokens,
                            size_t num_tokens)
{
    jtoktok_t *tok;
    if (parser->toknext >= num_tokens)
    {
        return NULL;
    }
    tok        = &tokens[parser->toknext++];
    tok->start = tok->end = INVALID_ARRAY_INDEX;
    tok->size             = 0;
    tok->parent           = NO_PARENT_IDX;
    tok->json             = parser->json;
    return tok;
}
