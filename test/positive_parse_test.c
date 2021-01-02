/**
 * @file positive_parse_test.c
 * @author Carl Mattatall (cmattatall2@gmail.com)
 * @brief Source module to test various human-conceived jsons that
 * should be parsed as valid jsons
 * @version 0.1
 * @date 2020-12-26
 *
 * @copyright Copyright (c) 2020 Carl Mattatall
 *
 *
 */
#include <stdio.h>
#include <string.h>

#include "jtok.h"

#define TKN_CNT 200

static const char *validJSON[] = {

    "{\"a\":[1,2,3,4,5,6],\"b\":3}",

    "{\"key1\":[],\"key2\":[]}",

    /* empty array */
    "{\"key\" : []}",

    /* primitive */
    "{\"key\" : true}",
    "{\"key\" : false}",
    "{\"key\" : null}",

    /* Strings */
    "{\"key\" : \"value\"}",
    "{\"key\" : \"ThisIsAveryLongStringThatINeedToMakeVeryLong\"}",
    "{\"key\" : \"true\"}",
    "{\"key\" : \"false\"}",
    "{\"key\" : \"null\"}",
    "{\"key\" : \"key\"}", /* duplicate string as value for key */
    "{\"false\" : \"false\"}",
    "{\"null\" : \"null\"}",
    "{\"true\" : \"true\"}",

    /* numbers */
    "{\"decimalValueKey\" : 123.456}",
    "{\"key\" : 1234 }",
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
    "{\"decimalValueString\" : \"-123.456\"}",
    "{\"exponentValue\": -1.001e-9}",
    "{\"exponentValue\": -1.001e9}",
    "{\"exponentValue\": -1.001e+9}",
    "{\"exponentValue\": -1.001E-9}",
    "{\"exponentValue\": -1.001E9}",
    "{\"exponentValue\": -1.001E+9}",
    "{\"exponentValue\": -1e-9}",
    "{\"exponentValue\": -1e9}",
    "{\"exponentValue\": -1e+9}",
    "{\"exponentValue\": -1E-9}",
    "{\"exponentValue\": -1E9}",
    "{\"exponentValue\": -1E+9}",

    /* escaped hex values */
    "{\"hexValueStringifiedKey\" : \"\\uffff\"}",
    "{\"hexValueStringifiedKey\" : \"\\u0000\"}",
    "{\"hexValueStringifiedKey\" : \"\\uFFFF\"}",
    "{\"hexValueStringifiedKey\" : \"\\uABCD\"}",

    /* Child objects */
    "{}",
    "{\"key\" : {}}", /* empty object */
    "{\"key\" : {\"childKey\" : 123}}",
    "{\"key\" : {\"childKey\" : \"childStringValue\"}}",
    "{\"key\" : [ true, true, false, false]}",
    "{\"key\" : { }}",

    /* Arrays */
    "{\"key\" : [ 1, 2, 3] }",
    "{\"key1\" : [{\"arrJsonKey1\" : \"arrJsonVal1\"}, {\"arrJsonKey2\" : "
    "\"arrJsonVal2\"}]}",
    "{\"key\" : [\"1\"]}",
    "{\"key\" : [1]}",
    "{\"key\" : []}", /* empty array */

    /* Mixing object types */
    "{\"recursion\" : {\"into\" : {\"child\" : {\"objects\" : {\"key\" : [ "
    "\"array\", \"that\", \"is\", \"deeply\", \"nested\"]}}}}}",
    "{\"key\" : {\"key\" : [{\"arrKey\" : \"arrValue\"}]}}",
    "{\"key\" : {\"childKey\" : [ \"array\", \"of\", \"strings\", \"as\", "
    "\"child\", \"key\", \"values\"]}}",
    "{\"key1\" : [{\"arrJsonKey1\" : \"arrJsonVal1\"}, {\"arrJsonKey2\" : "
    "\"arrJsonVal2\"}]}",

    /* Misc */
    "{\"differentTypesOfWhiteSpace\"\t:\t\"valueString\"}",
    "{\n\"differentTypesOfWhiteSpace\"\n:\n\"valueString\"\n}",


};

static jtok_tkn_t tokens[TKN_CNT];
int               main(void)
{
    unsigned long long i;
    unsigned long long max_i = sizeof(validJSON) / sizeof(*validJSON);
    printf("\nTesting jtok parser against valid jsons\n");
    for (i = 0; i < max_i; i++)
    {
        printf("\n%s ... ", validJSON[i]);
        JTOK_PARSE_STATUS_t status;
        status = jtok_parse(validJSON[i], tokens, TKN_CNT);
        if (status != JTOK_PARSE_STATUS_OK)
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
    return 0;
}