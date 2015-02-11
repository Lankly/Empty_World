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

int display(display_list_t* list,int width,int height){
  //One padded row
  move(0,0);
  display_list_node_t* next =list->first; 
  for(int i=0;i<width;i++){addch(' ');}
  //Now for each element in the list
  for(int i=0;i<height && i<list->size;i++){
    move(i,0);addch(' ');addch('a'+i);addch(' ');
    char* str = next->data;
    for(int j=0;j<width;j++){
      if(j<strlen(str)){addch(str[j]);}
      else{addch(' ');}
    }
    next=next->next;
  }
  return getch()-'a';
}
