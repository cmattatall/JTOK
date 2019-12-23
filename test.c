#include "jsmn.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "jtok.h"

static void tprint(const char *json, jtok_t t)
{
  int i;
  for (i = t.start; i < t.end; i++)
  {
    printf("%c", json[i]);
  }
  printf("\ntype   : %d"
         "\nstart  : %d"
         "\nend    : %d"
         "\nsize   : %d"
         "\nparent : %d\n\n",
         t.type,
         t.start,
         t.end,
         t.size,
         t.parent);
}

int main(void)
{
  const char *jsons[] =
      {
          "{\"GPIO_PIN_CONFIG\":{\"id\":1, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
          "{\"GPIO_PIN_CONFIG\":{\"id\":2, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
          "{\"GPIO_PIN_CONFIG\":{\"id\":3, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
          "{\"GPIO_PIN_CONFIG\":{\"id\":4, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
          "{\"GPIO_PIN_CONFIG\":{\"id\":5, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
          "{\"GPIO_PIN_CONFIG\":{\"id\":6, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
          "{\"GPIO_PIN_CONFIG\":{\"id\":7, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
          "{\"GPIO_PIN_CONFIG\":{\"id\":8, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
      };

  int j = 0;


  jtok_t *tokens;

  jparser_t parser;

  parseRetval_t ret; 
  ret = jtokenize(&parser, jsons[j]);
  
  printf("status = %d, cnt = %d\n", ret.status, ret.cnt);
  if(ret.status == JPARSE_OK)
  {
    int t;
    for (t = 0; t < ret.cnt; t++)
    {
      tprint(jsons[j], tokens[t]);
    }
  }  


  if(tokens != NULL)
  {
    free(tokens);
  }
  return 0;
}

/*
typedef enum 
{
	JSMN_PRIMITIVE = 0,
	JSMN_OBJECT = 1,
	JSMN_ARRAY = 2,
	JSMN_STRING = 3
} jsmntype_t;
*/