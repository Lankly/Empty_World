#include <stdbool.h>

#ifndef PLAYER_H
#define PLAYER_H

#include "creature.h"
#include "items.h"
struct creature_t* player;

void player_init();
int player_add_breathable(int type);
int player_add_consumable(int type);
int player_add_intrinsic(int type);
int player_add_resistance(int type);

int player_remove_breathable(int type);
int player_remove_consumable(int type);
int player_remove_intrinsic(int type);
int player_remove_resistance(int type);

void damage_player(int dmg);
void heal_player(int amt);

#endif
