#include "colors.h"
#include "helpers.h"
#include <curses.h>

void color_init(){
  start_color();

  //Create colors
  //init_color(COLOR_GREY,745,745,745);
  //init_color(COLOR_DARK_GREY,519,519,519);

  //Create pairs
  init_pair(CP_YELLOW_BLACK, COLOR_YELLOW, COLOR_BLACK);
  init_pair(CP_GREY_BLACK, COLOR_GREY, COLOR_BLACK);
  init_pair(CP_GREEN_BLACK, COLOR_GREEN, COLOR_BLACK);
  init_pair(CP_BLACK_WHITE, COLOR_BLACK, COLOR_WHITE);
  init_pair(CP_BLACK_YELLOW, COLOR_BLACK, COLOR_YELLOW);
  init_pair(CP_BLACK_RED, COLOR_BLACK, COLOR_RED);
  init_pair(CP_DARK_GREY_BLACK, COLOR_DARK_GREY, COLOR_BLACK);
  init_pair(CP_BLUE_BLACK, COLOR_BLUE, COLOR_BLACK);
}
