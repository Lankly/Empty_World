#include "helpers.h"
#include <curses.h>
#include <stdlib.h>

//Helper Methods

void* Calloc(int items, int size)
{
  void* ret = calloc(items, size);
  if (ret == 0)
  {
    endwin();
    printf("Error: out of memory\n");
    exit(1);
  }
  return ret;
}
