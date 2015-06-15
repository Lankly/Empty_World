#include <stdbool.h>
#include "creature.h"
#include "items.h"

#ifndef MAP_H
#define MAP_H

//The Map Data Type
struct creature_list_t;
typedef struct map_t{
  int dlevel;
  int width;
  int height;
  int max_item_height;
  int *tiles;
  struct item_map_t *items;
  struct item_map_t *known_items;
  struct creature_list_t *creatures;
  struct map_t *known_map;
} map_t;

map_t *cur_map;

void map_init(struct map_t *map, int w, int h,int max_item_height, int dlevel);
void map_set_tile(struct map_t *map, int x, int y, int tile);
int map_get_tile(struct map_t *map, int x, int y);

struct creature_t;
void map_add_creature(struct map_t *map, struct creature_t *creature);
void map_remove_creature(struct map_t *map, struct creature_t *creature);

void map_draw_rect(struct map_t *map, int x, int y, int w, int h, int tile);
void map_draw_borders(struct map_t *map);
void map_draw_random_rooms(struct map_t *map, int x, int y);
void map_place_up_stair(struct map_t *map, 
			int x, 
			int y, 
			struct map_t *go_to_map);
void map_place_down_stair_randomly(struct map_t *map);
void map_place_spawners(struct map_t *map);

bool map_coord_is_door(struct map_t*, int x,int y);
bool map_tile_is_door(int tile);
bool map_tile_is_visible(struct map_t *map, int x, int y);
bool map_tile_stopme(struct map_t *map, int x, int y);
void map_examine_tile(struct map_t *map);
void map_reveal(struct map_t *map, int rev_dist);

void map_cleanup(struct map_t *map);
void draw_map(struct map_t *map);

#endif
