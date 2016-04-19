#include "item_callbacks.h"
#include "status.h"
#include "creature.h"

void defaultUseCallback(struct item_use_t* data){
  msg_add("Nothing happened!");}
void defaultConsumeCallback(struct item_consume_t* data){
  msg_add("Nothing happened!");}
void defaultZapCallback(struct item_zap_t* data){
  msg_add("Nothing happened!");}

void downStairUseCallback(struct item_use_t* data){
  if(data == NULL || data->item == NULL){
    quit("Error: Cannot access NULL Stair");
  }
  //If there's no map to go down to, make a new one.
  if(data->item->go_to_map == NULL){
    data->item->go_to_map = map_new();
    //map initialization
    map_init(data->item->go_to_map, 
	     map_get_width(cur_map), 
	     map_get_height(cur_map),
	     map_get_max_item_height(cur_map),
	     map_get_dlevel(cur_map) + 1);
    //Map draw
    int px, py;
    creature_get_coord(player, &px, &py);
    map_draw_random_rooms(data->item->go_to_map, px, py);
    map_cleanup(data->item->go_to_map);
    map_draw_borders(data->item->go_to_map);
    //Move player
    map_remove_creature(cur_map, player);
    map_add_creature(data->item->go_to_map, player);
    //Place stairs
    map_place_up_stair(data->item->go_to_map, px, py, cur_map);
    map_place_down_stair_randomly(data->item->go_to_map);
    
    //Spawners
    map_place_spawners(data->item->go_to_map);
  }
  else{  //Just move player if the map exists
    map_remove_creature(cur_map, player);
    map_add_creature(data->item->go_to_map, player);
  }
  //Make the actual change of map
  cur_map = data->item->go_to_map;
}

void upStairUseCallback(struct item_use_t* data){
  if(data == NULL || data->item == NULL || data->item->go_to_map == NULL){
    quit("Error: Cannot change to NULL Map");
  }
  map_add_creature(data->item->go_to_map, player);
  map_remove_creature(cur_map, player);
  cur_map = data->item->go_to_map;
}

void ironSwordUseCallback(struct item_use_t* data){
  if(data==NULL){
    quit("Error: Cannot use item on NULL");
  }
  //If the item was used on a player
  if(data->type==CALLBACK_TYPE_PLAYER){
    damage_creature(player, data->item->name, data->item->damage);
  }
  //If it was used on a creature
  else if(data->type==CALLBACK_TYPE_CREATURE){
    damage_creature(data->creature, data->item->name, data->item->damage);
  }
  //If it was used on another item
  else if(data->type==CALLBACK_TYPE_ITEM){
    if(data->item == NULL){quit("Error: Cannot damage NULL item");}
    if(data->caller==NULL){quit("Error: Item_Use caller is NULL");}
    if(data->caller->material > data->item->material){
      free(data->item);
      msg_add("The blade completely destroys it!");
    }
    else{msg_add("The blade glances off, barely scratching it.");}
  }
  //Otherwise, error
  else if(data->type<0 || data->type>CALLBACK_TYPE_MAX){
    quit("Error: Invalid target type");
  }
}
