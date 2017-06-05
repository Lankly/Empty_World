#include <curses.h>
#include <stdbool.h>
#include "colors.h"
#include "helpers.h"

/*********************
 * PRIVATE VARIABLES *
 *********************/

bool compatibility_mode = false;


/************************
 * FUNCTION DEFINITIONS *
 ************************/

void colors_init(){
  start_color();

  //Create colors
  //init_color(COLOR_GREY,745,745,745);
  //init_color(COLOR_DARK_GREY,519,519,519);

  //Create pairs
  init_pair(CP_BLACK_GREEN, COLOR_BLACK, COLOR_GREEN);
  init_pair(CP_BLACK_RED, COLOR_BLACK, COLOR_RED);
  init_pair(CP_BLACK_WHITE, COLOR_BLACK, COLOR_WHITE);
  init_pair(CP_BLACK_YELLOW, COLOR_BLACK, COLOR_YELLOW);
  init_pair(CP_BLUE_BLACK, COLOR_BLUE, COLOR_BLACK);
  init_pair(CP_DARK_GREY_BLACK, COLOR_DARK_GREY, COLOR_BLACK);
  init_pair(CP_GREEN_BLACK, COLOR_GREEN, COLOR_BLACK);
  init_pair(CP_GREY_BLACK, COLOR_GREY, COLOR_BLACK);
  init_pair(CP_RED_BLACK, COLOR_RED, COLOR_BLACK);
  init_pair(CP_YELLOW_BLACK, COLOR_YELLOW, COLOR_BLACK);
  init_pair(CP_WHITE_BLACK, COLOR_WHITE, COLOR_BLACK);
}

int color_pair_dark_text(){
  if(compatibility_mode){
    return CP_WHITE_BLACK;
  }
  return CP_GREY_BLACK;
}

/* COMPATIBILITY MODE FUNCTIONS */

bool compatibility_mode_on(){
  return compatibility_mode;
}

void set_compatibility_mode(bool on){
  compatibility_mode = on;
}

void toggle_compatibility_mode(){
  set_compatibility_mode(!compatibility_mode_on());
}
