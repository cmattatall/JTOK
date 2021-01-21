/**
 * @file no_tokens_test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to test API hardening against case where
 * caller provides no tokens
 * @version 0.1
 * @date 2021-01-12
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 */
#include <stdio.h>
#include "jtok.h"

static jtok_tkn_t tokens[5];

int main(void)
{
    if (jtok_parse("{\"key\":\"value\", \"key2\":true}", tokens, 0) !=
        JTOK_PARSE_STATUS_NOMEM)
    {
        return 1;
    }
    return 0;
}