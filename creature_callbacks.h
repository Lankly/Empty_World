#ifndef CREATURE_CALLBACKS_H
#define CREATURE_CALLBACKS_H

#include "map.h"
#include "items.h"
#include "creature.h"

void defaultTakeTurnCallback(struct creature_t* creature,
			     struct map_t* map,
			     struct item_map_t* items);
void defaultPathfindCallback(struct creature_t* creature,
			     struct map_t* map,
			     struct item_map_t* items);
#endif
