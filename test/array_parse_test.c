/**
 * @file array_nesting_parse_test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to test functionality of array parsing
 * @version 0.1
 * @date 2020-12-31
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 * @todo
 */
#include <stdio.h>

#include "jtok.h"

#define TOKEN_MAX (200u)

static const char *valid_jsons[] = {

    "{\"a\":[]}",

    "{\"a\":[[]]}",

    "{\"a\":[[],[]]}",

    "{\"a\":[[],[],[]]}",

    "{\"a\":[[[],[]],[[],[]],[[],[]]]}",

    "{\"a\":[[[1,2,3],[true,false,null]],[[1,2,3],[true,false,null]],[[1,2,3],["
    "true,false,null]]]}",

    "{\"a\":[[[1,2,3],[\"a\",\"b\",\"c\"]],[[1,2,3],[\"a\",\"b\",\"c\"]],[[1,2,"
    "3],[\"a\",\"b\",\"c\"]]]}",

    "{\"a\":[[[1,2,3],[{},{},{}]],[[1,2,3],[{},{},{}]],[[1,2,3],[{},{},{}]]]}",

    "{\"a\":[[[1,2,3],[{\"a\":1,\"b\":2},{\"c\":3,\"d\":4}]],[[1,2,3],[{\"a\":"
    "1,\"b\":2},{\"c\":3,\"d\":4}]],[[1,2,3],[{\"a\":1,\"b\":2},{\"c\":3,\"d\":"
    "4}]]]}",

};

static jtok_parser_t p;
static jtok_tkn_t    tokens[TOKEN_MAX];
int                  main(void)
{
    unsigned long long i;
    unsigned long long max_i = sizeof(valid_jsons) / sizeof(*valid_jsons);
    for (i = 0; i < max_i; i++)
    {
        p = jtok_new_parser(valid_jsons[i]);
        printf("\n%s ... ", valid_jsons[i]);
        JTOK_PARSE_STATUS_t status = jtok_parse(&p, tokens, TOKEN_MAX);
        if (status != JTOK_PARSE_STATUS_PARSE_OK)
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
}