#include "helpers.h"
#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Helper Methods

void quit(const char* error_msg){
  endwin();
  printf(error_msg);
  exit(1);
}

int get_coord(int x,int y,int width){
  return y*width+x;
}

char* str_lowercase(char* str){
  char* ret = Calloc(strlen(str)+1,sizeof(char));
  for(int i=0;i<strlen(str);i++){
    ret[i]=tolower(str[i]);
  }
  return ret;
}

bool str_is_num(char* str){
  for(int i=0;i<strlen(str);i++){
    if(!isdigit(str[i])){
      return false;
    }
  }
  return true;
}

void* Calloc(int items, int size)
{
  void* ret = calloc(items, size);
  if (ret == 0)
  {
    quit("Error: out of memory\n");
  }
  return ret;
}
