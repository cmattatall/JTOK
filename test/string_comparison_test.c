/**
 * @file string_comparison_test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to test comparison json tokens
 * @version 0.1
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 * @note
 */

#include "jtok.h"

#define TOKEN_MAX 200

static struct
{
    char json1[250];
    char json2[250];
} json_table[] = {
    {.json1 = "{\"key\" : \"value\"}", .json2 = "{\"key\":\"value\"}"},
    {.json1 = "{ \"key\" : \"value\"}", .json2 = "{\"key\" :\"value\"}"},
    {.json1 = "{\"key\" : \"value\"}", .json2 = "{  \"key\" :\"value\"  }"},
};


static jtok_tkn_t    tokens1[TOKEN_MAX];
static jtok_tkn_t    tokens2[TOKEN_MAX];
static jtok_parser_t p1;
static jtok_parser_t p2;

int main(void)
{
    int i;
    int max_i = sizeof(json_table) / sizeof(*json_table);

    JTOK_PARSE_STATUS_t status;
    for (i = 0; i < max_i; i++)
    {
        bool passed = true;
        p1          = jtok_new_parser(json_table[i].json1);
        printf("\ncomparing %s and %s... ", json_table[i].json2,
               json_table[i].json1);

        status = jtok_parse(&p1, tokens1, TOKEN_MAX);
        if (status != JTOK_PARSE_STATUS_PARSE_OK)
        {
            passed = false;
        }

        p2 = jtok_new_parser(json_table[i].json1);
        if (passed)
        {
            status = jtok_parse(&p2, tokens2, TOKEN_MAX);
            if (status != JTOK_PARSE_STATUS_PARSE_OK)
            {
                passed = false;
            }


            if (passed)
            {
                /* Compare "key" with "key" */
                if (!jtok_toktokcmp(&tokens1[1], &tokens2[1]))
                {
                    passed = false;
                }

                /* Compare "value" with "value" */
                if (!jtok_toktokcmp(&tokens1[2], &tokens2[2]))
                {
                    passed = false;
                }
            }
        }

        if (passed)
        {
            printf("passed.");
            continue;
        }
        else
        {
            printf("failed.");
            return status;
        }
        printf("\n");
    }
    return 0;
}