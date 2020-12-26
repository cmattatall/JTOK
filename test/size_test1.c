/**
 * @file size_test1.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Test to check parsed size of json objects
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

static const struct
{
    char         json[250];
    unsigned int size;
} true_table[] = {
    {
        .json = "{\"key1\":5,\"key2\":{\"childkey1\":1}, \"arrkey\":[1,2,3]}",
        .size = 3,
    },

    {
        .json = "{}",
        .size = 0,
    },

    {
        .json = "{\"key\":1}",
        .size = 1,
    },

    {
        .json = "{\"key\":[]}",
        .size = 1,
    },

    {
        .json = "{\"key\":{}}",
        .size = 1,
    },

    {
        .json = "{\"key\":true}",
        .size = 1,
    },

    {
        .json = "{\"key\":false}",
        .size = 1,
    },

    {
        .json = "{\"key\":null}",
        .size = 1,
    },
};


static jtok_tkn_t    tokens[TOKEN_MAX];
static jtok_parser_t p;

int main(void)
{
    unsigned int        i;
    unsigned int        max_i = sizeof(true_table) / sizeof(*true_table);
    JTOK_PARSE_STATUS_t status;
    for (i = 0; i < max_i; i++)
    {
        p = jtok_new_parser(true_table[i].json);
        printf("\nConfirming that %s has size %u... ", true_table[i].json,
               true_table[i].size);
        status = jtok_parse(&p, tokens, TOKEN_MAX);
        if (status != JTOK_PARSE_STATUS_PARSE_OK)
        {
            return status;
        }
        else
        {
            if (tokens[0].size != true_table[i].size)
            {
                return -1;
            }
        }
    }
    return 0;
}
