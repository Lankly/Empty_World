#include "item_callbacks.h"
#include "status.h"
#include "creature.h"
#include "player.h"

void defaultUseCallback(struct item_use_t* data){
  msg_add("Nothing happened!");}
void defaultConsumeCallback(struct item_consume_t* data){
  msg_add("Nothing happened!");}
void defaultZapCallback(struct item_zap_t* data){
  msg_add("Nothing happened!");}

void ironSwordUseCallback(struct item_use_t* data){
  if(data==NULL){
    quit("Error: Cannot use item on NULL");
  }
  //If the item was used on a player
  if(data->type==CALLBACK_TYPE_PLAYER){
    damage_creature(player,data->item->damage);
  }
  //If it was used on a creature
  else if(data->type==CALLBACK_TYPE_CREATURE){
    damage_creature(data->creature, data->caller->damage);
  }
  //If it was used on another item
  else if(data->type==CALLBACK_TYPE_ITEM){
    if(data->item == NULL){quit("Error: Cannot damage NULL item");}
    if(data->caller==NULL){quit("Error: Item_Use caller is NULL");}
    if(data->caller->material > data->item->material){
      destroy_item(data->item);
      msg_add("The blade completely destroys it!");
    }
    else{msg_add("The blade glances off, barely scratching it.");}
  }
  //Otherwise, error
  else if(data->type<0 || data->type>CALLBACK_TYPE_MAX){
    quit("Error: Invalid target type");
  }
}
