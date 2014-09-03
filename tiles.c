#include "tiles.h"
#include <curses.h>
#include "colors.h"

//Quick Reminder on Hardness:
//1 - Dirt
//2 - Wood
//3 - Stone
//4 - Hardest at this time (Unbreakable)
void tile_data_init(){
  tile_data[TILE_UNKNOWN]= (tile_t) {.id=TILE_UNKNOWN ,.deep=false, .passable=false, .display=' ', .exam_text="How did you get out here?\n", .hardness=10, .stopme=false};
  tile_data[TILE_FLOOR]= (tile_t) {.id=TILE_FLOOR, .deep=false, .passable=true, .display='.', .exam_text="It is a hard stone floor.\n", .hardness=3, .stopme=false};
  tile_data[TILE_WALL]=  (tile_t) {.id=TILE_WALL, .deep=false, .passable=false, .display=ACS_CKBOARD, .exam_text="It is a hard stone wall.\n", .hardness=3, .stopme=false};
  tile_data[TILE_DOOR_OPEN]= (tile_t) {.id=TILE_DOOR_OPEN, .deep=false, .passable=true, .display='-', .exam_text="It is an open wooden door.\n", .hardness=2, .stopme=true, .openable=true};
  tile_data[TILE_DOOR_CLOSE]= (tile_t) {.id=TILE_DOOR_CLOSE, .deep=false, .passable=false, .display='+', .exam_text="It is a closed wooden door.\n", .hardness=2, .stopme=true, .openable=true};
  tile_data[TILE_DOOR_BROKEN]= (tile_t) {.id=TILE_DOOR_BROKEN, .deep=false, .passable=true, .display='|', .exam_text="It is a broken wooden door.\n", .hardness=2, .stopme=true, .openable=true};
  tile_data[TILE_CORRIDOR]= (tile_t) {.id=TILE_CORRIDOR, .deep=false, .passable=true, .display=ACS_CKBOARD | COLOR_PAIR(CP_GREY_BLACK), .exam_text="It is a hard stone corridor.\n", .hardness=3, .stopme=true};
}

int tile_id(tile_t t){
  return t.id;
}
