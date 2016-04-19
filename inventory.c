#include <string.h>
#include "colors.h"
#include "inventory.h"
#include "helpers.h"
#include "status.h"

/* Initializes the player's inventory equip slots
 */
void inventory_init(inventory_t *inv){
  inv->last_inventory_id = -1;
  inv->num_items = 0;
}

inventory_t *inventory_new(){
  inventory_t *new = Calloc(1, sizeof(inventory_t));
  inventory_init(new);
  return new;
}

/* This method handles adding an item into the given creature's inventory.
 * If the item could not be added, returns false.
 */
bool inventory_add(inventory_t *inv, item_t *item){
  if(inv == NULL || item == NULL){
    return false;
  }
  
  //First, checks if there is room in the inventory
  if(inv->cur_weight + item->weight > inv->max_weight){
    return false;
  }

  //Actually create the spot in the inventory
  inventory_node_t *new = Calloc(1, sizeof(inventory_node_t));
  new->item = item;
  
  //Next, checks if this is the first item added
  if(inv->first == NULL){
    inv->first = new;
    inv->last = inv->first;
    inv->first->next = inv->last;
  }
  
  //Normal add
  else{
    inv->last->next = new;
    inv->last = new;
  }
  
  //Update weight, add an inventory id, increment count
  inv->cur_weight += item->weight;
  inv->last->id = ++inv->last_inventory_id;
  inv->num_items++;
  
  return true;
}

/* This method handles removing an item from a given creature's inventory.
 * It takes in the item that it will remove. Returns the removed item.
 * If no item was removed, returns NULL.
 */
item_t *inventory_remove_by_item(inventory_t *inv, item_t *item){
  if(inv == NULL || item == NULL){
    return NULL;
  }
  
  /* Now we loop through all the items in the inventory, stopping when we
   * go through the whole thing or when the current item is the same as the 
   * one that we were given.
   */
  inventory_node_t *cur = inv->first;
  inventory_node_t *prev;
  while(cur != NULL){
    //If found, begin removing item
    if(item == cur->item){
      item_t *to_return = cur->item;
      
      inv->num_items--;
      //Case where remove first item
      if(prev == NULL){
	inv->first = cur->next;
      }
      else{prev->next = cur->next;}
      //Case where remove last item
      if(cur == inv->last){
	inv->last = prev;
      }
      cur->next = NULL;
      free(cur);
      return to_return;;
    }
    prev = cur;
    cur = cur->next;
  }

  return NULL;
}

/* This method handles removing an item from the given creature's inventory.
 * It takes in an integer representing the item in the inventory.
 * Returns the item that was removed. Returns NULL if no change was made.
 */
item_t* inventory_remove_by_id(inventory_t *inv, int inventory_id){
  if(inv == NULL){
    return NULL;
  }
  
  /* Now we loop through all the items in the inventory, stopping when we go
   * through the whole thing or when the current id is greater than the one
   * we're looking for (since item ids can only be in increasing order, 
   * we ignore buffer overflow, which is never going to happen)
   */
  inventory_node_t* cur=inv->first;
  inventory_node_t* prev;
  while(cur!=NULL && cur->id <= inventory_id){
    //If found, begin removing item
    if(cur->id == inventory_id){
      item_t* to_return = cur->item;
      //Case where remove first item
      if(prev==NULL){
	inv->first = cur->next;
      }
      else{prev->next=cur->next;}
      //Case where remove last item
      if(cur == inv->last){
	inv->last=prev;
      }
      cur->next=NULL;
      free(cur);
      return to_return;
    }
    prev=cur;
    cur=cur->next;
  }
  return NULL;
}

/* Displays to the player the list of all items in their inventory. Since items
 * cannot have names longer than 16 characters, the layout for this menu will be
 * slightly different.
 */
void display_inventory(inventory_t *inv){
  if(inv == NULL || inv->first == NULL || inv->num_items == 0){
    msg_add("No items.");
    return;
  }

  //We need to figure out how wide we want our columns
  int col_width = MAX_NAME_LEN + 4; //4 = 'X - '
  
  //Initialize the array we will use with the display_list function
  int **items = Calloc(inv->num_items, sizeof(int*));
  
  //Initialize the char array we will use to format our strings
  char *format = Calloc(col_width+1, sizeof(char));
  
  int id = 0;
  //Create list of items out of the creature's inventory
  for(struct inventory_node_t *cur = inv->first;
      cur != NULL && id < inv->num_items;
      cur = cur->next){
    snprintf(format, col_width, "%c - %s",
	     (char)(((int)'a') + id),
	     cur->item->name);
    items[id] = str_to_ints(format, col_width);
    items[id][0] = (items[id][0]) | COLOR_PAIR(CP_GREEN_BLACK);
    memset(format, 0, col_width);
    id++;
  }
      
  display_list("< > To navigate, a-Z to use, ESC to quit",
	       items, inv->num_items, col_width);
}
