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
#define TILE_SAND 7
#define TILE_SAND_ALT1 8
#define TILE_SAND_ALT2 9
#define TILE_SAND_MOVING 10
#define TILE_MAX 10

void tile_data_init();

/***********************
 * GETTERS AND SETTERS *
 ***********************/

int tile_get_display(int tile_index);
int tile_get_darkened_display(int tile_index);
int tile_get_display_color(int tile_index);
int tile_get_display_color_alt(int tile_index);
int tile_get_hardness(int tile_index);
bool tile_is_deep(int tile_index);
bool tile_is_passable(int tile_index);
bool tile_is_openable(int tile_index);
bool tile_is_transparent(int tile_index);
bool tile_stops_quickmove(int tile_index);
char *tile_get_exam_text(int tile_index);

#endif
