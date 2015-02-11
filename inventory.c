#include "inventory.h"
#include "helpers.h"

void inventory_init(){
  inventory = (inventory_t*)Calloc(1,sizeof(inventory_t));
  head      = (equipment_head_t*)Calloc(1,sizeof(equipment_head_t));
  chest     = (equipment_chest_t*)Calloc(1,sizeof(equipment_chest_t));
  left_arm  = (equipment_arm_t*)Calloc(1,sizeof(equipment_arm_t));
  right_arm = (equipment_arm_t*)Calloc(1,sizeof(equipment_arm_t));
  left_leg  = (equipment_leg_t*)Calloc(1,sizeof(equipment_leg_t));
  right_leg = (equipment_leg_t*)Calloc(1,sizeof(equipment_leg_t));
  last_inventory_id=-1;
}

bool inventory_add(item_t* item){
  if(item==NULL){quit("NULL item");}
  if(inventory==NULL){quit("Inventory not initialized");}
  //Checks if there is room in the inventory
  if(inventory->cur_weight+item->weight > inventory->max_weight){return false;}
  //If this is the first item added
  if(inventory->first==NULL){
    inventory->first=(inventory_node_t*)Calloc(1,sizeof(inventory_node_t));
    inventory->last=inventory->first;
    //Setup connections
    inventory->first->next=inventory->last;
    inventory->first->item=item;
  }else{//Normal add
    inventory_node_t* to_add=(inventory_node_t*)Calloc(1,sizeof(inventory_node_t));
    to_add->item=item;
    inventory->last->next=to_add;
    inventory->last=to_add;
  }
  //Update weight, add an inventory id
  inventory->cur_weight+=item->weight;
  inventory->last->id=++last_inventory_id;
  return true;
}

item_t* inventory_remove(int inventory_id){
  if(inventory_id<0 || inventory_id>last_inventory_id){quit("Error: invalid inventory id");}
  if(inventory==NULL){quit("Error: Inventory not initialized");}
  //Now we loop through all the items in the inventory, stopping when we go through the whole thing
  //or when the current id is greater than the one we're looking for 
  //(since item ids can only be in increasing order, ignoring buffer overflow, which is never going to happen)
  inventory_node_t* cur=inventory->first;
  inventory_node_t* prev;
  while(cur!=NULL && !cur->id>inventory_id){
    //If found, begin removing item
    if(cur->id==inventory_id){
      item_t* to_return = cur->item;
      //Case where remove first item
      if(prev==NULL){inventory->first=cur->next;}
      else{prev->next=cur->next;}
      //Case where remove last item
      if(cur==inventory->last){inventory->last=prev;}
      cur->next=NULL;
      free(cur);
      return to_return;
    }
    prev=cur;
    cur=cur->next;
  }
  return NULL;
}

bool equip(int inventory_id){
  if(inventory_id<0 || inventory_id>last_inventory_id){quit("Error: invalid inventory id");}
  if(inventory==NULL){quit("Error: Inventory not initialized");}
  //Now we loop through all the items in the inventory, stopping when we go through the whole thing
  //or when the current id is greater than the one we're looking for 
  //(since item ids can only be in increasing order, ignoring buffer overflow, which is never going to happen)
  inventory_node_t* cur=inventory->first;
  while(cur!=NULL && !cur->id>inventory_id){
    //If found, begin equip
    if(cur->id==inventory_id){
      item_t* to_equip = cur->item;
      //Resist
      if(to_equip->wearable==WEAR_RESIST){return false;}
      //Head
      else if(to_equip->wearable==WEAR_HEAD){if(head->hat==NULL){head->hat=to_equip;return true;}}
      //Chest
      else if(to_equip->wearable==WEAR_CHEST){
	if(!to_equip->material==MAT_LEATHER && !to_equip->material==MAT_CLOTH && chest->armour==NULL){chest->armour=to_equip;return true;}
	else if(chest->shirt==NULL){chest->shirt=to_equip;return true;}
      }
      //Forearm
      else if(to_equip->wearable==WEAR_ARM){
	if(left_arm->forearm_armour==NULL){left_arm->forearm_armour=to_equip;}
	else if(right_arm->forearm_armour==NULL){right_arm->forearm_armour=to_equip;}
      }
      //Hand
      else if(to_equip->wearable==WEAR_HAND){
	if(left_arm->hand==NULL){left_arm->hand=to_equip;return true;}
	else if(right_arm->hand==NULL){right_arm->hand=to_equip;return true;}
      }
      //Leg
      else if(to_equip->wearable==WEAR_LEG){
	if(left_leg->leg_armour==NULL){left_leg->leg_armour=to_equip;return true;}
	else if(right_leg->leg_armour==NULL){right_leg->leg_armour=to_equip;return true;}
      }
      //Foot
      else if(to_equip->wearable==WEAR_FOOT){
	if(left_leg->shoe==NULL){left_leg->shoe=to_equip;return true;}
	else if(right_leg->shoe==NULL){right_leg->shoe=to_equip;return true;}
      }
      //Fingers
      else if(to_equip->wearable==WEAR_FINGER){
	if(left_arm->index==NULL){left_arm->index=to_equip;return true;}
	else if(left_arm->middle==NULL){left_arm->middle=to_equip;return true;}
	else if(left_arm->ring==NULL){left_arm->ring=to_equip;return true;}
	else if(left_arm->pinky==NULL){left_arm->pinky=to_equip;return true;}
	else if(right_arm->index==NULL){right_arm->index=to_equip;}
	else if(right_arm->middle==NULL){right_arm->middle=to_equip;return true;}
	else if(right_arm->ring==NULL){right_arm->ring=to_equip;return true;}
	else if(right_arm->pinky==NULL){right_arm->pinky=to_equip;return true;}
      }
      //Eyes
      else if(to_equip->wearable==WEAR_EYES){if(head->eyes==NULL){head->eyes=to_equip;return true;}}
      //Neck
      else if(to_equip->wearable==WEAR_NECK){if(neck==NULL){neck=to_equip;return true;}}
      //Shoulder
      else if(to_equip->wearable==WEAR_SHOULDER){
	if(left_arm->shoulder_armour==NULL){left_arm->shoulder_armour=to_equip;return true;}
	else if(right_arm->shoulder_armour==NULL){right_arm->shoulder_armour=to_equip;return true;}
      }
      
      //if it was not equipped to anything, this function returns false
      return false;
    }
    //prev=cur;
    cur=cur->next;
  }
  //if the item was never found, return false
  return false;
}

bool unequip(int inventory_id){
  if(inventory_id<0 || inventory_id>last_inventory_id){return false;}

  bool found=false;
  //begin looking through each possible equip slot
  //Done this way so that if an item is equipped in multiple slots, it will be removed from each
  if(head->hat!=NULL  && head->hat->id==inventory_id)  {head->hat=NULL;found=true;}
  if(head->eyes!=NULL && head->eyes->id==inventory_id) {head->eyes=NULL;found=true;}
  if(chest->armour!=NULL && chest->armour->id==inventory_id) {chest->armour=NULL;found=true;}
  if(chest->shirt!=NULL  && chest->shirt->id==inventory_id)  {chest->shirt=NULL;found=true;}
  if(left_arm->shoulder_armour!=NULL && left_arm->shoulder_armour->id==inventory_id) {left_arm->shoulder_armour=NULL;found=true;}
  if(left_arm->forearm_armour!=NULL  && left_arm->forearm_armour->id==inventory_id)  {left_arm->forearm_armour=NULL;found=true;}
  if(left_arm->hand!=NULL   && left_arm->hand->id==inventory_id)                     {left_arm->hand=NULL;found=true;}
  if(left_arm->index!=NULL  && left_arm->index->id==inventory_id)                    {left_arm->index=NULL;found=true;}
  if(left_arm->middle!=NULL && left_arm->middle->id==inventory_id)                   {left_arm->middle=NULL;found=true;}
  if(left_arm->ring!=NULL   && left_arm->ring->id==inventory_id)                     {left_arm->ring=NULL;found=true;}
  if(left_arm->pinky!=NULL  && left_arm->pinky->id==inventory_id)                    {left_arm->pinky=NULL;found=true;}
  if(left_arm->palm!=NULL   && left_arm->palm->id==inventory_id)                     {left_arm->palm=NULL;found=true;}
  if(right_arm->shoulder_armour!=NULL && right_arm->shoulder_armour->id==inventory_id) {right_arm->shoulder_armour=NULL;found=true;}
  if(right_arm->forearm_armour!=NULL  && right_arm->forearm_armour->id==inventory_id)  {right_arm->forearm_armour=NULL;found=true;}
  if(right_arm->hand!=NULL   && right_arm->hand->id==inventory_id)                     {right_arm->hand=NULL;found=true;}
  if(right_arm->index!=NULL  && right_arm->index->id==inventory_id)                    {right_arm->index=NULL;found=true;}
  if(right_arm->middle!=NULL && right_arm->middle->id==inventory_id)                   {right_arm->middle=NULL;found=true;}
  if(right_arm->ring!=NULL   && right_arm->ring->id==inventory_id)                     {right_arm->ring=NULL;found=true;}
  if(right_arm->pinky!=NULL  && right_arm->pinky->id==inventory_id)                    {right_arm->pinky=NULL;found=true;}
  if(right_arm->palm!=NULL   && right_arm->palm->id==inventory_id)                     {right_arm->palm=NULL;found=true;}
  if(left_leg->leg_armour!=NULL && left_leg->leg_armour->id==inventory_id) {left_leg->leg_armour=NULL;found=true;}
  if(left_leg->shoe!=NULL       && left_leg->shoe->id==inventory_id)       {left_leg->shoe=NULL;found=true;}
  if(right_leg->leg_armour!=NULL && right_leg->leg_armour->id==inventory_id) {right_leg->leg_armour=NULL;found=true;}
  if(right_leg->shoe!=NULL       && right_leg->shoe->id==inventory_id)       {right_leg->shoe=NULL;found=true;}
  if(pants!=NULL && pants->id==inventory_id) {pants=NULL;found=true;}
  if(neck!=NULL  && neck->id==inventory_id)  {neck=NULL;found=true;}

  return found;
}

void display_inventory(){
  if(inventory==NULL || inventory->first==NULL){return;}
  //Convert inventory_t to diplay_list_t
  inventory_node_t* cur_node = inventory->first;
  display_list_t* items = Calloc(1,sizeof(display_list_t));
  display_list_node_t* disp=Calloc(1,sizeof(display_list_node_t));
  disp->data=cur_node->item->exam_text;
  items->first=disp;
  cur_node=cur_node->next;
  while(cur_node!=NULL){
    disp->next=Calloc(1,sizeof(display_list_node_t));
    disp->data=cur_node->item->exam_text;
    disp=disp->next;
    cur_node=cur_node->next;
  }
  display(items,80,24);
}
