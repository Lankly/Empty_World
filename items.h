#include <stdbool.h>
#include <curses.h>

#ifndef ITEMS_H
#define ITEMS_H

#define ITEM_UNKNOWN  0
#define ITEM_UP_STAIR 1
#define ITEM_DOWN_STAIR 2
#define CORPSE_HUMAN  3
#define CORPSE_WERE   4
#define CORPSE_ORC    5
#define CORPSE_GOBLIN 6
#define CORPSE_SKELETON 7
#define CORPSE_CANINE 8
#define CORPSE_FELINE 9
#define CORPSE_INSECT 10
#define CORPSE_RODENT 11
#define CORPSE_AVIAN  12
#define CORPSE_DEMON  13
#define CORPSE_MINDFLAYER 14
#define CORPSE_GRIFFON 15
#define CORPSE_SENTINEL 16
#define CORPSE_PLANT  17
#define CORPSE_EYE    18
#define ITEM_IRON_SWORD 19
#define ITEM_MAX 19

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
  char* exam_text;
  int gold_value;
  int size;//how much space it takes up on an item stack, in a bag
  int material;
  int weight;
  int damage;
  bool legendary;//if true, only one may exist  
  bool stopme;
  int wearable;
  bool ranged;
  bool is_static;
  bool is_two_handed;
  int extrinsic;
  int curse_lvl;//0=uncursed,1=blessed,2=cursed
  int water_breathing_lvl;//0=normal,1=shallow,2=deep

  struct map_t* go_to_map;

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

//This givest he callbacks information about how to zap an item
typedef struct item_zap_t{
  int type;
  struct creature_list_t* creatures;
}item_zap_t;

#include "item_callbacks.h"

//Material Types
#define MAT_UNKNOWN 0
#define MAT_CLOTH   1
#define MAT_LEATHER 2
#define MAT_WOOD    3
#define MAT_SILVER  4
#define MAT_GOLD    5
#define MAT_STONE   6
#define MAT_IRON    7

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

//The Item Map Data Type
typedef struct item_map_node_t{
  item_t* item;
  struct item_map_node_t* next;
}item_map_node_t;

/* This represents a stack of items on a given coordinate on the floor. Next and
 * Prev help cycle through the list of item stacks.
 */
typedef struct item_map_t{
  struct item_map_t* next;
  struct item_map_t* prev;
  item_map_node_t* first;
  int x; int y;
  int size;//This is the current size of the item stack
}item_map_t;

#include "map.h"

void item_data_init();
void item_map_init(struct item_map_t* items, int w, int h);

bool add_item(struct map_t* map, int x, int y, struct item_t* i);
item_t* remove_item(struct map_t* map, int x, int y, int index);
int count_items(struct map_t* map,int x,int y);
void destroy_item(item_t* item);

int get_item_sym(struct map_t* map,int x,int y, int index);
int get_top_item_sym(struct map_t* map,int x,int y);
int get_top_item_sym_from_stack(struct item_map_t* items);

item_t* item_create_from_data(int index);
int items_display(struct map_t* map,int x,int y);
#endif
