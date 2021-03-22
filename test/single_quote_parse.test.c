/**
 * @file single_quote_parse.test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to test support for single quote keys
 * @version 0.1
 * @date 2021-03-21
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 */

#include <stdio.h>

#include "jtok.h"

#define TOKEN_MAX (200u)

static const char *valid_jsons[] = {
    "{\'key\' : \'string\'}",           "{\'key\' : 123 }",
    "{\'key\' : [ 1, 2, 3]}",           "{\'key\' : \'true\'}",
    "{\'key\' : [ true, false, null]}",
};

static jtok_tkn_t tokens[TOKEN_MAX];
int               main(void)
{
    unsigned long long i;
    unsigned long long max_i = sizeof(valid_jsons) / sizeof(*valid_jsons);
    for (i = 0; i < max_i; i++)
    {
        printf("\n%s ... ", valid_jsons[i]);
        JTOK_PARSE_STATUS_t status =
            jtok_parse(valid_jsons[i], tokens, TOKEN_MAX);
        if (status != JTOK_PARSE_STATUS_OK)
        {
            printf("failed with status %d.\n", status);
            return 1;
        }
        else
        {
            printf("passed.\n");
            continue;
        }
    }

    return 0;
}