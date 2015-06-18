#include "creature_callbacks.h"
#include "creature.h"
#include "player.h"
#include "map.h"
#include "items.h"
#include "tiles.h"

/* Move to the player, if visible.
 * Wander around randomly otherwise.
 */
void defaultPathfindCallback(struct creature_t* creature, 
			     map_t* map, 
			     item_map_t* items){
  //Make sure everything is right before starting
  if(creature==NULL){quit("ERROR: Cannot move NULL Creature");}
  if(map==NULL){quit("Error: Cannot move Creature on NULL Map");}
  if(map_get_tile(map, creature->x, creature->y)==TILE_UNKNOWN){
    return;
  }

  //If it can see the player, try to attack
  if(creature_is_visible(player, creature)){
    int x_diff = player->x - creature->x;
    int y_diff = player->y - creature->y;
    //Next, check if the player is adjacent
    if(x_diff >= -1 && x_diff <= 1 && y_diff >= -1 && y_diff <= 1){
      damage_creature(player, creature->name, creature_get_damage(creature));
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
			     struct map_t* map){
  //Make sure everything is right before starting
  if(creature==NULL){quit("ERROR: Cannot move NULL Creature");}
  if(map==NULL){quit("Error: Cannot move Creature on NULL Map");}
  
  int x_diff = player->x-creature->x;
  int y_diff = player->y-creature->y;
  if(x_diff>=-1 && x_diff<=1 && y_diff>=-1 && y_diff<=-1){
    damage_creature(player, creature->name, creature_get_damage(creature));
  }
}

/* All a spawner does on its turn is spawn creatures, with a small chance each
 * time. 
 */
void spawnerTakeTurnCallback(struct creature_t* creature,
			     struct map_t* map){
  if(map == NULL){
    quit("Error: Cannot spawn on NULL Map");}

  //Works one in 500 times
  if(rand() % 500 !=0){return;}
  
  /* Decide what creature to spawn. Does not spawn that creature if not on the
   * proper floor range.
   */
  int id = 0;
  for(;id == CREATURE_UNKNOWN
	|| id == CREATURE_SPAWNER
	|| (map->dlevel <1 && id == CREATURE_HUMAN_WARRIOR)
	|| (map->dlevel <1 && id == CREATURE_HUMAN_BOWMAN)
	|| (map->dlevel <1 && id == CREATURE_HUMAN_PIKEMAN)
	|| (map->dlevel <1 && id == CREATURE_HUMAN_MONK)
	|| (map->dlevel <1 && id == CREATURE_HUMAN_WIZARD)
	|| (map->dlevel < 5 && id == CREATURE_WERECREATURE)
	|| ((map->dlevel < 2 || map->dlevel > 8) && id==CREATURE_GOBLIN_WARRIOR)
	|| ((map->dlevel < 2 || map->dlevel > 8) && id==CREATURE_GOBLIN_BOWMAN)
	|| ((map->dlevel < 2 || map->dlevel > 8) && id==CREATURE_GOBLIN_PIKEMAN)
	|| ((map->dlevel < 2 || map->dlevel > 7) && id==CREATURE_GOBLIN_WIZARD)
	|| ((map->dlevel < 5 || map->dlevel > 12) && id == CREATURE_ORC_WARRIOR)
	|| ((map->dlevel < 5 || map->dlevel > 12) && id == CREATURE_ORC_BOWMAN)
	|| ((map->dlevel < 5 || map->dlevel > 12) && id == CREATURE_ORC_PIKEMAN)
	|| ((map->dlevel < 5 || map->dlevel > 12) && id == CREATURE_ORC_WIZARD)
	|| ((map->dlevel < 12 || map->dlevel > 15) && id == CREATURE_MINDFLAYER)
	|| ((map->dlevel < 1 || map->dlevel>5) && id==CREATURE_SKELETON_WARRIOR)
	|| ((map->dlevel < 1 || map->dlevel>5) && id==CREATURE_SKELETON_BOWMAN)
	|| ((map->dlevel < 1 || map->dlevel>5) && id==CREATURE_SKELETON_PIKEMAN)
	|| ((map->dlevel < 1 || map->dlevel > 2) && id == CREATURE_PUPPY)
	|| ((map->dlevel < 1 || map->dlevel > 4) && id == CREATURE_DOG)
	|| id == CREATURE_WARDOG
	|| ((map->dlevel < 1 || map->dlevel > 2) && id == CREATURE_KITTEN)
	|| ((map->dlevel < 1 || map->dlevel > 3) && id == CREATURE_HOUSECAT)
	|| id == CREATURE_WARCAT
	|| ((map->dlevel < 3 || map->dlevel > 12) && id == CREATURE_LION)
	|| ((map->dlevel < 3 || map->dlevel > 12) && id == CREATURE_PUMA)
	|| ((map->dlevel < 3 || map->dlevel > 12) && id == CREATURE_WOLF)
	|| ((map->dlevel < 3 || map->dlevel > 12) && id == CREATURE_HORSE)
	|| ((map->dlevel < 13 || map->dlevel > 25) && id==CREATURE_UNICORN)
	|| ((map->dlevel < 13 || map->dlevel > 24) && id==CREATURE_UNICORN_GOOD)
	|| ((map->dlevel < 13 || map->dlevel > 25) && id==CREATURE_UNICORN_EVIL)
	|| ((map->dlevel < 1 || map->dlevel > 3) && id ==CREATURE_PAPER_GOLEM)
	|| ((map->dlevel < 2 || map->dlevel > 6) && id ==CREATURE_ROCK_GOLEM)
	|| ((map->dlevel < 5 || map->dlevel > 9) && id ==CREATURE_SILVER_GOLEM)
	|| ((map->dlevel < 8 || map->dlevel > 12) && id==CREATURE_GOLD_GOLEM)
	|| ((map->dlevel < 11 || map->dlevel >15) && id==CREATURE_IRON_GOLEM)
	|| ((map->dlevel < 7 || map->dlevel > 17) && id ==CREATURE_FLOATING_EYE)
	;
      id = (rand() % (CREATURE_MAX-1)) + 1);
  
  //Create the creature from data
  struct creature_t *c = creature_create_from_data(id);
  c->dlevel = map->dlevel;
  
  //Now add it to the map
  map_add_creature(map, c);
  do{
    creature_place_on_map(c, map);
  }while(creature_is_visible(c, player));
}
