/**
 * @file negative_key_check.test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Negative Unit tests for jtok_object_has_key
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


static const keyMatcher expectedFalseCases[] = {
    {.json = "{\"abcd\" : 1}", .key = "never"},
    {.json = "{\"abcd\" : 1, \"efgh\" : 2, \"ijkl\": \"abcd\"}",
     .key  = "gonna"},
    {.json = "{\"abcd\" : 1, \"efgh\" : 2, \"ijkl\": \"abcd\"}", .key = "give"},
    {.json = "{\"abcd\" : 1, \"efgh\" : 2, \"ijkl\": \"abcd\"}", .key = "you"},
    {.json = "{\"123\"  : 1}", .key = "up"},
    {.json = "{\"spaces in here\"  : 1}", .key = "rick"},
    {.json = "{\"sub object\"  : { \"a\":1,\"b\":2}}", .key = "rolled"},

};


int main(void)
{
    long i;
    long max_i = sizeof(expectedFalseCases) / sizeof(*expectedFalseCases);

    char *json;
    char *key;
    for (i = 0; i < max_i; ++i)
    {
        memset(tokens, 0, sizeof(tokens));
        json = (char *)expectedFalseCases[i].json;
        key  = (char *)expectedFalseCases[i].key;
        printf("Checking if %s contains key %s...\n", json, key);
        if (JTOK_PARSE_STATUS_OK != jtok_parse(json, tokens, sizeof(tokens)))
        {
            printf("Failed. %s is not a valid json!!\n", json);
        }
        else
        {
            jtok_tkn_t *keyAddr = jtok_obj_has_key(tokens, key);
            if (keyAddr != NULL)
            {
                return -1;
            }
        }
    }
    return 0;
}