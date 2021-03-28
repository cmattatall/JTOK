/**
 * @file recursion_failure_test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to confirm that parsing fails when parsing nest
 * depth maximums are exceeded
 * @version 0.1
 * @date 2021-01-11
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 */
#include <stdio.h>
#include <string.h>

#include "jtok.h"

#define TOKEN_MAX (200u)

static const char *invalidJSON[] = {
    "{\"key\":[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]"
    "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]}",
};

static jtok_tkn_t tokens[TOKEN_MAX];
int               main(void)
{
    unsigned int i;
    unsigned int max_i = sizeof(invalidJSON) / sizeof(*invalidJSON);
    for (i = 0; i < max_i; i++)
    {
        printf("\n%s ... ", invalidJSON[i]);
        JTOK_PARSE_STATUS_t status;
        status = jtok_parse(invalidJSON[i], tokens, TOKEN_MAX);
        if (status != JTOK_PARSE_STATUS_NEST_DEPTH_EXCEEDED)
        {
            printf("failed.\n");
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