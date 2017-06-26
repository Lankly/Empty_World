#include <stdlib.h>
#include <curses.h>
#include <limits.h>
#include <math.h>
#include "helpers.h"
#include "colors.h"
#include "creatures.h"
#include "display.h"
#include "items.h"
#include "maps.h"

/************************
 * FUNCTION DEFINITIONS *
 ************************/

void *Calloc(size_t items, size_t size){
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

void game_init(){
  display_init();        /* Has to come first or colors won't work. */
  colors_init();
  creatures_init();
  items_init();
  tile_data_init();
  //maps_init();

  //ncurses
  cbreak();                    /* Disable line buffering */
  curs_set(0);                 /* Make cursor invisible */
  noecho();                    /* getch() will not print characters */
  keypad(stdscr, true);        /* getch() will get token correctly */
  timeout(500);                /* getch timeout of half second */
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

  int first = *(int *)item1;
  int second = *(int *)item2;
  
  return first - second;
}

dir_t rand_dir(){
  return rand() % DIR_COUNT;
}

int get_coord_in_arr(int x, int y, int width){
  if(x < 0 || y < 0 || x >= width){
    return -1;
  }

  return (width * y) + x;
}

double dist(int x1, int y1, int x2, int y2){
  return sqrt(x1*x2 + y1*y2);
}

unsigned int hash(unsigned int n) {
    n = ((n >> 16) ^ n) * 0x45d9f3b;
    n = ((n >> 16) ^ n) * 0x45d9f3b;
    n = (n >> 16) ^ n;
    return n;
}
