/**
 * @file object_sibling_test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to test the sibling relationship of parsed objects
 * @version 0.1
 * @date 2020-12-26
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 *
 */

#include <stdio.h>

#include "jtok.h"

#define JSON_STRLEN (250u)
#define TOKEN_MAX (200u)
#define SIBLING_TREE_SIZE (50u)


static const struct
{
    char json[JSON_STRLEN];
    int  sibling_tree[SIBLING_TREE_SIZE];
} table[] = {

    {.json         = "{\"a\":[1,2,3,4,5,6],\"b\":3}",
     .sibling_tree = {JTOK_NO_SIBLING_IDX, 9, JTOK_NO_SIBLING_IDX}},

    {.json         = "{\"a\":1}",
     .sibling_tree = {JTOK_NO_SIBLING_IDX, JTOK_NO_SIBLING_IDX}},

    {.json         = "{\"a\":1,\"b\":2}}",
     .sibling_tree = {JTOK_NO_SIBLING_IDX, 3, JTOK_NO_SIBLING_IDX}},

    {.json         = "{\"a\":1,\"b\":2,\"c\":[]}}",
     .sibling_tree = {JTOK_NO_SIBLING_IDX, 3, 5, JTOK_NO_SIBLING_IDX}},

    {.json         = "{\"a\":1,\"b\":2,\"c\":[], \"d\":{}}}",
     .sibling_tree = {JTOK_NO_SIBLING_IDX, 3, 5, 7, JTOK_NO_SIBLING_IDX}},

    {.json         = "{\"a\":{\"a1\":1,\"a2\":2},\"b\":3}",
     .sibling_tree = {JTOK_NO_SIBLING_IDX, 7, JTOK_NO_SIBLING_IDX}},


};


static jtok_tkn_t tokens[TOKEN_MAX];

int main(void)
{

    unsigned long long i;
    unsigned long long max_i = sizeof(table) / sizeof(*table);
    for (i = 0; i < max_i; i++)
    {
        printf("\nChecking sibling tree status of %s ... ", table[i].json);
        JTOK_PARSE_STATUS_t status;
        status = jtok_parse(table[i].json, tokens, TOKEN_MAX);
        if (status != JTOK_PARSE_STATUS_OK)
        {
            printf("parse failed with status %d.\n", status);
            return 1;
        }
        else
        {
            long        s;
            jtok_tkn_t *current_tkn = tokens;
            for (s = 0; s < SIBLING_TREE_SIZE && s < tokens[0].size; s++)
            {
                if (s == 0)
                {
                    /* Top level json object CANNOT have siblings */
                    if (current_tkn->sibling != JTOK_NO_SIBLING_IDX)
                    {
                        printf("Failed.\n");
                        return -1;
                    }

                    if (current_tkn->size > 0)
                    {
                        current_tkn = &tokens[1];
                    }
                }
                else
                {
                    if (current_tkn->sibling != table[i].sibling_tree[s])
                    {
                        printf("Failed.\n");
                        return -1;
                    }
                    else
                    {
                        /* Walk to next sibling */
                        current_tkn = &tokens[current_tkn->sibling];

                        if (current_tkn->sibling == JTOK_NO_SIBLING_IDX)
                        {
                            break;
                        }
                    }
                }
            }
            printf("Passed.\n");
        }
    }
    return 0;
}