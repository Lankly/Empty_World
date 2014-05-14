#ifndef MAP_H
#define MAP_H

//The Map Data Type
typedef struct {
  int width;
  int height;
  int* tiles;
} map_t;

void map_init(map_t* map, int w, int h);

void map_set_tile(map_t* map, int x, int y, int tile);

int map_get_tile(map_t* map, int x, int y);

void map_draw_rect(map_t* map, int x, int y, int w, int h, int tile);

void map_draw_borders(map_t* map);

#endif
