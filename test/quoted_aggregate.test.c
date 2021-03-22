/**
 * @file quoted_aggregate.test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to make sure that quoted aggregate types
 * are parsed as valid
 * @version 0.1
 * @date 2021-03-22
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 */


#include <stdio.h>

#include "jtok.h"

#define TOKEN_MAX (200u)

static const char *valid_jsons[] = {
    "{\'key\': \"{\'n\' : [ \'abc\', \'def\', \'ghi\']}\"}",
    "{\'key\': \"[ 1, 2, 3]\"}",
    "{\'key\': \"[ true, false, null]\"}",
    "{\'key\': \"[ \'true\', \'false\', \'null\']\"}",
    "{\'key\': \"{}\"}",
    "{\'key\': \"[]\"}",
    "{\'key\': \"[1]\"}",
    "{\'key\': \"[\'1\']\"}",
    "{\'key\': \"[\'true\']\"}",
    "{\'key\': \"[\'false\']\"}",
    "{\'key\': \"[\'null\']\"}",
    "{\'key\': \"[\'{}\', \'{}\']\"}",
    "{\'key\': \"{\'nested json key\' : [ \'abc\', \'def\', \'ghi\']}\"}",
    "{\'key\': \"[ 1, 2, 3]\"}",
    "{\'key\': \"[ true, false, null]\"}",
    "{\'key\': \"[ \'true\', \'false\', \'null\']\"}",
    "{\'key\': \"{}\"}",
    "{\'key\': \"[]\"}",
    "{\'key\': \"[1]\"}",
    "{\'key\': \"[\'1\']\"}",
    "{\'key\': \"[\'true\']\"}",
    "{\'key\': \"[\'false\']\"}",
    "{\'key\': \"[\'null\']\"}",
    "{\'key\': \"[\'{}\', \'{}\']\"}",
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