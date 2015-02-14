#include "status.h"
#include <stdbool>

#ifndef PLAYER_H
#define PLAYER_H

//This is a list of all the types of things that the player can breathe
typedef struct breathe_node_t{
  int breathe_type;
  struct breathe_node_t next;
}breathe_node_t;
typedef struct{
  breathe_node_t first;
}breathe_list_t;

//This is a list of all the types of things that the player can eat or drink
typedef struct consume_node_t{
  int consume_type;
  struct consume_node_t;
}consume_node_t;
typedef struct{
  consume_node_t first;
}consume_list_t;

//This is a list of all the intrinsics that a player has
typedef struct intrinsics_node_t{
  int intrinsic_type;
  struct intrinsics_node_t next;
}intrinsics_node_t;
typedef struct{
  intrinsics_node_t first;
}intrinsics_list_t;

//This is a list of all the resistances that a player has
typedef struct resistances_node_t{
  int damange_type;
  struct resistances_node_t next;
}resistances_node_t;
typedef struct{
  resistances_node_t first;
}resistances_list_t;

//This defines the player
typedef struct{
  int strength;  int perception;    int endurance;
  int charisma;  int intelligence;  int agility;
  int luck;      int health;        int hunger;
  int gold;      int level;

  char* name;
  int x; int y;

  bool can_move;
  bool can_fly;
  breathe_list_t breathables;
  consume_list_t consumables;
  intrinsics_list_t intrinsics;
  resistances_list_t resistances;
}player_t;


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
#define BREATHE_MAX

player_t* player;

void player_init();
int player_add_breathable(int type);
int player_add_consumable(int type);
int player_add_intrinsic(int type);
int player_add_resistance(int type);

int player_remove_breathable(int type);
int player_remove_consumable(int type);
int player_remove_intrinsic(int type);
int player_remove_resistance(int type);

#endif
