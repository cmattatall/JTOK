/**
 * @file key_check.test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Positive Unit tests for jtok_object_has_key
 * @version 0.1
 * @date 2021-03-28
 *
 * @copyright Copyright (c) 2021 Carl Mattatall
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "jtok.h"

#define TOKEN_MAX 200

static jtok_tkn_t tokens[TOKEN_MAX];

typedef struct
{
    char json[250];
    char key[25];
} keyMatcher;


static const keyMatcher expectedTrueCases[] = {
    {.json = "{\"abcd\" : 1}", .key = "abcd"},
    {.json = "{\"abcd\" : 1, \"efgh\" : 2, \"ijkl\": \"abcd\"}", .key = "abcd"},
    {.json = "{\"abcd\" : 1, \"efgh\" : 2, \"ijkl\": \"abcd\"}", .key = "efgh"},
    {.json = "{\"abcd\" : 1, \"efgh\" : 2, \"ijkl\": \"abcd\"}", .key = "ijkl"},
    {.json = "{\"123\"  : 1}", .key = "123"},
    {.json = "{\"spaces in here\"  : 1}", .key = "spaces in here"},
    {.json = "{\"sub object\"  : { \"a\":1,\"b\":2}}", .key = "sub object"},
};


int main(void)
{
    long i;
    long max_i = sizeof(expectedTrueCases) / sizeof(*expectedTrueCases);

    char *json;
    char *key;
    for (i = 0; i < max_i; ++i)
    {
        memset(tokens, 0, sizeof(tokens));
        json = (char *)expectedTrueCases[i].json;
        key  = (char *)expectedTrueCases[i].key;
        printf("Checking if %s contains key %s...\n", json, key);
        if (JTOK_PARSE_STATUS_OK != jtok_parse(json, tokens, sizeof(tokens)))
        {
            printf("Failed. %s is not a valid json!!\n", json);
        }
        else
        {
            jtok_tkn_t *keyAddr = jtok_obj_has_key(tokens, key);
            if (keyAddr == NULL)
            {
                return -1;
            }
        }
    }
    return 0;
}