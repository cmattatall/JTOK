

#include <stdio.h>

#include "jtok.h"

static jtok_parser_t parser1;
static jtok_tkn_t    tokens1[200];
static jtok_parser_t parser2;
static jtok_tkn_t    tokens2[200];

int main(int argc, char **argv)
{
    parser1 = jtok_new_parser("{\"key1\":\"value1\", \"key2\":\"value2\"}");

    JTOK_PARSE_STATUS_t status = jtok_parse(&parser1, tokens1, 200);
    if (status != JTOK_PARSE_STATUS_PARSE_OK)
    {
        return -1;
    }

    int i;
    for (i = 0; i < 200; i++)
    {
        printf("token[%d] : ", i);
        for (int j = tokens1[i].start; j < tokens1[i].end; j++)
        {
            printf("%c", tokens1[i].json[j]);
        }
        printf("\n");

        if (tokens1[i].type == JTOK_UNASSIGNED_TOKEN)
        {
            break;
        }
    }

    return 0;
}