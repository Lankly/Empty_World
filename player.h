#include <stdbool.h>

#ifndef PLAYER_H
#define PLAYER_H

#define PLAYER_NAME_SIZE 12

#include "creature.h"
#include "items.h"

struct creature_t* player;
int cmd;

/*void playerTakeTurnCallback(struct creature_t* creature,
  struct map_t* map);*/

void player_init();

int get_cur_weapon_skill();

#endif
