#include <stdio.h>
#include <string.h>

#include "jtok.h"

static const char *invalidJSON[] = {
    "{\"key\"}", /* no value */
    "{key : 123 }", /* unquoted key */
    "key : 123", /* no braces */
    "[ ]", /* no json */
    "\"key\"", /* quoted string without braces */
    "123",
    "{[\"123\", \"456\", \"abc\"]}",
    "{\"key\" : [\"123\" \"456\" \"abc\"]}", /* commas are missing from array */
    "{\"keyMissingSecondQuote : \"stringValue\"}",
    "{keyMissingFirstQuote\" : \"stringValue\"}",
    "{\"key\" : valueStringMissingFirstQuote\"}",
    "{\"key\" : \"valueStringMissingSecondQuote}",
    "{\"key\" : \\uxxxx}", /* invalid hex string */
    "{\"key\" : \"\\uxxxx\"}" /* invalid quoted hex string */
    "{\"key\" : {}", /* mismatched braces */
    "{\"key\" : }}", /* mismatched braces */
    "{ \"key\": [}", /* mismatched square brackets */
    "{\"key\":]}",   /* mismatched square brackets */
    "{123}", /* value without key */
    "{{\"childKey\" : \"childValue\"}}", /* child json with no key */
    "{[]}", /* no key for empty array */
    "{\"key\" : {[]}}", /* array with no key inside child object */
};

static jtoktok_t tokens[200];
int main(void){
    printf("\nTesting jtok parser against invalid jsons\n");
    for(unsigned int i = 0; i < sizeof(invalidJSON)/sizeof(*invalidJSON); i++)
    {   
        jtok_parser_t p = jtok_new_parser(invalidJSON[i]);
        printf("%s ... ", invalidJSON[i]);
        jtokerr_t status = jtok_parse(&p, tokens, sizeof(tokens)/sizeof(*tokens));
        if(status >= 0)
        {   
            printf("failed with status %d", status);
            return 1;
        }
        else
        {
            printf("passed");
        }
        printf("\n");
    }
    return 0;
}