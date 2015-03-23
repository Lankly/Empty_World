#include "creature_callbacks.h"
#include "creature.h"
#include "player.h"
#include "map.h"
#include "items.h"
#include "tiles.h"

/* Move to the player, if visible.
 * Wander around randomly otherwise.
 */
void defaultPathfindCallback(creature_t* creature, 
			     map_t* map, 
			     item_map_t* items){
  //Make sure everything is right before starting
  if(creature==NULL){quit("ERROR: Cannot move NULL Creature");}
  if(map==NULL){quit("Error: Cannot move Creature on NULL Map");}
  if(map_get_tile(map, creature->x, creature->y)==TILE_UNKNOWN){
    quit("Error: Creature cannot exist on TILE_UNKNOWN");
  }

  //If it can see the player, go towards it
  if(creature_is_visible(creature)){
    int x_diff = player->x-creature->x;
    int y_diff = player->y-creature->y;
    //Next, check if the player is adjacent
    if(x_diff>=-1 && x_diff<=1 && y_diff>=-1 && y_diff<=-1){
      damage_creature(player,creature_get_damage(creature));
    }
    //If not, move towards it
    else{
      int move_x=0, move_y=0;
      //If the creature is to the right of the player
      if(player->x - creature->x < 0){move_x--;}
      //If the creature is to the left of the player
      else if(player->x - creature->x > 0){move_x++;}
      //If the creature is below the player
      if(player->x - creature->y < 0){move_y--;}
      //If the creature is above the player
      else if(player->x - creature->y > 0){move_y++;}
      
      creature->x+=move_x;
      creature->y+=move_y;
    }
  }
  //Else wander randomly
  else{
    //It is perfectly okay for it to not move at all for its turn
    int move_x=rand()%2, move_y=rand()%2;
    while(!tile_data[map_get_tile(cur_map,
				  creature->x+move_x,
				  creature->y+move_y)].passable){
      creature->x = creature->x + move_x;
      creature->y = creature->y + move_y;
    }
  }
}

void defaultTakeTurnCallback(struct creature_t* creature,
			     struct map_t* map,
			     struct item_map_t* items){
  //Make sure everything is right before starting
  if(creature==NULL){quit("ERROR: Cannot move NULL Creature");}
  if(map==NULL){quit("Error: Cannot move Creature on NULL Map");}
  if(map_get_tile(map, creature->x, creature->y)==TILE_UNKNOWN){
    quit("Error: Creature cannot exist on TILE_UNKNOWN");
  }
  int x_diff = player->x-creature->x;
  int y_diff = player->y-creature->y;
  if(x_diff>=-1 && x_diff<=1 && y_diff>=-1 && y_diff<=-1){
    damage_creature(player,creature_get_damage(creature));
  }
  else{
    (*creature->takeTurn)(creature, map, items);
  }
}
