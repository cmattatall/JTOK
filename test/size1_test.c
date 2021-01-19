/**
 * @file size_test1.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Test to check parsed size of json objects
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

static const struct
{
    char         json[250];
    unsigned int size;
} true_table[] = {
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

    {
        .json = "{\"key1\":1, \"key2\":2}",
        .size = 2,
    },

    {
        .json = "{\"key1\":\"1\", \"key2\":\"2\"}",
        .size = 2,
    },

    {
        .json = "{\"key1\":true, \"key2\":false }",
        .size = 2,
    },

    {
        .json = "{\"key1\":[], \"key2\":[]}",
        .size = 2,
    },


    {
        .json = "{\"key1\":{}, \"key2\":{}}",
        .size = 2,
    },

    {
        .json = "{\"key1\":5,\"key2\":{\"childkey1\":1}, \"arrkey\":[1,2,3]}",
        .size = 3,
    },

};


static jtok_tkn_t tokens[TOKEN_MAX];

int main(void)
{
    unsigned long long  i;
    unsigned long long  max_i = sizeof(true_table) / sizeof(*true_table);
    JTOK_PARSE_STATUS_t status;
    for (i = 0; i < max_i; i++)
    {
        printf("\nConfirming that %s has size %u... ", true_table[i].json,
               true_table[i].size);
        status = jtok_parse(true_table[i].json, tokens, TOKEN_MAX);
        if (status != JTOK_PARSE_STATUS_OK)
        {
            printf("parse failed with status %d\n", status);
            return status;
        }
        else
        {
            if (tokens[0].size != true_table[i].size)
            {
                printf("failed. parsed size was %d\n", tokens[0].size);
                return -1;
            }
            else
            {
                printf("passed.\n");
                continue;
            }
        }
    }
    return 0;
}
