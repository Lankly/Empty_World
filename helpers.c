#include <stdlib.h>
#include <curses.h>
#include "helpers.h"

/************************
 * FUNCTION DEFINITIONS *
 ************************/

void *Calloc(size_t items, size_t size)
{
  void *ret = calloc(items, size);
  if (ret == NULL)
  {
    quit("Error: Out of memory!\n");
  }
  return ret;
}

void quit(const char* error_msg){
  endwin();
  printf("%s\n",error_msg);
  exit(1);
}

int int_cmp(void *item1, void *item2){
  if(item1 == item2){
    return 0;
  }
  if(item1 == NULL){
    return -1;
  }
  if(item2 == NULL){
    return 1;
  }

  return (*((int *)item1)) - (*((int *)item2));
}
