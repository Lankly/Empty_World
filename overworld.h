#ifndef OVERWORLD_H
#define OVERWORLD_H

#include <stdbool.h>
#include "creature.h"
#include "items.h"
#include "tiles.h"

/* Definitions */
#define OVERWORLD_TYPE_NONE 0
#define OVERWORLD_TYPE_LARGE 1
#define OVERWORLD_TYPE_WASTES 2

/* Public Variables */
bool on_overworld_wastes;
bool on_overworld_large;

/* Functions */

void overworld_init();

void draw_overworld();
void draw_overworld_coord(int x, int y, int z);

bool overworld_place_creature(creature_t *c, int x, int y, int z);
bool overworld_place_item(item_t *i, int x, int y, int z);

/* Getters and Setters */

bool overworld_get_tile(int *t, int *x, int *y, int *z);

bool overworld_set_tile(int t, int x, int y, int z);
void overworld_set_type(int type);

#endif
