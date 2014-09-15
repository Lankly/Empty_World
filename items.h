#include <stdbool.h>
#include "status.h"

#ifndef ITEMS_H
#define ITEMS_H

#define ITEM_UNKNOWN 0
#define ITEM_IRON_SWORD 1
#define ITEM_MAX 1

typedef struct{
  int id;
  int str_bonus;int per_bonus;int end_bonus;int cha_bonus;
  int int_bonus;int agl_bonus;int luc_bonus;int hth_bonus;
  int res_bonus0;int res_bonus1;

  int dmg_type0;//See damage types below
  int dmg_type1;int res_type0;int res_type1;
  int mob_advantage0;//refer to mob catalogue
  int mob_advantage1; int mob_advantage2;
  int mob_disadvantage0;int mob_disadvantage1;int mob_disadvantage2;

  int display;
  char* exam_text;
  int gold_value;
  int size;//how much space it takes up on an item stack, in a bag
  int material;
  int weight;
  bool legendary;//if true, only one may exist
  
  int edible;//0=inedible,1=food,2=water,3=swallowable
  int food_type;//See food types list below
  int nutrition;//how healthy it is for you
  int satiation;//how much space it takes up in your stomach
  int grant_intrinsic;//See *trinsic list below
  int intrinsic_probability;//up to 100

  int wearable;
  bool is_weapon;
  bool is_two_handed;
  int extrinsic;

  int curse_lvl;//0=uncursed,1=blessed,2=cursed
  int water_breathing_lvl;//0=normal,1=shallow,2=deep
  int turn_to_mob;//0=default, refer to mob catalogue for more

}item_t;

//DAMAGE TYPES LIST
#define DMG_UNKNOWN 0
#define DMG_BLUNT   1
#define DMG_FIRE    2
#define DMG_ICE     3
#define DMG_LIGHTNING 4
#define DMG_PSYCHIC 5

//Material Types
#define MAT_UNKNOWN 0
#define MAT_WOOD    1
#define MAT_IRON    2
#define MAT_LEATHER 3
#define MAT_CLOTH   4
#define MAT_SILVER  5
#define MAT_GOLD    6

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

//FOOD TYPES LIST
#define FOOD_UNKNOWN 0
#define FOOD_MEAT  1
#define FOOD_BREAD 2
#define FOOD_DRUG  3
#define FOOD_MAX 3

//DRINK TYPES LIST
#define DRINK_UNKNOWN 0
#define DRINK_WATER 1
#define DRINK_ALCOHOL 2
#define DRINK_POTION  3
#define DRINK_MAX 3

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
#define WEAR_MAX     10

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
  int size;
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
#endif
