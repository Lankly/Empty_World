#include <stdbool.h>
#include <curses.h>
#include "bodies.h"

#ifndef ITEMS_H
#define ITEMS_H

typedef struct itemlist_t itemlist_t;
typedef struct item_map_t item_map_t;

#define ITEM_UNKNOWN  0
#define ITEM_UP_STAIR 1
#define ITEM_DOWN_STAIR 2
#define ITEM_IRON_SWORD 3
#define ITEM_MAX 3

//Damage Types
#define DMG_BLUNT 0
#define DMG_SLASHING 1
#define DMG_PIERCING 2
#define DMG_FIRE 3
#define DMG_COLD 4
#define DMG_INFECTION 5
#define DMG_CRUSHING 6
#define DMG_EXPLOSIVE 7
#define DMG_PSYCHIC 8
#define DMG_MAX 8

#define BUC_UNCURSED 0
#define BUC_CURSED 1
#define BUC_BLESSED 2

//These are callbacks that will be inside of each item
struct item_use_t; struct item_consume_t; struct item_zap_t;
typedef void (*itemUseCallback)(struct item_use_t* data);
typedef void (*itemConsumeCallback)(struct item_consume_t* data);
typedef void (*itemZapCallback)(struct item_zap_t* data);

typedef struct item_t{
  int id;
  int str_bonus;int per_bonus;int end_bonus;int cha_bonus;
  int int_bonus;int agl_bonus;int luc_bonus;int hth_bonus;

  int display;
  char *name;
  char *exam_text;
  char *dmg_verb;
  int gold_value;
  int size;//how much space it takes up on an item stack, in a bag
  int material;
  int weight;
  int health;
  int damage;
  int damage_type;
  int weapon_type;
  int wearable;

  bool is_static;
  bool is_two_handed;
  bool stopme;
  bool immovable;
  bool edible;

  int extrinsic;
  int curse_lvl;
  int water_breathing_lvl;//0=normal,1=shallow,2=deep

  struct map_t* go_to_map;

  struct body_part_t *corpse;
  
  itemUseCallback use;
  itemConsumeCallback consume;
  itemZapCallback zap;
}item_t;

#include "creature.h"

//This gives the callbacks information about how to use an item
typedef struct item_use_t{
  int type;
  item_t* caller;
  item_t* item;
  struct creature_t* creature;
}item_use_t;

//This gives the callbacks information about how to consume an item
typedef struct item_consume_t{
  int type;
  struct creature_t* creature;
}item_consume_t;

//This gives the callbacks information about how to zap an item
typedef struct item_zap_t{
  int type;
  struct creature_list_t* creatures;
}item_zap_t;

#include "item_callbacks.h"

//Material Types
#define MAT_UNKNOWN 0
#define MAT_CLOTH   1
#define MAT_GOLD    2
#define MAT_LEATHER 3
#define MAT_WOOD    4
#define MAT_BONE    5
#define MAT_SILVER  6
#define MAT_STONE   7
#define MAT_BRONZE  8
#define MAT_IRON    9
#define MAT_STEEL   10
#define MAT_ADAMANTINE 11
#define MAT_MITHRIL 12

//WEARABLE TYPES LIST
#define WEAR_UNKNOWN 0
#define WEAR_RESIST  1
#define WEAR_HEAD    2
#define WEAR_CHEST   3
#define WEAR_ARM     4
#define WEAR_HAND    5
#define WEAR_LEG     6
#define WEAR_FOOT    7
#define WEAR_FINGER  8
#define WEAR_EYES    9
#define WEAR_NECK    10
#define WEAR_SHOULDER 11
#define WEAR_MAX     11

item_t item_data[ITEM_MAX+1];

//Simple item list
struct itemlist_t{
  item_t *item;
  struct itemlist_t *next;
};

/* This represents a stack of items on a given coordinate on the floor. Next and
 * Prev help cycle through the list of item stacks.
 */
struct item_map_t{
  struct item_map_t* next;
  itemlist_t* first;
  int x; int y;
  int size;//This is the current size of the item stack
};

#include "map.h"

void item_data_init();
void item_map_init(struct item_map_t* items, int w, int h);
void item_map_add(item_map_t *items, item_t *item);

bool add_item(struct map_t* map, int x, int y, struct item_t* i, bool reveal);
item_t* remove_item(item_map_t* map, int x, int y, int index);
int count_items(struct map_t* map,int x,int y);
void destroy_item(item_t* item);

char *get_dmg_verb(item_t *weapon);
char *get_item_name(item_t *item);
int get_item_sym(struct map_t* map,int x,int y, int index);
int get_top_item_sym(struct map_t* map,int x,int y);
int get_top_item_sym_from_stack(struct item_map_t* items);

item_t *item_create_from_data(int index);
int items_display(struct map_t* map,int x,int y);
#endif
