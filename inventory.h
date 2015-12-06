#include <stdlib.h>
#include "items.h"
#include "creature.h"

#ifndef INVENTORY_H
#define INVENTORY_H

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

//This defines an inventory for creatures.
typedef struct inventory_node_t{
  item_t* item;
  int id;//the order in which items were added
  
  struct inventory_node_t* next;
}inventory_node_t;

typedef struct{
  equipment_head_t* head;
  equipment_chest_t* chest;
  equipment_arm_t* left_arm;
  equipment_arm_t* right_arm;
  equipment_leg_t* left_leg;
  equipment_leg_t* right_leg;
  item_t* pants;
  item_t* neck;
  item_t* wield;

  int max_weight;
  int cur_weight;
  int num_items;

  struct inventory_node_t* first;
  struct inventory_node_t* last;
  int last_inventory_id;
}inventory_t;

void inventory_init(struct creature_t* creature);
bool inventory_add(struct creature_t* creature, item_t* item);
void inventory_remove_by_item(struct creature_t* creature, item_t* item);
item_t* inventory_remove_by_id(struct creature_t* creature, int inventory_id);
bool equip_by_item(struct creature_t* creature, item_t* item);
bool equip_by_id(struct creature_t* creature, int inventory_id);
bool unequip_by_item(struct creature_t* creature, item_t* item);
bool unequip_by_id(struct creature_t* creature, int inventory_id);
void display_inventory(struct creature_t *c);

#endif
