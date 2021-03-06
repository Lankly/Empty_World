#include <stdlib.h>
#include "items.h"
#include "creature.h"

#ifndef INVENTORY_H
#define INVENTORY_H

typedef struct inventory_node_t inventory_node_t;
typedef struct inventory_t inventory_t;

inventory_t *inventory_new();
bool inventory_add(inventory_t *inv, item_t *item);
bool inventory_empty(inventory_t *inv);
void inventory_purge(inventory_t *inv);
item_t *inventory_remove_by_item(inventory_t *inv, item_t *item);
item_t* inventory_remove_by_id(inventory_t *inv, int inventory_id);

void display_inventory(inventory_t *inv);

#endif
