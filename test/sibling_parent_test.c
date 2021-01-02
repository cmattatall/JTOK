/**
 * @file sibling_parent_test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source file to test the sibling->parent relationship for parsed
 * token trees
 * @version 0.1
 * @date 2020-12-31
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 */
#include <stdio.h>

#include "jtok.h"

#define TOKEN_MAX (200u)

static void setup(void);
static void teardown(void);

static const struct
{
    char json[150];
    int  parent[50];
} table[] = {
    {.json = "{\"a\":[[],[],[]]}", .parent = {-1, 0, 1, 2, 2, 2}},
    {.json   = "{\"a\": 1, \"b\":2, \"c\":true, \"d\":{}, \"e\":[]}",
     .parent = {-1, 0, 1, 0, 3, 0, 5, 0, 7, 0, 9}},
    {.json   = "{\"a\":[1,2,3,4], \"b\":{\"bb\":1,\"cc\":false}}",
     .parent = {-1, 0, 1, 2, 2, 2, 2, 0, 7, 8, 9, 8, 11}},
    {.json = "{}", .parent = {-1}},
    {.json = "{\"a\":1}", .parent = {-1, 0, 1}},
    {.json = "{\"a\":true}", .parent = {-1, 0, 1}},
    {.json = "{\"a\":false}", .parent = {-1, 0, 1}},
    {.json = "{\"a\":null}", .parent = {-1, 0, 1}},
    {.json = "{\"a\":\"RandomString\"}", .parent = {-1, 0, 1}},
    {.json = "{\"a\":{}}", .parent = {-1, 0, 1}},
    {.json = "{\"a\":[]}", .parent = {-1, 0, 1}},
    {.json = "{\"a\":1, \"b\":2}", .parent = {-1, 0, 1, 0, 3}},
};

static jtok_tkn_t    tokens[TOKEN_MAX];
int                  main(void)
{
    setup();
    unsigned long long i;
    unsigned long long max_i = sizeof(table) / sizeof(*table);

    for (i = 0; i < max_i; i++)
    {   
        JTOK_PARSE_STATUS_t status;
        status = jtok_parse(table[i].json, tokens, TOKEN_MAX);
        if (JTOK_PARSE_STATUS_OK != status)
        {
            printf("%s Failed json parsing!\n", table[i].json);
            return -1;
        }
        unsigned long j;

        printf("Testing parent tree nodes for json %s ... ", table[i].json);

        for (j = 0; j < tokens[0].size; j++)
        {
            if (tokens[j].parent != table[i].parent[j])
            {
                printf("failed.\n");
                return i;
            }
        }
        printf("passed.\n");
    }

    teardown();
    return 0;
}


static void setup(void)
{}


static void teardown(void)
{}
