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
          "{\"GPIO_PIN_CONFIG\":{\"id\":1, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}\r\n",
          "{\"GPIO_PIN_CONFIG\":{\"id\":2, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
          "{\"GPIO_PIN_CONFIG\":{\"id\":3, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
          "{\"GPIO_PIN_CONFIG\":{\"id\":4, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
          "{\"GPIO_PIN_CONFIG\":{\"id\":5, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
          "{\"GPIO_PIN_CONFIG\":{\"id\":6, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
          "{\"GPIO_PIN_CONFIG\":{\"id\":7, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
          "{\"GPIO_PIN_CONFIG\":{\"id\":8, \"type\":0, \"label\":0, \"active\":1, \"trigger\":1, \"debounce\":10 }}",
      };


  
  int j = 0;


  jparser_t parser;

  parseRetval_t ret; 
  
  ret = jtokenize(&parser, jsons[j]);
  
  printf("status = %d, cnt = %d\n", ret.status, ret.cnt);
  if(ret.status == JPARSE_OK)
  { 
    printf("status was ok :)\n");
    int t;
    for (t = 0; t < ret.cnt; t++)
    { 
      printf("token %d\n", t);
      tprint(jsons[j], parser.tokens[t]);
    }
  }  
  else
  { 
    
    printf("return status was not ok\n");
    /*
    int x;
    for(x = 0; x < parser.pos; x++)
    {
      printf("%c", jsons[j][x]);
    }
    printf("\n");
    */
  }

  return 0;
}

