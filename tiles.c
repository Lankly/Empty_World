#include "tiles.h"
#include <curses.h>

void tile_data_init(){
  tile_data[TILE_FLOOR]= (tile_t) {.deep=false, .passable=true, .display='.', .exam_text="It is a hard stone floor.\n", .hardness=3};
  tile_data[TILE_WALL]=  (tile_t) {.deep=false, .passable=false, .display=ACS_CKBOARD, .exam_text="It is a hard stone wall.\n", .hardness=3};
  tile_data[TILE_DOOR_OPEN]= (tile_t) {.deep=false, .passable=true, .display='-', .exam_text="It is an open wooden door.\n", .hardness=2};
  tile_data[TILE_DOOR_CLOSE]= (tile_t) {.deep=false, .passable=false, .display='+', .exam_text="It is a closed wooden door.\n", .hardness=2};
  tile_data[TILE_DOOR_BROKEN]= (tile_t) {.deep=false, .passable=true, .display='|', .exam_text="It is a broken wooden door.\n", .hardness=2};
}
