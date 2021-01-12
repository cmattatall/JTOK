/**
 * @file not_enough_tokens_test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to test API response when caller does not
 * allocate enough tokens
 * @version 0.1
 * @date 2021-01-12
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 */
#include <stdio.h>
#include "jtok.h"

#define TOKEN_COUNT 3

static jtok_tkn_t tokens[TOKEN_COUNT];

int main(void)
{
    if (jtok_parse("{\"key\":\"value\", \"key2\":true}", tokens, TOKEN_COUNT) !=
        JTOK_PARSE_STATUS_NOMEM)
    {
        return 1;
    }
    return 0;
}