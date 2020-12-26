/**
 * @file object_comparison_test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to test comparison semantics on jtok objects
 * @version 0.1
 * @date 2020-12-26
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 * @note
 * Object comparison semantics should behave as follows:
 *
 * Objects are equal ONLY if:
 *  - they have the same child keys (in any order)
 *  - their child keys have the same order
 *
 * This means that although A may be a subset of B, A is not equal to B
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
    /* swapped order comparisons */
    {.json1 = "{\"key1\":[],\"key2\":[]}", .json2 = "{\"key2\":[],\"key1\":[]}"},
    {.json1 = "{\"key1\":1,\"key2\":2}", .json2 = "{\"key2\":2,\"key1\":1}"},
    {.json1 = "{\"key1\":{},\"key2\":{}}", .json2 = "{\"key2\":{},\"key1\":{}}"},
    {.json1 = "{\"key1\":\"1\",\"key2\":\"2\"}", .json2 = "{\"key2\":\"2\",\"key1\":\"1\"}"}, 
};
/* clang-format on */

static jtok_tkn_t    tokens1[TOKEN_MAX];
static jtok_tkn_t    tokens2[TOKEN_MAX];
static jtok_parser_t p1;
static jtok_parser_t p2;

int main(void)
{
    unsigned long long i;
    unsigned long long max_i = sizeof(true_cmp_table) / sizeof(*true_cmp_table);

    JTOK_PARSE_STATUS_t status;
    for (i = 0; i < max_i; i++)
    {
        p1 = jtok_new_parser(true_cmp_table[i].json1);
        p2 = jtok_new_parser(true_cmp_table[i].json2);
        printf("\ncomparing %s and %s... ", true_cmp_table[i].json2,
               true_cmp_table[i].json1);

        status = jtok_parse(&p1, tokens1, TOKEN_MAX);
        if (status != JTOK_PARSE_STATUS_PARSE_OK)
        {
            printf("parse of %s failed with status %d\n",
                   true_cmp_table[i].json1, status);
            return status;
        }

        status = jtok_parse(&p2, tokens2, TOKEN_MAX);
        if (status != JTOK_PARSE_STATUS_PARSE_OK)
        {
            printf("parse of %s failed with status %d\n",
                   true_cmp_table[i].json2, status);
            return status;
        }

        jtok_tkn_t *obj1 = (jtok_tkn_t *)&tokens1[0];
        jtok_tkn_t *obj2 = (jtok_tkn_t *)&tokens2[0];
        if (jtok_toktokcmp(tokens1, obj1, tokens2, obj2))
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
