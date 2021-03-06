#ifndef CREATURE_CALLBACKS_H
#define CREATURE_CALLBACKS_H

#include "map.h"
#include "items.h"
#include "creature.h"

void defaultTakeTurnCallback(struct creature_t *creature,
			     struct map_t *map);
void defaultPathfindCallback(struct creature_t *creature,
			     struct map_t *map);
void defaultKillCallback(struct creature_t *creature,
			 struct map_t *map);

void ratPathfindCallback(struct creature_t *creature,
			 struct map_t *map);
void animalKillCallback(struct creature_t *a, struct map_t *map);

void spawnerTakeTurnCallback(struct creature_t *creature,
			     struct map_t *map);
#endif
