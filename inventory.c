#include "inventory.h"
#include "helpers.h"
#include "player.h"

/* Initializes the player's inventory equip slots
 */
void inventory_init(struct creature_t* creature){
  creature->inventory = (inventory_t*)Calloc(1,sizeof(inventory_t));
  creature->inventory->head = 
    (equipment_head_t*)Calloc(1,sizeof(equipment_head_t));
  creature->inventory->chest = 
    (equipment_chest_t*)Calloc(1,sizeof(equipment_chest_t));
  creature->inventory->left_arm = 
    (equipment_arm_t*)Calloc(1,sizeof(equipment_arm_t));
  creature->inventory->right_arm = 
    (equipment_arm_t*)Calloc(1,sizeof(equipment_arm_t));
  creature->inventory->left_leg = 
    (equipment_leg_t*)Calloc(1,sizeof(equipment_leg_t));
  creature->inventory->right_leg = 
    (equipment_leg_t*)Calloc(1,sizeof(equipment_leg_t));
  creature->inventory->last_inventory_id=-1;
}

/* This method handles adding an item into the given creature's inventory
 */
bool inventory_add(struct creature_t* creature, item_t* item){
  if(item==NULL){quit("NULL item");}
  if(creature->inventory==NULL){quit("Creature->Inventory not initialized");}
  //Checks if there is room in the inventory
  if(creature->inventory->cur_weight + item->weight 
     > creature->inventory->max_weight){
    return false;}
  //If this is the first item added
  if(creature->inventory->first==NULL){
    creature->inventory->first =
      (inventory_node_t*)Calloc(1,sizeof(inventory_node_t));
    creature->inventory->last = creature->inventory->first;
    //Setup connections
    creature->inventory->first->next = creature->inventory->last;
    creature->inventory->first->item=item;
  }else{//Normal add
    inventory_node_t* to_add=(inventory_node_t*)Calloc(1,sizeof(inventory_node_t));
    to_add->item=item;
    creature->inventory->last->next=to_add;
    creature->inventory->last=to_add;
  }
  //Update weight, add an inventory id
  creature->inventory->cur_weight+=item->weight;
  creature->inventory->last->id = ++creature->inventory->last_inventory_id;
  return true;
}

/* This method handles removing an item from a given creature's inventory.
 * It takes in the item that it will remove. Like remove_item_by_id, this 
 * method does not destroy the item, just removes it from the inventory.
 */
void inventory_remove_by_item(struct creature_t* creature, item_t* item){
  if(creature==NULL){
    quit("Error: Cannot remove item from NULL creature's inventory.");}
  if(item==NULL){
    quit("Error: Cannot remove NULL item from creature's inventory.");}
  if(creature->inventory==NULL){
    quit("Error: Cannot remove item from creature's NULL inventory.");}

  /* Now we loop through all the items in the inventory, stopping when we
   * go through the whole thing or when the current item is the same as the 
   * one that we were given.
   */
  inventory_node_t* cur = creature->inventory->first;
  inventory_node_t* prev;
  while(cur!=NULL){
    //If found, begin removing item
    if(item == cur->item){
      //Case where remove first item
      if(prev==NULL){
	creature->inventory->first = cur->next;
      }
      else{prev->next = cur->next;}
      //Case where remove last item
      if(cur == creature->inventory->last){
	creature->inventory->last = prev;
      }
      cur->next = NULL;
      free(cur);
      return;
    }
    prev=cur;
    cur=cur->next;
  }
}

/* This method handles removing an item from the given creature's inventory.
 * It takes in an integer representing the item's location in the inventory.
 * Returns the item that it removed.
 */
item_t* inventory_remove_by_id(struct creature_t* creature, int inventory_id){
  if(inventory_id<0 || inventory_id>creature->inventory->last_inventory_id){
    quit("Error: Invalid inventory id.");}
  if(creature==NULL){
    quit("Error: Cannot remove item from NULL creature's inventory.");}
  if(creature->inventory==NULL){
    quit("Error: Cannot remove item from creature's NULL inventory.");}
  /* Now we loop through all the items in the inventory, stopping when we go
   * through the whole thing or when the current id is greater than the one
   * we're looking for (since item ids can only be in increasing order, 
   * we ignore buffer overflow, which is never going to happen)
   */
  inventory_node_t* cur=creature->inventory->first;
  inventory_node_t* prev;
  while(cur!=NULL && !cur->id>inventory_id){
    //If found, begin removing item
    if(cur->id == inventory_id){
      item_t* to_return = cur->item;
      //Case where remove first item
      if(prev==NULL){
	creature->inventory->first = cur->next;
      }
      else{prev->next=cur->next;}
      //Case where remove last item
      if(cur == creature->inventory->last){
	creature->inventory->last=prev;
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

/* This method handles equipping an item, presumably from the given creature's
 * inventory. This method looks at every single place the item can be equipped
 * to and will attempt to equip it there. Currently, an item can only be worn
 * on one slot, but I will change this to work with multiple slots in the future
 */
bool equip_by_item(struct creature_t* creature, item_t* item){
  if(creature==NULL){
    quit("Error: Cannot equip item to NULL creature.");}
  if(item==NULL){
    quit("Error: Cannot equip NULL item to creature.");}
  
  //Resist
  if(item->wearable == WEAR_RESIST){return false;}
  bool to_return = false;
  //Head
  if(item->wearable==WEAR_HEAD 
     && creature->inventory->head->hat == NULL){
    creature->inventory->head->hat = item;
    to_return = true;
  }
  //Chest
  if(item->wearable == WEAR_CHEST){
    if(!item->material == MAT_LEATHER 
       && !item->material == MAT_CLOTH 
       && creature->inventory->chest->armour == NULL){
      creature->inventory->chest->armour = item;
      to_return = true;
    }
    else if(creature->inventory->chest->shirt == NULL){
      creature->inventory->chest->shirt = item;
      to_return = true;
    }
  }
  //Forearm
  if(item->wearable == WEAR_ARM){
    if(creature->inventory->left_arm->forearm_armour == NULL){
      creature->inventory->left_arm->forearm_armour=item;
      to_return = true;}
    else if(creature->inventory->right_arm->forearm_armour == NULL){
      creature->inventory->right_arm->forearm_armour = item;
      to_return = true;}
  }
  //Hand
  if(item->wearable == WEAR_HAND){
    if(creature->inventory->left_arm->hand == NULL){
      creature->inventory->left_arm->hand = item;
      to_return = true;}
    else if(creature->inventory->right_arm->hand == NULL){
      creature->inventory->right_arm->hand = item;
      to_return = true;}
  }
  //Leg
  if(item->wearable == WEAR_LEG){
    if(creature->inventory->left_leg->leg_armour == NULL){
      creature->inventory->left_leg->leg_armour = item;
      to_return = true;}
    else if(creature->inventory->right_leg->leg_armour == NULL){
      creature->inventory->right_leg->leg_armour = item;
      to_return = true;}
  }
  //Foot
  if(item->wearable == WEAR_FOOT){
    if(creature->inventory->left_leg->shoe == NULL){
      creature->inventory->left_leg->shoe = item;
      to_return = true;}
    else if(creature->inventory->right_leg->shoe == NULL){
      creature->inventory->right_leg->shoe = item;
      to_return = true;}
  }
  //Fingers
  if(item->wearable == WEAR_FINGER){
    if(creature->inventory->left_arm->index == NULL){
      creature->inventory->left_arm->index = item;
      to_return = true;}
    else if(creature->inventory->left_arm->middle == NULL){
      creature->inventory->left_arm->middle = item;
      to_return = true;}
    else if(creature->inventory->left_arm->ring == NULL){
      creature->inventory->left_arm->ring = item;
      to_return = true;}
    else if(creature->inventory->left_arm->pinky == NULL){
      creature->inventory->left_arm->pinky = item;
      to_return = true;}
    else if(creature->inventory->right_arm->index == NULL){
      creature->inventory->right_arm->index = item;
      to_return = true;}
    else if(creature->inventory->right_arm->middle == NULL){
      creature->inventory->right_arm->middle = item;
      to_return = true;}
    else if(creature->inventory->right_arm->ring == NULL){
      creature->inventory->right_arm->ring = item;
      to_return = true;}
    else if(creature->inventory->right_arm->pinky == NULL){
      creature->inventory->right_arm->pinky = item;
      to_return = true;}
  }
  //Eyes
  if(item->wearable == WEAR_EYES 
     && creature->inventory->head->eyes == NULL){
    creature->inventory->head->eyes = item;
    to_return = true;}
  //Neck
  if(item->wearable == WEAR_NECK 
     && creature->inventory->neck == NULL){
    creature->inventory->neck=item;
    to_return = true;}
  //Shoulder
  if(item->wearable == WEAR_SHOULDER){
    if(creature->inventory->left_arm->shoulder_armour  ==  NULL){
      creature->inventory->left_arm->shoulder_armour = item;
      to_return = true;}
    else if(creature->inventory->right_arm->shoulder_armour  ==  NULL){
      creature->inventory->right_arm->shoulder_armour=item;
      to_return = true;}
  }
  return to_return;
}

/* This method handles equipping an item in the given creature's inventory.
 * The item is given by its id.
 */
bool equip_by_id(struct creature_t* creature, int inventory_id){
  if(creature == NULL){
    quit("Error: Cannot eqiup item to NULL creature");}
  if(inventory_id<0 || inventory_id>creature->inventory->last_inventory_id){
    quit("Error: invalid inventory id");
  }
  if(creature->inventory==NULL){quit("Error: Creature->Inventory not initialized");}
  /* Now we loop through all the items in the inventory, stopping when we
   * go through the whole thing or when the current id is greater than the
   * one we're looking for (since item ids can only be in increasing order, 
   * we ignore buffer overflow, which is never going to happen)
   */
  inventory_node_t* cur=creature->inventory->first;
  while(cur!=NULL && !cur->id>inventory_id){
    //If found, begin equip
    if(cur->id == inventory_id){
      item_t* to_equip = cur->item;
      //Here we will call equip_by_item for convenience's sake
      return equip_by_item(creature, to_equip);
    }
    //prev=cur;
    cur=cur->next;
  }
  //if the item was never found, return false
  return false;
}

/* This method handles unequipping an item from the given creature. It takes
 * the item itself that will be removed.
 */
bool unequip_by_item(struct creature_t* creature, item_t* item){
  /* This method does not error if the item cannot be unequipped. It
   * simply returns false instead.
   */
  if(creature == NULL || item == NULL){return false;}

  bool found=false;
  /* Begin looking through each possible equip slot.
   * Done this way so that if an item is equipped in multiple slots, 
   * it will be removed from each.
   */
  if(creature->inventory->head->hat != NULL 
     && creature->inventory->head->hat == item){
    creature->inventory->head->hat = NULL;
    found = true;}
  if(creature->inventory->head->eyes != NULL 
     && creature->inventory->head->eyes == item){
    creature->inventory->head->eyes = NULL;
    found = true;}
  if(creature->inventory->chest->armour != NULL 
     && creature->inventory->chest->armour == item){
    creature->inventory->chest->armour = NULL;
    found = true;}
  if(creature->inventory->chest->shirt != NULL 
     && creature->inventory->chest->shirt == item){
    creature->inventory->chest->shirt = NULL;
    found = true;}
  if(creature->inventory->left_arm->shoulder_armour != NULL 
     && creature->inventory->left_arm->shoulder_armour == item){
    creature->inventory->left_arm->shoulder_armour = NULL;
    found = true;}
  if(creature->inventory->left_arm->forearm_armour != NULL  
     && creature->inventory->left_arm->forearm_armour == item){
    creature->inventory->left_arm->forearm_armour = NULL;
    found = true;}
  if(creature->inventory->left_arm->hand != NULL 
     && creature->inventory->left_arm->hand == item){
    creature->inventory->left_arm->hand = NULL;
    found = true;}
  if(creature->inventory->left_arm->index != NULL 
     && creature->inventory->left_arm->index == item){
    creature->inventory->left_arm->index = NULL;
    found = true;}
  if(creature->inventory->left_arm->middle != NULL 
     && creature->inventory->left_arm->middle == item){
    creature->inventory->left_arm->middle = NULL;
    found = true;}
  if(creature->inventory->left_arm->ring != NULL 
     && creature->inventory->left_arm->ring == item){
    creature->inventory->left_arm->ring = NULL;
    found = true;}
  if(creature->inventory->left_arm->pinky != NULL 
     && creature->inventory->left_arm->pinky == item){
    creature->inventory->left_arm->pinky = NULL;
    found = true;}
  if(creature->inventory->left_arm->palm != NULL 
     && creature->inventory->left_arm->palm == item){
    creature->inventory->left_arm->palm = NULL;
    found = true;}
  if(creature->inventory->right_arm->shoulder_armour != NULL 
     && creature->inventory->right_arm->shoulder_armour == item){
    creature->inventory->right_arm->shoulder_armour = NULL;
    found = true;}
  if(creature->inventory->right_arm->forearm_armour != NULL 
     && creature->inventory->right_arm->forearm_armour == item){
    creature->inventory->right_arm->forearm_armour = NULL;
    found = true;}
  if(creature->inventory->right_arm->hand != NULL 
     && creature->inventory->right_arm->hand == item){
    creature->inventory->right_arm->hand = NULL;
    found = true;}
  if(creature->inventory->right_arm->index != NULL 
     && creature->inventory->right_arm->index == item){
    creature->inventory->right_arm->index = NULL;
    found = true;}
  if(creature->inventory->right_arm->middle != NULL 
     && creature->inventory->right_arm->middle == item){
    creature->inventory->right_arm->middle = NULL;
    found = true;}
  if(creature->inventory->right_arm->ring != NULL 
     && creature->inventory->right_arm->ring == item){
    creature->inventory->right_arm->ring = NULL;
    found = true;}
  if(creature->inventory->right_arm->pinky != NULL 
     && creature->inventory->right_arm->pinky == item){
    creature->inventory->right_arm->pinky = NULL;
    found = true;}
  if(creature->inventory->right_arm->palm != NULL 
     && creature->inventory->right_arm->palm == item){
    creature->inventory->right_arm->palm = NULL;
    found = true;}
  if(creature->inventory->left_leg->leg_armour != NULL 
     && creature->inventory->left_leg->leg_armour == item){
    creature->inventory->left_leg->leg_armour = NULL;
    found = true;}
  if(creature->inventory->left_leg->shoe != NULL 
     && creature->inventory->left_leg->shoe == item){
    creature->inventory->left_leg->shoe = NULL;
    found = true;}
  if(creature->inventory->right_leg->leg_armour != NULL 
     && creature->inventory->right_leg->leg_armour == item){
    creature->inventory->right_leg->leg_armour = NULL;
    found = true;}
  if(creature->inventory->right_leg->shoe != NULL 
     && creature->inventory->right_leg->shoe == item){
    creature->inventory->right_leg->shoe = NULL;
    found = true;}
  if(creature->inventory->pants != NULL 
     && creature->inventory->pants == item){
    creature->inventory->pants = NULL;
    found = true;}
  if(creature->inventory->neck != NULL 
     && creature->inventory->neck == item){
    creature->inventory->neck = NULL;
    found = true;}

  return found;
}

/* This method handles unequipping an item from the given creature. It takes
 * the id of the item in that creature's inventory and uses it to unequip the
 * item.
 */
bool unequip_by_id(struct creature_t* creature, int inventory_id){
  /* This method does not error when removing an item if it can't. It simmply
   * returns false instead.
   */
  if(creature == NULL 
     || inventory_id < 0 
     || inventory_id > creature->inventory->last_inventory_id){
    return false;}
  bool found=false;
  /* Begin looking through each possible equip slot.
   * Done this way so that if an item is equipped in multiple slots, 
   * it will be removed from each.
   */
  //On top of head
  if(creature->inventory->head->hat != NULL 
     && creature->inventory->head->hat->id == inventory_id){
    creature->inventory->head->hat = NULL;
    found = true;}
  //In/On eyes
  if(creature->inventory->head->eyes != NULL 
     && creature->inventory->head->eyes->id == inventory_id){
    creature->inventory->head->eyes = NULL;
    found = true;}
  //Chest Armour/Chest Shirt
  if(creature->inventory->chest->armour != NULL 
     && creature->inventory->chest->armour->id == inventory_id){
    creature->inventory->chest->armour = NULL;
    found = true;}
  if(creature->inventory->chest->shirt != NULL 
     && creature->inventory->chest->shirt->id == inventory_id){
    creature->inventory->chest->shirt = NULL;
    found = true;}
  //Shoulder Armour
  if(creature->inventory->left_arm->shoulder_armour != NULL 
     && creature->inventory->left_arm->shoulder_armour->id == inventory_id){
    creature->inventory->left_arm->shoulder_armour = NULL;
    found = true;}
  //Forearm Armour
  if(creature->inventory->left_arm->forearm_armour != NULL
     && creature->inventory->left_arm->forearm_armour->id==inventory_id){
    creature->inventory->left_arm->forearm_armour = NULL;
    found = true;}
  //Left Hand
  if(creature->inventory->left_arm->hand != NULL 
     && creature->inventory->left_arm->hand->id == inventory_id){
    creature->inventory->left_arm->hand=NULL;
    found = true;}
  //Left Arm's Index Finger
  if(creature->inventory->left_arm->index != NULL
     && creature->inventory->left_arm->index->id == inventory_id){
    creature->inventory->left_arm->index = NULL;
    found = true;}
  //Left Arm's Middle Finger
  if(creature->inventory->left_arm->middle != NULL 
     && creature->inventory->left_arm->middle->id == inventory_id){
    creature->inventory->left_arm->middle = NULL;
    found = true;}
  //Left Arm's Ring Finger
  if(creature->inventory->left_arm->ring != NULL 
     && creature->inventory->left_arm->ring->id == inventory_id){
    creature->inventory->left_arm->ring = NULL;
    found = true;}
  //Left Arm's Pinky Finger
  if(creature->inventory->left_arm->pinky != NULL 
     && creature->inventory->left_arm->pinky->id == inventory_id){
    creature->inventory->left_arm->pinky = NULL;
    found = true;}
  //Left Arm's palm (imbedded)
  if(creature->inventory->left_arm->palm != NULL 
     && creature->inventory->left_arm->palm->id == inventory_id){
    creature->inventory->left_arm->palm = NULL;
    found = true;}
  //Right Arm's Shoulder Armour
  if(creature->inventory->right_arm->shoulder_armour != NULL 
     && creature->inventory->right_arm->shoulder_armour->id == inventory_id){
    creature->inventory->right_arm->shoulder_armour = NULL;
    found = true;}
  if(creature->inventory->right_arm->forearm_armour != NULL 
     && creature->inventory->right_arm->forearm_armour->id == inventory_id){
    creature->inventory->right_arm->forearm_armour=NULL;
    found = true;}
  if(creature->inventory->right_arm->hand != NULL
     && creature->inventory->right_arm->hand->id == inventory_id){
    creature->inventory->right_arm->hand=NULL;
    found = true;}
  if(creature->inventory->right_arm->index != NULL 
     && creature->inventory->right_arm->index->id == inventory_id){
    creature->inventory->right_arm->index=NULL;
    found = true;}
  if(creature->inventory->right_arm->middle != NULL 
     && creature->inventory->right_arm->middle->id == inventory_id){
    creature->inventory->right_arm->middle=NULL;
    found = true;}
  if(creature->inventory->right_arm->ring != NULL 
     && creature->inventory->right_arm->ring->id == inventory_id){
    creature->inventory->right_arm->ring=NULL;
    found = true;}
  if(creature->inventory->right_arm->pinky != NULL 
     && creature->inventory->right_arm->pinky->id == inventory_id){
    creature->inventory->right_arm->pinky=NULL;
    found = true;}
  if(creature->inventory->right_arm->palm != NULL 
     && creature->inventory->right_arm->palm->id == inventory_id){
    creature->inventory->right_arm->palm=NULL;
    found = true;}
  if(creature->inventory->left_leg->leg_armour != NULL 
     && creature->inventory->left_leg->leg_armour->id == inventory_id){
    creature->inventory->left_leg->leg_armour = NULL;
    found = true;}
  if(creature->inventory->left_leg->shoe != NULL 
     && creature->inventory->left_leg->shoe->id == inventory_id){
    creature->inventory->left_leg->shoe = NULL;
    found = true;}
  if(creature->inventory->right_leg->leg_armour != NULL 
     && creature->inventory->right_leg->leg_armour->id == inventory_id){
    creature->inventory->right_leg->leg_armour = NULL;
    found = true;}
  if(creature->inventory->right_leg->shoe != NULL 
     && creature->inventory->right_leg->shoe->id == inventory_id){
    creature->inventory->right_leg->shoe = NULL;
    found = true;}
  if(creature->inventory->pants != NULL 
     && creature->inventory->pants->id == inventory_id){
    creature->inventory->pants = NULL;
    found = true;}
  if(creature->inventory->neck != NULL 
     && creature->inventory->neck->id == inventory_id){
    creature->inventory->neck = NULL;
    found = true;}

  return found;
}

void display_inventory(){
  if(player->inventory == NULL || player->inventory->first == NULL){return;}
  //Convert player->inventory_t to diplay_list_t
  inventory_node_t* cur_node = player->inventory->first;
  display_list_t* items = Calloc(1,sizeof(display_list_t));
  display_list_node_t* disp=Calloc(1,sizeof(display_list_node_t));
  disp->data=cur_node->item->exam_text;
  items->first=disp;
  cur_node=cur_node->next;
  while(cur_node != NULL){
    disp->next=Calloc(1,sizeof(display_list_node_t));
    disp->data=cur_node->item->exam_text;
    disp=disp->next;
    cur_node=cur_node->next;
  }
  display(items,80,24);
}
