/**
 * @file empty_array_parse_test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to test parsing of various permutations of empty arrays
 * @version 0.1
 * @date 2020-12-26
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 * @note
 */
#include <stdio.h>

#include "jtok.h"

#define TOKEN_MAX (200u)

static const char *valid_jsons[] = {
    "{\"key\":[]}",       "{\"key\": []}",     "{\"key\":\t[]}",
    "{\"key\":\n[]}",     "{\"key\": [] }",    "{\"key\":\t[] }",
    "{\"key\":\n[] }",    "{\"key\": []\t}",   "{\"key\":\t[]\t}",
    "{\"key\":\n[]\t}",   "{\"key\": []\n}",   "{\"key\":\t[]\n}",
    "{\"key\":\n[]\n}",   "{\"key\": [ ]}",    "{\"key\":\t[ ]}",
    "{\"key\":\n[ ]}",    "{\"key\": [ ] }",   "{\"key\":\t[ ] }",
    "{\"key\":\n[ ] }",   "{\"key\": [ ]\t}",  "{\"key\":\t[ ]\t}",
    "{\"key\":\n[ ]\t}",  "{\"key\": [ ]\n}",  "{\"key\":\t[ ]\n}",
    "{\"key\":\n[ ]\n}",  "{\"key\": [\t]}",   "{\"key\":\t[\t]}",
    "{\"key\":\n[\t]}",   "{\"key\": [\t] }",  "{\"key\":\t[\t] }",
    "{\"key\":\n[\t] }",  "{\"key\": [\t]\t}", "{\"key\":\t[\t]\t}",
    "{\"key\":\n[\t]\t}", "{\"key\": [\t]\n}", "{\"key\":\t[\t]\n}",
    "{\"key\":\n[\t]\n}", "{\"key\": [\n]}",   "{\"key\":\t[\n]}",
    "{\"key\":\n[\n]}",   "{\"key\": [\n] }",  "{\"key\":\t[\n] }",
    "{\"key\":\n[\n] }",  "{\"key\": [\n]\t}", "{\"key\":\t[\n]\t}",
    "{\"key\":\n[\n]\t}", "{\"key\": [\n]\n}", "{\"key\":\t[\n]\n}",
    "{\"key\":\n[\n]\n}",
};

static jtok_parser_t p;
static jtok_tkn_t    tokens[TOKEN_MAX];
int                  main(void)
{
    unsigned int i;
    unsigned int max_i = sizeof(valid_jsons) / sizeof(*valid_jsons);
    for (unsigned int i = 0; i < max_i; i++)
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