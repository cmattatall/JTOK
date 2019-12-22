#ifndef JSON_LOOKUP_TABLES
#define JSON_LOOKUP_TABLES

#include "jtokens.h"
/*
//todo: create description of structure in misra format


*/
struct keyEntry
{
  char Str[50];
  jtokType_t validValueTypes;
  struct keyEntry *nextTable; //if we match this keyentry, we go to the next table's keyEntry
  void *container;
};




static char *configKeys[] = {
    "id",
    "type",
    "active",
    "label",
    "trigger",
    "debounce",
};

static char *writeKeys[] = {
    "hbInterval",
    "pin_info_word_terval"};

char *valid_keys[] =
    {
        "GPIO_PIN_CONFIG",
        "id",
        "type",
        "label",
        "active",
        "trigger",
        "debounce",
};

#endif