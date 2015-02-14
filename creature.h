#include "creature_callbacks.h"
#include <stdbool.h>

#ifndef CREATURE_H
#define CREATURE_H

typedef struct creature_inventory_node_t{
  item_t* item;
  struct creature_inventory_node_t* next;
}creature_inventory_node_t;
typedef struct{
  creature_inventory_node_t* first;
}creature_inventory_t*;

typedef struct{
int creature_id;
int type;
char* name;
creature_inventory_t* inventory;
int health;
int x; int y;

creatureTakeTurnCallback* takeTurn;
creaturePathfindCallback* move;

bool is_awake;
}creature_t;

creature_t creature_data[CREATURE_TYPE_MAX+1];
int creature_types_data[CREATURE_TYPE_MAX+1];

void creature_data_init();
void damage_creature(creature_t* creature, int damage);
void creature_place_on_map(creature_t* creature, map_t* map);
bool creature_is_visible(creature_t* creature);

//DEFINE CREATURE TYPES
#define CREATURE_TYPE_UNKNOWN  0
#define CREATURE_TYPE_HUMAN    1
#define CREATURE_TYPE_CANINE   2
#define CREATURE_TYPE_FELINE   3
#define CREATURE_TYPE_RODENT   4
#define CREATURE_TYPE_AVIAN    5
#define CREATURE_TYPE_INSECT   6
#define CREATURE_TYPE_GOBLIN   7
#define CREATURE_TYPE_ORC      8
#define CREATURE_TYPE_MINDFLAYER 9
#define CREATURE_TYPE_DEMON    10
#define CREATURE_TYPE_SKELTON  11
#define CREATURE_TYPE_SENTINEL 12
#define CREATURE_TYPE_GRIFFON  13
#define CREATURE_TYPE_FOG      14
#define CREATURE_TYPE_PLANT    15
#define CREATURE_TYPE_EQUINE   16
#define CREATURE_GOLEM         18
#define CREATURE_EYE           19
#define CREATURE_TYPE_MAX      19

//DEFINE CREATURE IDS
#define CREATURE_UNKNOWN       0
#define CREATURE_HUMAN_WARRIOR 1
#define CREATURE_HUMAN_BOWMAN  2
#define CREATURE_HUMAN_PIKEMAN 3
#define CREATURE_HUMAN_MONK    4
#define CREATURE_HUMAN_WIZARD  5
#define CREATURE_WERECREATURE 6
#define CREATURE_GOBLIN_WARRIOR 7
#define CREATURE_GOBLIN_BOWMAN  8
#define CREATURE_GOBLIN_PIKEMAN 9
#define CREATURE_GOBLIN_WIZARD  10
#define CREATURE_ORC_WARRIOR 11
#define CREATURE_ORC_BOWMAN  12
#define CREATURE_ORC_PIKEMAN 13
#define CREATURE_ORC_WIZARD  14
#define CREATURE_MINDFLAYER  15
#define CREATURE_SKELETON_WARRIOR 16
#define CREATURE_SKELETON_BOWMAN  17
#define CREATURE_SKELETON_PIKEMAN 18
#define CREATURE_PUPPY  19
#define CREATURE_DOG    20
#define CREATURE_WARDOG 21
#define CREATURE_KITTEN   22
#define CREATURE_HOUSECAT 23
#define CREATURE_WARCAT   24
#define CREATURE_LION 25
#define CREATURE_PUMA 26
#define CREATURE_WOLF 27
#define CREATURE_HORSE 28
#define CREATURE_UNICORN 29
#define CREATURE_UNICORN_GOOD 30
#define CREATURE_UNICORN_EVIL 31
#define CREATURE_PAPER_GOLEM  32
#define CREATURE_ROCK_GOLEM   33
#define CREATURE_SILVER_GOLEM 34
#define CREATURE_GOLD_GOLEM   35
#define CREATURE_IRON_GOLEM   36
#define CREATURE_FLOATING_EYE 37
#define CREATURE_MAX          37

#endif
