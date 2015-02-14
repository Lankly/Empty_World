#include <stdbool.h>
#include "status.h"
#include "item_callbacks.h"
#include <curses.h>

#ifndef ITEMS_H
#define ITEMS_H

#define ITEM_UNKNOWN 0
#define ITEM_IRON_SWORD 1
#define ITEM_MAX 1

typedef struct{
  int id;
  int str_bonus;int per_bonus;int end_bonus;int cha_bonus;
  int int_bonus;int agl_bonus;int luc_bonus;int hth_bonus;

  int display;
  char* exam_text;
  int gold_value;
  int size;//how much space it takes up on an item stack, in a bag
  int material;
  int weight;
  bool legendary;//if true, only one may exist
  
  int wearable;
  bool is_weapon;
  bool is_two_handed;
  int extrinsic;

  int curse_lvl;//0=uncursed,1=blessed,2=cursed
  int water_breathing_lvl;//0=normal,1=shallow,2=deep
  int turn_to_mob;//0=default, refer to mob catalogue for more

  itemUseCallback use;
  itemConsumeCallback consume;
  itemZapCallback zap;
}item_t;

//Material Types
#define MAT_UNKNOWN 0
#define MAT_CLOTH   1
#define MAT_LEATHER 2
#define MAT_WOOD    3
#define MAT_SILVER  4
#define MAT_GOLD    5
#define MAT_IRON    6

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

typedef struct item_map_t{
  struct item_map_t* next;
  struct item_map_t* prev;
  item_map_node_t* first;
  int x; int y;
  int size;//This is the current size of the item stack
}item_map_t;

#include "map.h"

void item_data_init();
void items_map_init(item_map_t* items, int w, int h);

bool add_item(map_t* map, int x, int y, item_t* i);
item_t* remove_item(map_t* map, int x, int y, int index);
int count_items(map_t* map,int x,int y);
char get_item_sym(map_t* map,int x,int y, int index);
char get_top_item_sym(map_t* map,int x,int y);
char get_top_item_sym_from_stack(item_map_t* items);
item_t* item_create_from_data(int index);
int items_display(map_t* map,int x,int y);
#endif
