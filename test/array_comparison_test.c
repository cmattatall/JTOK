/**
 * @file object_comparison_test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to test comparison semantics on jtok arrays
 * @version 0.1
 * @date 2020-12-26
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 * @note
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

    /* Arrays are equal only if they have the same elements AND order */
    {.json1 = "{\"arr\":[1,2,3]}", .json2 = "{\"arr\":[1,2,3]}"},
};
/* clang-format on */

static jtok_tkn_t    tokens1[TOKEN_MAX];
static jtok_tkn_t    tokens2[TOKEN_MAX];
static jtok_parser_t p1;
static jtok_parser_t p2;

int main(void)
{
    int i;
    int max_i = sizeof(true_cmp_table) / sizeof(*true_cmp_table);

    JTOK_PARSE_STATUS_t status;
    for (i = 0; i < max_i; i++)
    {
        bool passed = true;
        p1          = jtok_new_parser(true_cmp_table[i].json1);
        printf("\ncomparing %s and %s... ", true_cmp_table[i].json2,
               true_cmp_table[i].json1);

        status = jtok_parse(&p1, tokens1, TOKEN_MAX);
        if (status != JTOK_PARSE_STATUS_PARSE_OK)
        {
            passed = false;
        }

        p2 = jtok_new_parser(true_cmp_table[i].json1);
        if (passed)
        {
            status = jtok_parse(&p2, tokens2, TOKEN_MAX);
            if (status != JTOK_PARSE_STATUS_PARSE_OK)
            {
                passed = false;
            }


            if (passed)
            {
                if (!jtok_toktokcmp(tokens1, tokens2))
                {
                    passed = false;
                }
            }
        }


        if (passed)
        {
            printf("passed.\n");
            continue;
        }
        else
        {
            printf("failed.\n");
            return status;
        }
    }
    return 0;
}
