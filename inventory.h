#include <stdlib.h>
#include "items.h"
#include "creature.h"

#ifndef INVENTORY_H
#define INVENTORY_H

struct creature_t;

typedef struct{
  item_t* item;
  bool contributes_to_weight;
}equipment_t;

//This defines an inventory for creatures.
typedef struct inventory_node_t{
  item_t* item;
  int id;//the order in which items were added
  
  struct inventory_node_t* next;
}inventory_node_t;

typedef struct{
  int max_weight;
  int cur_weight;
  int num_items;

  inventory_node_t* first;
  inventory_node_t* last;
  int last_inventory_id;
}inventory_t;

inventory_t *inventory_new();
bool inventory_add(inventory_t *inv, item_t *item);
item_t *inventory_remove_by_item(inventory_t *inv, item_t *item);
item_t* inventory_remove_by_id(inventory_t *inv, int inventory_id);
void display_inventory(inventory_t *inv);

#endif
