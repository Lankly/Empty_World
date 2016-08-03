#ifndef MAP_H
#define MAP_H

//The Map Data Type
typedef struct map_t map_t;

#include <stdbool.h>
#include "creature.h"
#include "items.h"

/* Public Variables*/
map_t *cur_map;
int cur_map_index;


/* Functions*/

void map_init();

map_t *map_new(int w, int h,int max_item_height, int dlevel);
void map_sep(map_t *map);
typedef void (*map_func)(map_t *map);
void apply_to_all_maps(map_func f);

clist_t *map_get_creatures(map_t *map);
int map_get_dlevel(map_t *map);
item_map_t *map_get_items(map_t *map);
int map_get_max_item_height(map_t *map);
int map_get_item_symbol(map_t *map, int x, int y, int index);
int map_get_tile(map_t *map, int x, int y);
int map_get_height(map_t *map);
int map_get_width(map_t *map);

void map_set_tile(map_t *map, int x, int y, int tile);

int get_cur_dlevel();

void map_add_creature(map_t *map, creature_t *creature);
creature_t *map_remove_creature(map_t *map, creature_t *creature);

bool map_add_item(map_t *map, int x, int y, item_t *item, bool reveal);
int map_get_top_symbol(map_t *map, int x, int y);

void map_draw_rect(map_t *map, int x, int y, int w, int h, int tile);
void map_draw_borders(map_t *map);
void map_draw_random_rooms(map_t *map, int x, int y);
bool pickup_tile();
void map_place_up_stair(map_t *map, 
			int x, 
			int y, 
			map_t *go_to_map);
void map_place_down_stair_randomly(map_t *map);
void map_place_spawners(map_t *map);

bool map_coord_is_door(struct map_t*, int x,int y);
bool map_tile_is_door(int tile);
bool map_tile_is_visible(map_t *map, int x, int y, creature_t *c);
bool map_tile_stopme(map_t *map, int x, int y);
void map_examine_tile(map_t *map);
void map_reveal(map_t *map, int rev_dist);

void map_cleanup(map_t *map);
void draw_map(map_t *map);

void debug_reveal_map(map_t *map);

#endif
