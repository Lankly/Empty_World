#include "creature.h"
#include "creature_callbacks.h"
#include "items.h"
#include "inventory.h"
#include "map.h"
#include "player.h"
#include "tiles.h"
#include <stdbool.h>

/* Initializes all the possible default creatures. Certain fields will not be
 * initialized, because we can't know what they'll be in advance, such as level,
 * since that will be determined by the floor number and the player's level.
 */
void creature_data_init(){
  creature_data[CREATURE_UNKNOWN]=(creature_t){
    .takeTurn=&defaultTakeTurnCallback
  };
  creature_data[CREATURE_HUMAN_WARRIOR]=(creature_t){
    .corpse_type = CORPSE_HUMAN,
    .name = "This creature hasn't had it's name initialized!",
    .is_awake = true,
    .can_move = true,
    .can_fly = false,
    .can_see = true,
    .takeTurn = &defaultTakeTurnCallback
  };
}

/* This method will kill the creature and place its corpse and items
 */
void creature_kill(struct creature_t* creature){
  if(creature==NULL){quit("ERROR: Cannot kill NULL Creature");}
  item_t* corpse = item_create_from_data(creature->corpse_type);
  add_item(cur_map, creature->x, creature->y, corpse);

  if(creature->inventory!=NULL){
    while(creature->inventory->first!=NULL){
      item_t* next = creature->inventory->first->item;
      if(next!=NULL && rand()%2){
	add_item(cur_map, creature->x, creature->y, next);
      }
      inventory_node_t* to_remove = creature->inventory->first;
      creature->inventory->first = to_remove->next;
      free(to_remove);
    }
    free(creature->inventory);
  }
  free(creature);
}

/* This method handles how a creature takes damage.
 */
void damage_creature(struct creature_t* creature,int dmg){
  creature->health -= dmg;
  if(creature->health<=0){
    creature_kill(creature);}
}

/* This method places the given creature on the given map
 * randomly if it was placed for the first time,
 * at the stairs otherwise.
 */
void creature_place_on_map(struct creature_t* creature, map_t* map){
  /* If the creature is on the same dlevel in which it is being placed,
   * then we will place it randomly
   */
  if(creature->dlevel == map->dlevel){
    int x=rand()%map->width, y=rand()%map->height;
    while(!tile_data[map->tiles[get_coord(x,y,map->width)]].passable){
      x= rand()%map->width;
      y= rand()%map->height;
    }
    creature->x=x; 
    creature->y=y;
  }
    creature->dlevel=map->dlevel;
}


/* This method will return a value that determines how far a given creature is
   currently able to see. This value is determined by their perception,
   intelligence, luck, hunger, and level.
*/
int creature_see_distance(struct creature_t* creature){
  if(creature == NULL){
    quit("Error: Cannot get see-distance of NULL creature");}
  
  // Now we can calculate the correct value
  int to_return = ((player->can_see ? 1 : 0)
		   * (player->is_awake ? 1 : 0)
		   * (creature->intelligence
		      + creature->luck
		      + creature->level)
		   * creature->perception 
		   / ((creature->max_hunger - creature->hunger) / 100 + 1));
  /* Had to assign to variable b/c returning the equation was causing
   * the control to reach the end of the function. This stopped it for
   * some reason unknown to me at this time.
   */
  return to_return;
}

/* This method returns true if the given creature is visible to the player,
 * and false otherwise
 */
bool creature_is_visible(struct creature_t* creature){
  if(creature == NULL){
    quit("ERROR: Cannot get visibility of NULL creature.");
  }

  /* Initial check to see if the creature is too far away or 
   * if the player is blind
   */
  int diff_x = player->x - creature->x;
  int diff_y = player->y - creature->y;

  diff_x = diff_x<0 ? -diff_x : diff_x;
  diff_y = diff_y<0 ? -diff_y : diff_y;

  if(diff_x>creature_see_distance(player) 
     || diff_y>creature_see_distance(player)){
    return false;
  }

  /* Next we need to see if there are any obstructive tiles in the way.
   * Creatures do not matter. It is assumed that the player can see all
   * the creatures in a certain range if unobstructed.
  */
  int check_x = creature->x - player->x;
  int check_y = creature->y - player->y;
  int check_coord = get_coord(creature->x + check_x,
			      creature->y + check_y,
			      cur_map->width);
  while(check_x != check_y){
    if(!tile_data[cur_map->tiles[check_coord]].transparent){
      return false;
    }
    /* This is how we will zone-in on the target, by finding which values
     * to increment or decrement, checking the tiles along the way, until
     * the difference between x and y is zero.
     */
    if(check_x>check_y && creature->x+check_x>creature->x){check_x--;}
    else if(check_x>check_y && creature->x+check_x<creature->x){check_y--;}
    else if(check_y>check_x && creature->y+check_y>creature->y){check_y++;}
    else{check_x++;}
    check_coord = get_coord(creature->x+check_x,
			    creature->y+check_y,
			    cur_map->width);
  }
  /* Now that x and y are the same, if the creature is "behind" any non-
   * transparent tiles, we know the player can't see them. We increment/
   * decrement both until we reach the creature's coordinates.
   */
  while(check_x != creature->x){
    if(!tile_data[cur_map->tiles[check_coord]].transparent){
      return false;
    }
    if(check_x > creature->x){check_x--;}
    else{ check_x++;}
    if(check_y > creature->y){check_y++;}
    else{ check_y++;}
  }
  return true;
}

/* This method calculates how much damage a creature will do. It takes into
 * account its weapon, its health, and its stranth, etc.
 */
int creature_get_damage(struct creature_t* creature){
  if(creature == NULL){
    quit("ERROR: Cannot get damage of NULL creature.");
  }
  int to_return = 0;
  if(creature->inventory->weild != NULL){
    /* Base weapon damage is a function of strength, health, and weapon damage
     */
    to_return += creature->inventory->weild->damage * 
      creature->strength > 2 ? 1 : .5 *
      creature->health > 10 ? 1 : .8;
    /* If the weapon is ranged, intelligence is taken into account.
     */
    if(creature->inventory->weild->ranged){
	to_return = to_return * creature->intelligence > 2 ? 1 : .8;
    }
  }
  /* If the creature is attacking with no weapon, it is attacking with its hands,
   * its claws, its teeth, etc., and the damage it does is equal to its stranth.
   */
  else{
    to_return = creature->strength;
  }
  return to_return;
}
 
void set_level(struct creature_t* c, int l){c->level=l;}
void set_dlevel(struct creature_t* c, int d){c->dlevel=d;}
void set_name(struct creature_t* c, char* n){c->name=n;}

void set_strength(struct creature_t* c, int s){c->strength=s;}
void set_perception(struct creature_t* c, int p){c->perception=p;}
void set_endurance(struct creature_t* c, int e){c->endurance=e;}
void set_charisma(struct creature_t* c, int ch){c->charisma=ch;}
void set_intelligence(struct creature_t* c, int i){c->intelligence=i;}
void set_agility(struct creature_t* c, int a){c->agility=a;}
void set_luck(struct creature_t* c, int l){c->luck=l;}
void set_gold(struct creature_t* c, int g){c->gold=g;}
 
void set_health(struct creature_t* c, int h){c->health=h;}
void set_max_hunger(struct creature_t* c, int m){c->max_hunger=m;} 
void set_hunger(struct creature_t* c, int h){c->hunger=h;}
 
char* get_name(struct creature_t* c){return c-> name;}
int get_health(struct creature_t* c){return c-> health;}
int get_level(struct creature_t* c){return c-> level;}
int get_dlevel(struct creature_t* c){return c-> dlevel;}

int get_strength(struct creature_t* c){return c->strength;}
int get_perception(struct creature_t* c){return c->perception;}
int get_endurance(struct creature_t* c){return c->endurance;}
int get_charisma(struct creature_t* c){return c->charisma;}
int get_intelligence(struct creature_t* c){return c->intelligence;}
int get_agility(struct creature_t* c){return c->agility;}
int get_luck(struct creature_t* c){return c->luck;}
int get_gold(struct creature_t* c){return c->gold;}

void add_health(struct creature_t* c, int h){c->health+=h;}
