#include "player.h"
#include "creature.h"
#include <stdlib.h>

void player_init(char* name){
  player = (creature_t*)Calloc(1,sizeof(creature_t));
  set_strength(player, 1);
  set_perception(player, 1);
  set_endurance(player, 1);
  set_charisma(player, 1);
  set_intelligence(player, 1);
  set_luck(player, 1);
  set_health(player, 10);
  set_max_hunger(player, 200);
  set_hunger(player, player->max_hunger);
  set_gold(player, 5);
  set_level(player, 1);
  set_name(player, name);

  player->breathables = (breathe_list_t*)Calloc(1,sizeof(breathe_list_t));
  add_breathable(player, BREATHE_AIR);
  player->consumables = (consume_list_t*)Calloc(1,sizeof(consume_list_t));
  add_consumable(player, CONSUME_ANIMAL);
  add_consumable(player, CONSUME_INSECT);
  add_consumable(player, CONSUME_FOOD);
  add_consumable(player, CONSUME_DRUG);
  add_consumable(player, CONSUME_WATER);
  add_consumable(player, CONSUME_ALCOHOL);
  add_consumable(player, CONSUME_POTION);
  player->intrinsics = (intrinsics_list_t*)Calloc(1,sizeof(intrinsics_list_t));
  player->resistances = (resistances_list_t*)Calloc(1,sizeof(resistances_list_t));
}
