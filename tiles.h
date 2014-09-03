#include <stdbool.h>

#ifndef TILES_H
#define TILES_H

//Tile values
#define TILE_UNKNOWN 0
#define TILE_FLOOR 1
#define TILE_WALL 2
#define TILE_DOOR_OPEN 3
#define TILE_DOOR_CLOSE 4
#define TILE_DOOR_BROKEN 5
#define TILE_CORRIDOR 6
#define TILE_MAX 6

typedef struct {
  int id;
  bool  deep;
  bool  passable;
  int   display;
  char* exam_text;
  int   hardness;//0=mush, 1=dirt, 2=wood, 3=stone, 4=unbreakable  
  bool  stopme;
  bool  openable;
} tile_t;

tile_t tile_data[TILE_MAX];

int tile_id(tile_t t);

void tile_data_init();

#endif
