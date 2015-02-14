#include "item_callbacks.h"
#include "status.h"
#include "player.h"

void defaultUseCallback(item_use_t* data){msg_add("Nothing happened!");}
void defaultConsumeCallback(item_consume_t* data){msg_add("Nothing happened!");}

void ironSwordUseCallback(item_use_t* data){
  if(data==NULL){
    quit("Error: Cannot use item on NULL");
  }
  if(data->type==CALLBACK_TYPE_PLAYER){
    damage_player(player);
  }
  else if(data->type==CALLBACK_TYPE_CREATURE){
    damage_creature(data->creature;
  }
  else if(data->type==CALLBACK_TYPE_ITEM){
    if(data->item == NULL){quit("Error: Cannot damage NULL item");}
    if(data->caller==NULL){quit("Error: Item_Use caller is NULL");}
    if(data->caller->material > data->item->material){
      destroy_item(data->item);
      addmsg("The blade completely destroys it!");
    }
    else{addmsg("The blade glances off, barely scratching it.");}
  }
  else if(data->type<0 || data->type>CALLBACK_TYPE_MAX){
    quit("Error: Invalid target type");
  }
}
