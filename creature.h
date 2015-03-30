#include <stdbool.h>

#ifndef CREATURE_H
#define CREATURE_H

#include "items.h"
#include "map.h"

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

//DAMAGE TYPES LIST
#define DMG_UNKNOWN 0
#define DMG_BLUNT   1
#define DMG_FIRE    2
#define DMG_ICE     3
#define DMG_WATER   4
#define DMG_LIGHTNING 5
#define DMG_PSYCHIC 6
#define DMG_MAX     6

//EXTRINSICS,INTRINSICS LIST
#define TRINSIC_UNKNOWN 0
#define TRINSIC_FLYING 1
#define TRINSIC_HOVER  2
#define TRINSIC_RES_FIRE 3
#define TRINSIC_RES_ICE  4
#define TRINSIC_RES_LIGHTNING 5
#define TRINSIC_TELEPATHY 6
#define TRINSIC_BLIND_TELEPATHY 7
#define TRINSIC_TELEKINESIS 8
#define TRINSIC_DARKVISION  9
#define TRINSIC_LASER_VISION  10
#define TRINSIC_TELEPORTATION 11
#define TRINSIC_CONT_TELEPORTATION 12
#define TRINSIC_POLYMORPH 13
#define TRINSIC_CONT_POLYMORPH 14
#define TRINSIC_MAX 14

//CONSUMABLES TYPES LIST
#define CONSUME_UNKNOWN 0
#define CONSUME_ANIMAL  1
#define CONSUME_SAPIENT 2
#define CONSUME_INSECT  3
#define CONSUME_FOOD    4
#define CONSUME_DRUG    5
#define CONSUME_WATER   6
#define CONSUME_ALCOHOL 7
#define CONSUME_POTION  8
#define CONSUME_BLOOD   9
#define CONSUME_NONFOOD 10
#define CONSUME_MAX 10

//BREATHABLE TYPES LIST
#define BREATHE_UNKNOWN 0
#define BREATHE_AIR 1
#define BREATHE_WATER 2
#define BREATHE_POISON 3
#define BREATHE_LAVA 4
#define BREATHE_MAX 4

//This is a list of all the types of things that a creature can breathe
typedef struct breathe_node_t{
  int breathe_type;
  struct breathe_node_t* next;
}breathe_node_t;
typedef struct{
  struct breathe_node_t* first;
}breathe_list_t;

/* This is a list of all the types of things that a creature or the player can
 * eat or drink
 */
typedef struct consume_node_t{
  int consume_type;
  struct consume_node_t* next;
}consume_node_t;
typedef struct{
  consume_node_t* first;
}consume_list_t;

//This is a list of all the intrinsics that a creature has
typedef struct intrinsics_node_t{
  int intrinsic_type;
  struct intrinsics_node_t* next;
}intrinsics_node_t;
typedef struct{
  intrinsics_node_t* first;
}intrinsics_list_t;

//This is a list of all the resistances that a creature has
typedef struct resistances_node_t{
  int damage_type;
  struct resistances_node_t* next;
}resistances_node_t;
typedef struct{
  resistances_node_t* first;
}resistances_list_t;


struct creature_t; struct map_t; struct item_map_t;
typedef void (*creatureTakeTurnCallback)(struct creature_t* creature,
					 struct map_t* map);

#include "inventory.h"
struct creature_t{
  int strength;  int perception;    int endurance;
  int charisma;  int intelligence;  int agility;
  int luck;      int health;        int hunger;
  int gold;      int level;         int max_hunger;

  int corpse_type;
  int creature_id;
  int dlevel;
  int type;
  int x; int y;

  char* name;
  
  bool is_awake;
  bool can_move;
  bool can_fly;
  bool can_see;

  breathe_list_t* breathables;
  consume_list_t* consumables;
  intrinsics_list_t* intrinsics;
  resistances_list_t* resistances;

  inventory_t* inventory;
  creatureTakeTurnCallback takeTurn;
};

#include "creature_callbacks.h"

//This is a list of creatures
typedef struct creature_list_node_t{
  struct creature_t* creature;
  struct creature_list_node_t* next;
}creature_list_node_t;
struct creature_list_t{
  struct creature_list_node_t* first;
};

//This is all the default kinds of creatures that can be created
struct creature_t creature_data[CREATURE_TYPE_MAX+1];
int creature_types_data[CREATURE_TYPE_MAX+1];

void creature_data_init();
void creature_kill(struct creature_t* creature);
void damage_creature(struct creature_t* creature, int dmg);
void creature_place_on_map(struct creature_t* creature, map_t* map);
int creature_see_distance(struct creature_t* creature);
bool creature_is_visible(struct creature_t* creature);
bool creature_can_move_to(struct creature_t* creature, int x, int y, int cmd);
int creature_get_damage(struct creature_t* creature);

void set_level(struct creature_t* c, int l);
void set_dlevel(struct creature_t* c, int d);
void set_name(struct creature_t* c, char* n);
void set_gold(struct creature_t* c, int g);

void set_strength(struct creature_t* c, int s);
void set_perception(struct creature_t* c, int p);
void set_endurance(struct creature_t* c, int e);
void set_charisma(struct creature_t* c, int ch);
void set_intelligence(struct creature_t* c, int i);
void set_agility(struct creature_t* c, int a);
void set_luck(struct creature_t* c, int l);

void set_health(struct creature_t* c, int h);
void set_max_hunger(struct creature_t* c, int m);
void set_hunger(struct creature_t* c, int h);

char* get_name(struct creature_t* c);
int get_level(struct creature_t* c);
int get_dlevel(struct creature_t* c);
int get_weight(struct creature_t* c);
int get_gold(struct creature_t* c);

int get_health(struct creature_t* c);
int get_max_hunger(struct creature_t* c);
int get_hunger(struct creature_t* c);

int get_strength(struct creature_t* c);
int get_perception(struct creature_t* c);
int get_endurance(struct creature_t* c);
int get_charisma(struct creature_t* c);
int get_intelligence(struct creature_t* c);
int get_agility(struct creature_t* c);
int get_luck(struct creature_t* c);

void add_health(struct creature_t* c, int h);

int add_breathable(struct creature_t* creature, int type);
int add_consumable(struct creature_t* creature, int type);
int add_intrinsic(struct creature_t* creature, int type);
int add_resistance(struct creature_t* creature, int type);

int remove_breathable(struct creature_t* creature, int type);
int remove_consumable(struct creature_t* creature, int type);
int remove_intrinsic(struct creature_t* creature, int type);
int remove_resistance(struct creature_t* creature, int type);

#endif
