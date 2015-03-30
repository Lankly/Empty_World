#include <stdbool.h>

#ifndef PLAYER_H
#define PLAYER_H

#include "creature.h"
#include "items.h"

struct creature_t* player;
int cmd;

void playerTakeTurnCallback(struct creature_t* creature,
			    struct map_t* map);

void player_init();
#endif
