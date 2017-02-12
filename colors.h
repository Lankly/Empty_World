#ifndef COLORS_H
#define COLORS_H

//Color Pairs Enumeration
typedef enum color_pair_t {
  CP_BLACK_GREEN,
  CP_BLACK_RED,
  CP_BLACK_WHITE,
  CP_BLACK_YELLOW,
  CP_BLUE_BLACK,
  CP_DARK_GREY_BLACK,
  CP_GREEN_BLACK,
  CP_GREY_BLACK,
  CP_RED_BLACK,
  CP_YELLOW_BLACK
} color_pair_t;

#define COLOR_GREY 7
#define COLOR_DARK_GREY 8

#define CP_CORPSE CP_DARK_GREY_BLACK

//Initialize colors
void color_init();

#endif
