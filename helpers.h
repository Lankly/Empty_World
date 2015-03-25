#include <stdbool.h>
#include "map.h"

#ifndef HELPERS_H
#define HELPERS_H

#define TERMINAL_WIDTH  80
#define TERMINAL_HEIGHT 24
#define DEFAULT_ITEMS_STACK_SIZE 10

#define PASS_WEIGHT 50

bool qckmv;
int qckmv_cmd;

void* Calloc(int items, int size);
char* str_lowercase(char* str);
bool str_is_num(char* str);
int get_coord(int x,int y,int width);
void quit(const char* error_msg);
void analyze_cmd(int cmd, int* x, int* y);

typedef struct display_list_node_t{
  char* data;
  struct display_list_node_t* next;
}display_list_node_t;

typedef struct{
  display_list_node_t* first;
  int size;
}display_list_t; 

int display(display_list_t* list,int width,int height);

bool qckmv_continue(map_t*, int x, int y, int qckmv_cmd);

void game_init();
void draw_map(struct map_t* map);

void open_tile();
void close_tile();
void pickup_tile();
void debug();
void analyze_cmd(int cmd, int* x, int* y);

#endif
