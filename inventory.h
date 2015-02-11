#include <stdlib.h>
#include "items.h"

#ifndef INVENTORY_H
#define INVENTORY_H

typedef struct inventory_node_t{
  item_t* item;
  int id;//the order in which items were added
  struct inventory_node_t* next;
}inventory_node_t;

typedef struct{
  int max_weight;
  int cur_weight;
  inventory_node_t* first;
  inventory_node_t* last;
}inventory_t;

typedef struct{
  item_t* item;
  bool contributes_to_weight;
}equipment_t;

typedef struct{
  item_t* hat;
  item_t* eyes;
}equipment_head_t;

typedef struct{
  item_t* shoulder_armour;
  item_t* forearm_armour;
  item_t* hand;
  item_t* index; 
  item_t* middle;
  item_t* ring;
  item_t* pinky;
  item_t* palm;
}equipment_arm_t;

typedef struct{
  item_t* leg_armour;
  item_t* shoe;
}equipment_leg_t;

typedef struct{
  item_t* armour;
  item_t* shirt;
}equipment_chest_t;

inventory_t* inventory;
equipment_head_t* head;
equipment_chest_t* chest;
equipment_arm_t* left_arm;
equipment_arm_t* right_arm;
equipment_leg_t* left_leg;
equipment_leg_t* right_leg;
item_t* pants;
item_t* neck;
int last_inventory_id;

void inventory_init();
bool inventory_add(item_t* item);
item_t* inventory_remove(int inventory_id);
bool equip(int inventory_id);
bool unequip(int inventory_id);
void display_inventory();

#endif
