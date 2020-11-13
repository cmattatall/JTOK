#include <stdio.h>
#include <string.h>

#include "jtok.h"


static const char *validJTOK[] = {
    "{\"key\" : { }}",
    "{\"key\" : \"value\"}",
    "{\"key\" : \"ThisIsAveryLongStringThatINeedToMakeVeryLong\"}",
    "{\"key\" : 1234 }",
    "{\"key\" : [ 1, 2, 3] }",
    "{}",
    "{[]}",
    "{\"key\" : true}",
    "{\"key\" : false}",
    "{\"key\" : null}",
    "{\"key\" : {[]}}",
    "{\"key\" : { [ ] } }",
    "{\"key\" : {\"childKey\" : 123}}"
    "{\"key\" : {\"childKey\" : \"childStringValue\"}}",
    "{\"key\" : {\"childKey\" : [ \"array\", \"of\", \"strings\", \"as\", \"child\", \"key\", \"values\"]}}",
    "{\"key\" : \"key\"}",
    "{\"key\" : [ true, true, false, false]}",
    "{\"key\" : \"true\"}",
    "{\"key\" : \"false\"}",
    "{\"key\" : \"null\"}",
    "{\"true\" : \"true\"}",
    "{\"false\" : \"false\"}",
    "{\"null\" : \"null\"}",
    "{\"hexValueKey\" : \\uffff}",
    "{\"hexValueKey\" : \\uFFFF}",
    "{\"hexValueKey\" : \\u0000}",
    "{\"hexValueKey\" : \\uabcd}",
    "{\"hexValueKey\" : \\uABCD}",
    "{\"hexValueStringifiedKey\" : \"\\uffff\"}",
    "{\"hexValueStringifiedKey\" : \"\\u0000\"}",
    "{\"hexValueStringifiedKey\" : \"\\uFFFF\"}",
    "{\"hexValueStringifiedKey\" : \"\\uABCD\"}",
    "{\"decimalValueKey\" : 123.456}",
    "{\"decimalValueString\" : \"123.456\"}",
    "{\"exponentValue\": 1.001e-9}",
    "{\"exponentValue\": 1.001e9}",
    "{\"exponentValue\": 1.001e+9}",
    "{\"exponentValue\": 1.001E-9}",
    "{\"exponentValue\": 1.001E9}",
    "{\"exponentValue\": 1.001E+9}",
    "{\"exponentValue\": 1e-9}",
    "{\"exponentValue\": 1e9}",
    "{\"exponentValue\": 1e+9}",
    "{\"exponentValue\": 1E-9}",
    "{\"exponentValue\": 1E9}",
    "{\"exponentValue\": 1E+9}",
    "{\"differentTypesOfWhiteSpace\"\t:\t\"valueString\"}",
    "{\"recursion\" : {\"into\" : {\"child\" : {\"objects\" : {\"key\" : [ \"this\", \"is\", \"an\", \"array\", \"that\", \"is\", \"deeply\", \"nested\"]}}}}}",
    "{\"key\" : {[{\"arrKey\" : \"arrValue\"}]}}",
    "{\"key\" : [1]}",
    "{\"key\" : [\"1\"]}",
    "{\"key1\" : [{\"arrJsonKey1\" : \"arrJsonVal1\"}, {\"arrJsonKey2\" : \"arrJsonVal2\"}]}",
};

#define DEBUG

static jtoktok_t tokens[200];

int main(void){
    printf("\nTesting jtok parser against valid jtoks\n");
    for(unsigned int i = 0; i < sizeof(validJTOK)/sizeof(*validJTOK); i++)
    {   
        jtok_parser_t p = jtok_new_parser(validJTOK[i]);
        printf("%s ... ", validJTOK[i]);
        jtokerr_t status = jtok_parse(&p, tokens, sizeof(tokens)/sizeof(*tokens));
        if(status < 0)
        {   
            printf("failed");
            return 1;
        }
        else
        {
            printf("passed");
        }
        printf("\n");

        #ifdef DEBUG
        for(unsigned int j = 0; j < status; j++)
        {   
            char tmp[250];
            jtok_token_tostr(tmp, sizeof(tmp), p.json, tokens[j]);
            printf("%s\n", tmp);
        }

        #endif /* ifdef DEBUG */


        return 0;

    }
    return 0;
}