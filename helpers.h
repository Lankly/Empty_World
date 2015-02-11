#include <stdbool.h>

#ifndef HELPERS_H
#define HELPERS_H

void* Calloc(int items, int size);

char* str_lowercase(char* str);
bool str_is_num(char* str);

int get_coord(int x,int y,int width);

void quit(const char* error_msg);

typedef struct display_list_node_t{
  char* data;
  struct display_list_node_t* next;
}display_list_node_t;

typedef struct{
  display_list_node_t* first;
  int size;
}display_list_t; 

int display(display_list_t* list,int width,int height);

#endif
