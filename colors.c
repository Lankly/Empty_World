#include "colors.h"
#include <curses.h>

void color_init(){
  start_color();

  //Create colors
  init_color(COLOR_GREY,745,745,745);

  //Create pairs
  init_pair(CP_YELLOW_BLACK, COLOR_YELLOW, COLOR_BLACK);
  init_pair(CP_GREY_BLACK, COLOR_GREY, COLOR_BLACK);
  init_pair(CP_GREEN_BLACK, COLOR_GREEN, COLOR_BLACK);

}
