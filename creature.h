#include <stdbool.h>

#ifndef CREATURE_H
#define CREATURE_H

#include "items.h"
#include "map.h"

//DEFINE CREATURE TYPES
#define CREATURE_TYPE_UNKNOWN  0
#define CREATURE_TYPE_SPAWNER  1
#define CREATURE_TYPE_HUMAN    2
#define CREATURE_TYPE_CANINE   3
#define CREATURE_TYPE_FELINE   4
#define CREATURE_TYPE_RODENT   5
#define CREATURE_TYPE_AVIAN    6
#define CREATURE_TYPE_INSECT   7
#define CREATURE_TYPE_GOBLIN   8
#define CREATURE_TYPE_ORC      9
#define CREATURE_TYPE_MINDFLAYER 10
#define CREATURE_TYPE_DEMON    11
#define CREATURE_TYPE_SKELETON 12
#define CREATURE_TYPE_SENTINEL 13
#define CREATURE_TYPE_GRIFFON  14
#define CREATURE_TYPE_FOG      15
#define CREATURE_TYPE_PLANT    16
#define CREATURE_TYPE_EQUINE   17
#define CREATURE_TYPE_GOLEM    18
#define CREATURE_TYPE_EYE      19
#define CREATURE_TYPE_MAX      19

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
  struct breathe_node_t *next;
}breathe_node_t;
typedef struct{
  struct breathe_node_t *first;
}breathe_list_t;

/* This is a list of all the types of things that a creature or the player can
 * eat or drink
 */
typedef struct consume_node_t{
  int consume_type;
  struct consume_node_t* next;
}consume_node_t;
typedef struct{
  consume_node_t *first;
}consume_list_t;

//This is a list of all the intrinsics that a creature has
typedef struct intrinsics_node_t{
  int intrinsic_type;
  struct intrinsics_node_t *next;
}intrinsics_node_t;
typedef struct{
  intrinsics_node_t *first;
}intrinsics_list_t;

//This is a list of all the resistances that a creature has
typedef struct resistances_node_t{
  int damage_type;
  struct resistances_node_t *next;
}resistances_node_t;
typedef struct{
  resistances_node_t *first;
}resistances_list_t;


struct creature_t; struct map_t; struct item_map_t;
typedef void (*creatureTakeTurnCallback)(struct creature_t *creature,
					 struct map_t *map);
typedef void (*creaturePathfindCallback)(struct creature_t *creature,
					 struct map_t *map);
typedef void (*creatureKillCallback)(struct creature_t *creature,
				     struct map_t *map);

#include "inventory.h"
struct creature_t{
  int strength;  int perception;    int endurance;
  int charisma;  int intelligence;  int agility;
  int luck;      int health;        int hunger;
  int gold;      int level;         int max_hunger;
  int max_health;
  
  /* This is how hunger works:
   * You have a max_hunger. This represents how much food can be in your stomach
   * at one time. Above this, and the stomach will purge to 20% of its max. You 
   * also have a hunger integer that represents how much food is in your stomach
   * right now. Hunger is a large number, in the hundreds, and decrements once
   * every turn cycle.
   */

  int corpse_type;
  int were_type;
  int class;
  int creature_id;
  int display;
  int dlevel;
  int type;
  int x; int y;
  int last_position; //As in keys on a numpad

  char *name;
  char *exam_text;

  bool legendary;
  bool is_asleep;
  bool is_immobile;
  bool can_fly;
  bool is_blind;
  bool is_telepathic;

  breathe_list_t *breathables;
  consume_list_t *consumables;
  intrinsics_list_t *intrinsics;
  resistances_list_t *resistances;

  inventory_t *inventory;
  creatureTakeTurnCallback takeTurn;
  creaturePathfindCallback pathfind;
  creatureKillCallback kill;
};

#include "creature_callbacks.h"

//This is a list of creatures
typedef struct creature_list_node_t{
  struct creature_t *creature;
  struct creature_list_node_t *next;
}creature_list_node_t;
struct creature_list_t{
  struct creature_list_node_t *first;
};

//This is all the default kinds of creatures that can be created
struct creature_t creature_data[CREATURE_TYPE_MAX+1];

void creature_data_init();
struct creature_t *creature_create_from_data(int index);
struct creature_t *creature_spawn(int creature_id, struct map_t *map);
void damage_creature(struct creature_t *target, struct creature_t *source);
void creature_place_on_map(struct creature_t *creature, map_t *map);
int creature_see_distance(struct creature_t *creature);
bool creature_is_visible(struct creature_t *target, struct creature_t *seer);
bool creature_can_move_to(struct creature_t *creature, int x, int y, int cmd);
int creature_get_damage(struct creature_t *creature);

void set_blindness(struct creature_t *c, bool b);
void set_unconscious(struct creature_t *c, bool b);
void set_immobile(struct creature_t *c, bool b);
void set_telepathic(struct creature_t *c, bool b);
void set_level(struct creature_t *c, int l);
void set_dlevel(struct creature_t *c, int d);
void set_name(struct creature_t *c, char *n);
void set_exam_text(struct creature_t *c, char *e);
void set_class(struct creature_t *c, int class);
void set_gold(struct creature_t *c, int g);

void set_strength(struct creature_t *c, int s);
void set_perception(struct creature_t *c, int p);
void set_endurance(struct creature_t *c, int e);
void set_charisma(struct creature_t *c, int ch);
void set_intelligence(struct creature_t *c, int i);
void set_agility(struct creature_t *c, int a);
void set_luck(struct creature_t *c, int l);

void set_health(struct creature_t *c, int h);
void set_max_health(struct creature_t *c, int mh);
void set_max_hunger(struct creature_t *c, int m);
void set_hunger(struct creature_t *c, int h);

char *get_name(struct creature_t *c);
int get_class(struct creature_t *c);
int get_level(struct creature_t *c);
int get_dlevel(struct creature_t *c);
int get_weight(struct creature_t *c);
int get_gold(struct creature_t *c);

int get_health(struct creature_t *c);
int get_max_health(struct creature_t *c);
int get_hunger(struct creature_t *c);
int get_max_hunger(struct creature_t *c);

bool is_blind(struct creature_t *c);
bool is_unconscious(struct creature_t *c);
bool is_immobile(struct creature_t *c);
bool is_telepathic(struct creature_t *c);

int get_strength(struct creature_t *c);
int get_perception(struct creature_t *c);
int get_endurance(struct creature_t *c);
int get_charisma(struct creature_t *c);
int get_intelligence(struct creature_t *c);
int get_agility(struct creature_t *c);
int get_luck(struct creature_t *c);

void add_health(struct creature_t *c, int h);

int add_breathable(struct creature_t *creature, int type);
int add_consumable(struct creature_t *creature, int type);
int add_intrinsic(struct creature_t *creature, int type);
int add_resistance(struct creature_t *creature, int type);

int remove_breathable(struct creature_t *creature, int type);
int remove_consumable(struct creature_t *creature, int type);
int remove_intrinsic(struct creature_t *creature, int type);
int remove_resistance(struct creature_t *creature, int type);

#endif
