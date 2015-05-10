#include "creature.h"
#include "creature_callbacks.h"
#include "items.h"
#include "inventory.h"
#include "map.h"
#include "player.h"
#include "status.h"
#include "tiles.h"
#include <stdbool.h>

/* Initializes all the possible default creatures. Certain fields will not be
 * initialized, because we can't know what they'll be in advance, such as level,
 * since that will be determined by the floor number and the player's level.
 */
void creature_data_init(){
  creature_data[CREATURE_UNKNOWN] = (struct creature_t){
    .takeTurn=&defaultTakeTurnCallback
  };
  creature_data[CREATURE_HUMAN_WARRIOR] = (struct creature_t){
    .corpse_type = CORPSE_HUMAN,
    .name = "Human Warrior",
    .exam_text = "This is a human warrior.",
    .is_awake = true,
    .can_move = true,
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
  int to_return = (player->can_see
		   * player->is_awake
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

  return map_tile_is_visible(cur_map, creature->x, creature->y);
}

/* This method returns true if a given creature can move to a given tile, not
 * taking into account the creature's position.
 */
bool creature_can_move_to(struct creature_t* creature, int x, int y, int cmd){
  if(creature == NULL){quit("ERROR: Cannot analyze NULL Creature");}
  //Coordinates must be in-bounds, but should not be game-breaking if not
  if(x < 0 || x >= TERMINAL_WIDTH
     || y <0 || y >= TERMINAL_HEIGHT){
    return false;
  }

  //Get the tile we're looking at moving to
  tile_t move_tile = tile_data[cur_map->tiles[y * cur_map->width + x]];
  //Quick check to see if the tile allows passing
  if(!move_tile.passable){return false;}

  //Get all the tiles surrounding the one we're looking at
  tile_t u = tile_data[cur_map->tiles[(y-1) * cur_map->width + x]];
  tile_t d = tile_data[cur_map->tiles[(y+1) * cur_map->width + x]];
  tile_t r = tile_data[cur_map->tiles[y * cur_map->width + x+1]];
  tile_t l = tile_data[cur_map->tiles[y * cur_map->width + x-1]];

  /* For the creature to be able to pass, if they're trying to pass through a
   * corner, they cannot be caryying too much.
   */
  if(creature->inventory->cur_weight > PASS_WEIGHT 
     && ((cmd==KEY_HOME && !u.passable && !l.passable) 
	 || (cmd==KEY_PPAGE && !u.passable && !r.passable) 
	 || (cmd==KEY_END && !d.passable && !l.passable) 
	 || (cmd==KEY_NPAGE && !d.passable && !r.passable))){
    //If we were trying to move the player, print out a quick alert
    if(creature == player){
      msg_add("You are too heavy to pass through.");}
    return false;
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
  double to_return = 0;
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
  /* If the creature is attacking with no weapon, it is attacking with its 
   * hands, its claws, its teeth, etc., and the damage it does is equal to its 
   * stranth.
   */
  else{
    to_return = creature->strength;
  }
  return (int)to_return;
}
 
void set_level(struct creature_t* c, int l){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  c->level=l;
}

void set_dlevel(struct creature_t* c, int d){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  c->dlevel=d;
}

void set_name(struct creature_t* c, char* n){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  c->name=n;
}

void set_vision(struct creature_t* c, bool b){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  c->can_see = b;
}

void set_conscious(struct creature_t* c, bool b){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  c->is_awake = b;
}

void set_strength(struct creature_t* c, int s){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  c->strength=s;
}

void set_perception(struct creature_t* c, int p){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  c->perception=p;
}

void set_endurance(struct creature_t* c, int e){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  c->endurance=e;
}

void set_charisma(struct creature_t* c, int ch){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  c->charisma=ch;
}

void set_intelligence(struct creature_t* c, int i){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  c->intelligence=i;
}

void set_agility(struct creature_t* c, int a){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  c->agility=a;
}

void set_luck(struct creature_t* c, int l){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  c->luck=l;
}

void set_gold(struct creature_t* c, int g){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  c->gold=g;
}

 
void set_health(struct creature_t* c, int h){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  c->health=h;
}

void set_max_hunger(struct creature_t* c, int m){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  c->max_hunger=m;
}
 
void set_hunger(struct creature_t* c, int h){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  c->hunger=h;}
 
char* get_name(struct creature_t* c){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  return c-> name;
}

int get_health(struct creature_t* c){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  return c-> health;
}

int get_level(struct creature_t* c){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  return c-> level;
}

int get_dlevel(struct creature_t* c){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  return c-> dlevel;
}


int get_strength(struct creature_t* c){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
 return c->strength;
}

int get_perception(struct creature_t* c){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  return c->perception;
}

int get_endurance(struct creature_t* c){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  return c->endurance;
}

int get_charisma(struct creature_t* c){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  return c->charisma;
}

int get_intelligence(struct creature_t* c){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  return c->intelligence;
}

int get_agility(struct creature_t* c){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  return c->agility;
}

int get_luck(struct creature_t* c){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  return c->luck;
}

int get_gold(struct creature_t* c){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  return c->gold;
}


void add_health(struct creature_t* c, int h){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  c->health+=h;
}


int add_breathable(struct creature_t* creature, int type){
  //Make sure everything is right
  if(creature==NULL){quit("Error: Cannot set values of NULL Creature");}
  if(creature->breathables==NULL){quit("ERROR: NULL Creature_Breathables");}

  if(type<0 || type>BREATHE_MAX){
    return 1;
}
  
  struct breathe_node_t* next = creature->breathables->first;
  //Case where adding first breathable
  if(next==NULL){
    next=(struct breathe_node_t*)Calloc(1,sizeof(struct breathe_node_t));
    next->breathe_type=type;
    return 0;
  }
  /* Otherwise navigate to the end and add it, making sure 
   * the type isn't already in the list
   */
  while(next->next!=NULL){
    if(next->breathe_type==type){return 1;}
    next=next->next;
  }
  next->next =
    (struct breathe_node_t*)Calloc(1,sizeof(struct breathe_node_t));
  next->next->breathe_type=type;
  return 0;
}

int add_consumable(struct creature_t* creature, int type){
  //Make sure everything is right
  if(creature==NULL){
    quit("ERROR: NULL Creature");}
  if(creature->consumables==NULL){
    quit("ERROR: NULL Creature_Consumables");}
  if(type<0 || type>CONSUME_MAX){return 1;}

  struct consume_node_t* next = creature->consumables->first;
  //Case where adding first consumable
  if(next==NULL){
    next = (consume_node_t*)Calloc(1,sizeof(consume_node_t));
    next->consume_type = type;
    return 0;
  }
  /* Otherwise navigate to the end and add it, making sure 
   * the type isn't already in the list
   */
  while(next->next!=NULL){
    if(next->consume_type==type){return 1;}
    next=next->next;
  }
  next->next = (consume_node_t*)Calloc(1,sizeof(consume_node_t));
  next->next->consume_type = type;
  return 0;
}

int add_intrinsic(struct creature_t* creature, int type){
  //Make sure everything is right
  if(creature==NULL){quit("ERROR: NULL Creature");}
  if(creature->intrinsics==NULL){quit("ERROR: NULL Creature_Intrinsics");}
  if(type<0 || type>TRINSIC_MAX){return 1;}

  struct intrinsics_node_t* next = creature->intrinsics->first;
  //Case where adding first intrinsic
  if(next==NULL){
    next=(struct intrinsics_node_t*)
      Calloc(1,sizeof(struct intrinsics_node_t));
    next->intrinsic_type=type;
    return 0;
  }
  /* Otherwise navigate to the end and add it, making sure 
   * the type isn't already in the list
   */
  while(next->next!=NULL){
    if(next->intrinsic_type==type){return 1;}
    next=next->next;
  }
  next->next = 
    (struct intrinsics_node_t*)Calloc(1,sizeof(struct intrinsics_node_t));
  next->next->intrinsic_type=type;
  return 0;
}

int add_resistance(struct creature_t* creature, int type){
  //Make sure everything is right
  if(creature==NULL){quit("ERROR: NULL Creature");}
  if(creature->resistances==NULL){quit("ERROR: NULL Creature_Resistance");}
  if(type<0 || type>DMG_MAX){return 1;}

  struct resistances_node_t* next = creature->resistances->first;
  //Case where adding first resistance
  if(next==NULL){
    next = (struct resistances_node_t*)
      Calloc(1,sizeof(resistances_node_t));
    next->damage_type=type;
    return 0;
  }
  /*Otherwise navigate to the end and add it, making sure 
   * the type isn't already in the list
   */
  while(next->next!=NULL){
    if(next->damage_type==type){return 1;}
    next=next->next;
  }
  next->next = (resistances_node_t*)Calloc(1,sizeof(resistances_node_t));
  next->next->damage_type=type;
  return 0;
}

int remove_breathable(struct creature_t* creature, int type){
  //Make sure everything is right
  if(creature==NULL){
    quit("ERROR: NULL Creature");}
  /* In this function, if there are no items to remove, we react as if we have
   * successfully removed the item
   */
  if(creature->breathables==NULL || creature->breathables->first==NULL){
    return 0;}
  if(type<0 || type>BREATHE_MAX){return 1;}

  breathe_node_t* next = creature->breathables->first;
  //If it's the first node
  if(next->breathe_type==type){
    creature->breathables->first = next->next;
    free(next);
    return 0;
  }
  //Loop through to find it
  while(next->next!=NULL){
    if(next->next->breathe_type==type){
      breathe_node_t* temp = next->next;
      next->next=next->next->next;
      free(temp);
      return 0;
    }
  }
  //Not found
  return 1;
}

int remove_consumable(struct creature_t* creature, int type){
  //Make sure everything is right
  if(creature==NULL){
    quit("ERROR: NULL Creature");}
  /* In this function, if there are no items to remove, we react as if we have
   * successfully removed the item
   */
  if(creature->consumables==NULL || creature->consumables->first==NULL){
    return 0;}
  if(type<0 || type>CONSUME_MAX){return 1;}

  consume_node_t* next = creature->consumables->first;
  //If it's the first node
  if(next->consume_type==type){
    creature->consumables->first=next->next;
    free(next);
    return 0;
  }
  //Loop through to find it
  while(next->next!=NULL){
    if(next->next->consume_type==type){
      consume_node_t* temp = next->next;
      next->next=next->next->next;
      free(temp);
      return 0;
    }
  }
  //Not found
  return 1;
}

int remove_instrinsic(struct creature_t* creature, int type){
  //Make sure everything is right
  if(creature==NULL){
    quit("ERROR: NULL Creature");}
  /* In this function, if there are no items to remove, we react as if we have
   * successfully removed the item
   */
  if(creature->intrinsics==NULL || creature->intrinsics->first==NULL){
    return 0;}
  if(type<0 || type>TRINSIC_MAX){return 1;}

  intrinsics_node_t* next = creature->intrinsics->first;
  //If it's the first node
  if(next->intrinsic_type==type){
    creature->intrinsics->first=next->next;
    free(next);
    return 0;
  }
  //Loop through to find it
  while(next->next!=NULL){
    if(next->next->intrinsic_type==type){
      intrinsics_node_t* temp = next->next;
      next->next=next->next->next;
      free(temp);
      return 0;
    }
  }
  //Not found
  return 1;
}

int remove_resistance(struct creature_t* creature, int type){
  //Make sure everything is right
  if(creature==NULL){
    quit("ERROR: NULL Creature");}
  /* In this function, if there are no items to remove, we react as if we have
   * successfully removed the item
   */
  if(creature->resistances==NULL || creature->resistances->first==NULL){
    return 0;}
  if(type<0 || type>DMG_MAX){return 1;}

  resistances_node_t* next = creature->resistances->first;
  //If it's the first node
  if(next->damage_type==type){
    creature->resistances->first=next->next;
    free(next);
    return 0;
  }
  //Loop through to find it
  while(next->next!=NULL){
    if(next->next->damage_type==type){
      resistances_node_t* temp = next->next;
      next->next = next->next->next;
      free(temp);
      return 0;
    }
  }
  //Not found
  return 1;
}
