/**
 * @file object_comparison_test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to test comparison semantics on jtok arrays
 * @version 0.1
 * @date 2020-12-26
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 *
 */
#include <stdio.h>

#include "jtok.h"

#define TOKEN_MAX 200

/* clang-format off */
static struct
{
    char json1[250];
    char json2[250];
} true_cmp_table[] = {
    {.json1 = "{\"arr\":[{}, {}, {\"keyInObjectInArray\": \"valueInObjectInArray\"}}]}", .json2 = "{\"arr\":[{}, {}, {\"keyInObjectInArray\": \"valueInObjectInArray\"}]}"},


    {.json1 = "{\"arr\":[1,2,3]}", .json2 = "{\"arr\":[1,2,3]}"},
    {.json1 = "{\"arr\":[true, false, true, null]}", .json2 = "{\"arr\":[true, false, true, null]}"},
    
};
/* clang-format on */

static jtok_tkn_t tokens1[TOKEN_MAX];
static jtok_tkn_t tokens2[TOKEN_MAX];

int main(void)
{
    int i;
    int max_i = sizeof(true_cmp_table) / sizeof(*true_cmp_table);

    JTOK_PARSE_STATUS_t status;
    for (i = 0; i < max_i; i++)
    {
        printf("\ncomparing %s and %s... ", true_cmp_table[i].json2,
               true_cmp_table[i].json1);

        status = jtok_parse(true_cmp_table[i].json1, tokens1, TOKEN_MAX);
        if (status != JTOK_PARSE_STATUS_OK)
        {
            printf("parse of %s failed with status %d\n",
                   true_cmp_table[i].json1, status);
            return status;
        }

        status = jtok_parse(true_cmp_table[i].json2, tokens2, TOKEN_MAX);
        if (status != JTOK_PARSE_STATUS_OK)
        {
            printf("parse of %s failed with status %d\n",
                   true_cmp_table[i].json2, status);
            return status;
        }

        jtok_tkn_t *arr1 = (jtok_tkn_t *)&tokens1[0];
        jtok_tkn_t *arr2 = (jtok_tkn_t *)&tokens2[0];
        if (jtok_toktokcmp(arr1, arr2))
        {
            printf("passed.\n");
            continue;
        }
        else
        {
            printf("failed.\n");

            return -1;
        }
    }
    return 0;
}
