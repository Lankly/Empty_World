#include <stdbool.h>

#ifndef TILES_H
#define TILES_H

//Tile values
typedef enum {
  TILE_UNKNOWN,
  TILE_FLOOR,
  TILE_WALL,
  TILE_DOOR_OPEN,
  TILE_DOOR_CLOSE,
  TILE_DOOR_BROKEN,
  TILE_CORRIDOR,
  TILE_MAX
} tile_t;

typedef struct {
  int id;
  int display;
  int display_color;
  int display_color_alt;
  int hardness;  //Scale of 0 to 100
  bool deep;
  bool passable;
  bool stopme;
  bool openable;
  bool transparent;
  char* exam_text;
} tile_data_t;

tile_data_t tile_data[TILE_MAX + 1];

int tile_id(tile_data_t t);

void tile_data_init();

tile_data_t get_tile(tile_t t);

#endif
