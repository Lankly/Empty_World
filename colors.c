#include "colors.h"
#include "helpers.h"
#include <curses.h>

void color_init(){
  start_color();

  //Create colors
  //init_color(COLOR_GREY,745,745,745);
  //init_color(COLOR_DARK_GREY,519,519,519);
  init_color(COLOR_ORANGE, 945, 812, 230);
  init_color(COLOR_ORANGE2, 945, 683, 230);
  init_color(COLOR_DARK_ORANGE, 500, 425, 121);
  
  //Create pairs
  init_pair(CP_YELLOW_BLACK, COLOR_YELLOW, COLOR_BLACK);
  init_pair(CP_GREY_BLACK, COLOR_GREY, COLOR_BLACK);
  init_pair(CP_GREEN_BLACK, COLOR_GREEN, COLOR_BLACK);
  init_pair(CP_BLACK_GREEN, COLOR_BLACK, COLOR_GREEN);
  init_pair(CP_BLACK_RED, COLOR_BLACK, COLOR_RED);
  init_pair(CP_BLACK_WHITE, COLOR_BLACK, COLOR_WHITE);
  init_pair(CP_BLACK_YELLOW, COLOR_BLACK, COLOR_YELLOW);
  init_pair(CP_DARK_GREY_BLACK, COLOR_DARK_GREY, COLOR_BLACK);
  init_pair(CP_BLUE_BLACK, COLOR_BLUE, COLOR_BLACK);
  init_pair(CP_SAND_1, COLOR_DARK_ORANGE, COLOR_ORANGE2);
  init_pair(CP_SAND_2, COLOR_DARK_ORANGE, COLOR_ORANGE);
}

