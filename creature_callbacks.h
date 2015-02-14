#include "map.h"
#include "items.h"

#ifndef CREATURE_CALLBACKS_H
#define CREATURE_CALLBACKS_H

typedef void (*creatureTakeTurnCallback)(creature_t* creature,
					 map_t* map,
					 item_map_t* items);
typedef void (*creaturePathfindCallback)(creature_t* creature,
					 map_t* map,
					 item_map_t* items);

void defaultTakeTurnCallback(creature_t* creature,
			     map_t* map,
			     item_map_t* items);
void defaultPathfindCallback(creature_t* creature);

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
#define CREATURE_TYPE_MAX      15

#endif
