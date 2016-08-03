#include "colors.h"
#include "helpers.h"
#include "tiles.h"
#include <curses.h>
#include <stdbool.h>
#include <string.h>

/* Struct */
typedef struct tile_t{
  int id;
  int display;
  int display_color;
  int display_color_alt;
  int hardness;  //0=mush, 1=dirt, 2=wood, 3=stone, 4=unbreakable  
  bool deep;
  bool passable;
  bool stopme;
  bool openable;
  bool transparent;
  char *exam_text;
}tile_t;


tile_t tile_data[TILE_MAX + 1];

//Quick Reminder on Hardness:
//1 - Dirt
//2 - Wood
//3 - Stone
//4 - Hardest at this time (Unbreakable)
void tile_data_init(){
  tile_data[TILE_UNKNOWN] = (tile_t){
    .id = TILE_UNKNOWN,
    .passable = false,
    .display = ' ',
    .exam_text = "You do not know what this is",
    .hardness = 10}; 
  tile_data[TILE_FLOOR] = (tile_t){
    .id = TILE_FLOOR,
    .passable = true,
    .display = term != NULL && strstr(term, "xterm") ? ACS_BULLET : '.',
    .transparent = true,
    .exam_text = "It is a hard stone floor.\n",
    .hardness = 3};
  tile_data[TILE_WALL] = (tile_t){
    .id = TILE_WALL,
    .display = ACS_PLUS,
    .exam_text = "It is a hard stone wall.\n",
    .hardness = 3};
  tile_data[TILE_DOOR_OPEN] = (tile_t){
    .id = TILE_DOOR_OPEN,
    .passable = true,
    .display = '-',
    .display_color = CP_GREEN_BLACK,
    .display_color_alt = CP_GREEN_BLACK,
    .exam_text = "It is an open wooden door.\n",
    .hardness = 2,
    .transparent = true,
    .stopme = true,
    .openable = true};
  tile_data[TILE_DOOR_CLOSE] = (tile_t){
    .id = TILE_DOOR_CLOSE,
    .display = '+',
    .display_color = CP_GREEN_BLACK,
    .display_color_alt = CP_GREEN_BLACK,
    .exam_text = "It is a closed wooden door.\n",
    .hardness = 2,
    .stopme = true,
    .openable = true};
  tile_data[TILE_DOOR_BROKEN] = (tile_t){
    .id = TILE_DOOR_BROKEN,
    .passable = true,
    .display = '|',
    .display_color = CP_GREEN_BLACK,
    .display_color_alt = CP_GREEN_BLACK,
    .exam_text = "It is a broken wooden door.\n",
    .hardness = 2,
    .transparent = true,
    .stopme = true,
    .openable = true};
  tile_data[TILE_CORRIDOR] = (tile_t){
    .id = TILE_CORRIDOR,
    .passable = true,
    .transparent = true,
    .display = ACS_CKBOARD,
    .display_color = CP_DARK_GREY_BLACK,
    .display_color_alt = CP_YELLOW_BLACK,
    .exam_text = 
    "It is a hard stone corridor.\n",
    .hardness = 3,
    .stopme = true};
  tile_data[TILE_SAND] = (tile_t){
    .id = TILE_SAND,
    .passable = true,
    .transparent = true,
    .display = ',',
    .display_color = CP_SAND_1,
    .display_color_alt = CP_YELLOW_BLACK,
    .exam_text = "It is sand.\n",
    .hardness = 1,
    .stopme = true};
  tile_data[TILE_SAND_ALT1] = (tile_t){
    .id = TILE_SAND_ALT1,
    .passable = true,
    .transparent = true,
    .display = '.',
    .display_color = CP_SAND_1,
    .display_color_alt = CP_YELLOW_BLACK,
    .exam_text = "It's just more sand.\n",
    .hardness = 1,
    .stopme = true};
  tile_data[TILE_SAND_ALT2] = (tile_t){
    .id = TILE_SAND_ALT2,
    .passable = true,
    .transparent = true,
    .display = '`',
    .display_color = CP_SAND_1,
    .display_color_alt = CP_YELLOW_BLACK,
    .exam_text = "Sand.\n",
    .hardness = 1,
    .stopme = true};
  tile_data[TILE_SAND_MOVING] = (tile_t){
    .id = TILE_SAND_MOVING,
    .passable = true,
    .transparent = true,
    .display = '~',
    .display_color = CP_SAND_2,
    .display_color_alt = CP_YELLOW_BLACK,
    .exam_text = "It is sand.\n",
    .hardness = 1,
    .stopme = true};
}

/* Returns the display character, paired with its colors. Takes into account
 * whether use_8_colors is turned on.
 */
int tile_get_display(int tile_index){
  if(tile_index > TILE_MAX){
    tile_index = TILE_UNKNOWN;
  }
  
  int color = 0;
  if(!use_8_colors && tile_data[tile_index].display_color != 0){
    color = tile_data[tile_index].display_color;
  }
  else if(use_8_colors && tile_data[tile_index].display_color_alt != 0){
    color = tile_data[tile_index].display_color_alt;
  }

  int disp = tile_data[tile_index].display;
  
  return disp | COLOR_PAIR(color);
}

/* Returns the display character, paired with dark colors. Takes into account
 * whether use_8_colors is turned on.
 */
int tile_get_darkened_display(int tile_index){
  if(tile_index > TILE_MAX){
    tile_index = TILE_UNKNOWN;
  }

  int color = use_8_colors ? CP_BLUE_BLACK : CP_DARK_GREY_BLACK;
  
  int disp = tile_data[tile_index].display;

  return disp | COLOR_PAIR(color);
}

int tile_get_display_color(int tile_index){
  if(tile_index > TILE_MAX){
    tile_index = TILE_UNKNOWN;
  }

  return tile_data[tile_index].display_color;
}
int tile_get_display_color_alt(int tile_index){
  if(tile_index > TILE_MAX){
    tile_index = TILE_UNKNOWN;
  }

  return tile_data[tile_index].display_color_alt;
}
int tile_get_hardness(int tile_index){
  if(tile_index > TILE_MAX){
    tile_index = TILE_UNKNOWN;
  }

  return tile_data[tile_index].hardness;
}
bool tile_is_deep(int tile_index){  if(tile_index > TILE_MAX){
    tile_index = TILE_UNKNOWN;
  }

  return tile_data[tile_index].deep;

}
bool tile_is_passable(int tile_index){
  if(tile_index > TILE_MAX){
    tile_index = TILE_UNKNOWN;
  }

  return tile_data[tile_index].passable;
}
bool tile_is_openable(int tile_index){
  if(tile_index > TILE_MAX){
    tile_index = TILE_UNKNOWN;
  }

  return tile_data[tile_index].openable;
}
bool tile_is_transparent(int tile_index){
  if(tile_index > TILE_MAX){
    tile_index = TILE_UNKNOWN;
  }

  return tile_data[tile_index].transparent;
}
bool tile_stops_quickmove(int tile_index){
  if(tile_index > TILE_MAX){
    tile_index = TILE_UNKNOWN;
  }

  return tile_data[tile_index].stopme;  
}
char *tile_get_exam_text(int tile_index){
  if(tile_index > TILE_MAX){
    tile_index = TILE_UNKNOWN;
  }

  return tile_data[tile_index].exam_text;
}
