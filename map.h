#include <stdbool.h>
#include "items.h"

#ifndef MAP_H
#define MAP_H

//The Map Data Type
typedef struct {
  int width;
  int height;
  int max_item_height;
  int* tiles;
  item_map_t* items;
} map_t;

void map_init(map_t* map, int w, int h,int max_item_height);

void map_set_tile(map_t* map, int x, int y, int tile);

int map_get_tile(map_t* map, int x, int y);

void map_draw_rect(map_t* map, int x, int y, int w, int h, int tile);

void map_draw_borders(map_t* map);

void map_draw_random_rooms(map_t* map);

bool map_coord_is_door(map_t*, int x,int y);

bool map_tile_is_door(int tile);

void map_cleanup(map_t* map);

#endif
