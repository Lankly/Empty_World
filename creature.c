#include "classes.h"
#include "colors.h"
#include "creature.h"
#include "creature_callbacks.h"
#include "items.h"
#include "inventory.h"
#include "macros.h"
#include "map.h"
#include "status.h"
#include "tiles.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>

//This is a list of creatures
struct creaturelist_t{
  struct creature_t *creature;
  struct creaturelist_t *next;
};

//the most recent command given by the player
int cmd;

struct creature_t{
  int strength;  int perception;    int endurance;
  int charisma;  int intelligence;  int agility;
  int dexterity; int luck;
  int health;    int hunger;        int gold;
  int level;     int max_hunger;    int max_health;
  int cur_carry_weight;
  
  /* This is how hunger works:
   * You have a max_hunger. This represents how much food can be in your stomach
   * at one time. Above this, and the stomach will purge to 20% of its max. You 
   * also have a hunger integer that represents how much food is in your stomach
   * right now. Hunger is a large number, in the hundreds, and decrements once
   * every turn cycle.
   */

  int turn_tokens;
  int turn_tokens_reset_amount;
  
  int were_type;
  int class;
  int creature_id;
  int display;
  int type;
  int x; int y;
  int last_position; //As in keys on a numpad

  char *name;
  char *exam_text;
  
  bool is_asleep;
  bool is_immobile;
  bool is_flying;
  bool is_blind;
  bool is_telepathic;

  struct intlist_t *breathables;
  struct intlist_t *consumables;
  struct intlist_t *intrinsics;
  struct intlist_t *resistances;

  inventory_t *inventory;
  creatureTakeTurnCallback takeTurn;
  creaturePathfindCallback pathfind;
  creatureKillCallback kill;

  struct body_part_t *body;
};

//This is all the default kinds of creatures that can be created
creature_t creature_data[CREATURE_TYPE_MAX+1];


/* Initializes all the possible default creatures. Certain fields will not be
 * initialized, because we can't know what they'll be in advance, such as level,
 * since that will be determined by the floor number and the player's level.
 */
void creature_data_init(){
  creature_data[CREATURE_TYPE_UNKNOWN] = (struct creature_t){
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_TYPE_SPAWNER] = (struct creature_t){
    .creature_id = CREATURE_TYPE_SPAWNER,
    .display = '%' | COLOR_PAIR(CP_GREEN_BLACK),
    .name = "???",
    .exam_text = "???",
    .is_immobile = true,
    .is_blind = true,
    .max_health = 100,
    .takeTurn = &spawnerTakeTurnCallback,
    .turn_tokens = 1,
    .turn_tokens_reset_amount = 1
  };
  creature_data[CREATURE_TYPE_HUMAN] = (struct creature_t){
    .creature_id = CREATURE_TYPE_HUMAN,
    .display = '@',
    .name = "Human",
    .exam_text = "This is a human.",
    .max_health = 30,
    .strength = 2,
    .takeTurn = &defaultTakeTurnCallback,
    .turn_tokens = 1,
    .turn_tokens_reset_amount = 1
  };
  creature_data[CREATURE_TYPE_GOBLIN] = (struct creature_t){
    .creature_id = CREATURE_TYPE_GOBLIN,
    .display = 'G',
    .name = "Goblin",
    .exam_text = "This is a goblin.",
    .max_health = 35,
    .strength = 3,
    .takeTurn = &defaultTakeTurnCallback,
    .turn_tokens = 1,
    .turn_tokens_reset_amount = 1
  };
  creature_data[CREATURE_TYPE_ORC] = (struct creature_t){
    .creature_id = CREATURE_TYPE_ORC,
    .display = 'R',
    .name = "Orc",
    .exam_text = "This is an orc.",
    .max_health = 40,
    .strength = 4,
    .takeTurn = &defaultTakeTurnCallback,
    .turn_tokens = 1,
    .turn_tokens_reset_amount = 1
  };
  creature_data[CREATURE_TYPE_MINDFLAYER] = (struct creature_t){
    .creature_id = CREATURE_TYPE_MINDFLAYER,
    .display = 'M',
    .name = "Mindflayer",
    .exam_text = "This is a mindflayer.",
    .is_flying = true,
    .max_health = 120,
    .strength = 15,
    .takeTurn = &defaultTakeTurnCallback,
    .turn_tokens = 1,
    .turn_tokens_reset_amount = 1
  };
  creature_data[CREATURE_TYPE_SKELETON] = (struct creature_t){
    .creature_id = CREATURE_TYPE_SKELETON,
    .display = 'S',
    .name = "Skeleton",
    .exam_text = "This is a skeleton.",
    .max_health = 10,
    .strength = 2,
    .perception = 1,
    .takeTurn = &defaultTakeTurnCallback,
    .turn_tokens = 1,
    .turn_tokens_reset_amount = 1
  };

  creature_data[CREATURE_TYPE_RODENT] = (struct creature_t){
    .creature_id = CREATURE_TYPE_RODENT,
    .display = 'r',
    .name = "Rat",
    .exam_text = "This is a rat.",
    .max_health = 5,
    .strength = 1,
    .perception = 2,
    .intelligence = 1,
    .luck = 1,
    .takeTurn = &defaultTakeTurnCallback,
    .pathfind = &ratPathfindCallback,
    .kill = &defaultKillCallback,
    .turn_tokens = 1,
    .turn_tokens_reset_amount = 1
  };
  creature_data[CREATURE_TYPE_AVIAN] = (struct creature_t){
    .creature_id = CREATURE_TYPE_AVIAN,
    .display = 'o',
    .name = "Owl",
    .exam_text = "This is an owl.",
    .max_health = 5,
    .strength = 1,
    .perception = 2,
    .intelligence = 1,
    .luck = 1,
    .takeTurn = &defaultTakeTurnCallback,
    .turn_tokens = 1,
    .turn_tokens_reset_amount = 1
  };
  creature_data[CREATURE_TYPE_INSECT] = (struct creature_t){
    .creature_id = CREATURE_TYPE_INSECT,
    .display = 'h',
    .name = "Hornet",
    .exam_text = "This is a hornet.",
    .max_health = 5,
    .is_flying = true,
    .strength = 2,
    .perception = 2,
    .charisma = -1,
    .luck = 1,
    .takeTurn = &defaultTakeTurnCallback,
    .turn_tokens = 1,
    .turn_tokens_reset_amount = 1
  };
  creature_data[CREATURE_TYPE_CANINE] = (struct creature_t){
    .creature_id = CREATURE_TYPE_CANINE,
    .display = 'd',
    .name = "Dog",
    .exam_text = "This is a dog.",
    .max_health = 10,
    .strength = 3,
    .perception = 3,
    .intelligence = 2,
    .luck = 1,
    .takeTurn = &defaultTakeTurnCallback,
    .turn_tokens = 1,
    .turn_tokens_reset_amount = 1
  };
  creature_data[CREATURE_TYPE_FELINE] = (struct creature_t){
    .creature_id = CREATURE_TYPE_FELINE,
    .display = 'c',
    .name = "Cat",
    .exam_text = "This is a cat.",
    .max_health = 9,
    .strength = 2,
    .perception = 2,
    .intelligence = 3,
    .takeTurn = &defaultTakeTurnCallback,
    .turn_tokens = 1,
    .turn_tokens_reset_amount = 1
  };
  creature_data[CREATURE_TYPE_EQUINE] = (struct creature_t){
    .creature_id = CREATURE_TYPE_EQUINE,
    .display = 'H',
    .name = "Horse",
    .exam_text = "This is a horse. How did it get here?",
    .max_health = 30,
    .strength = 8,
    .takeTurn = &defaultTakeTurnCallback,
    .turn_tokens = 1,
    .turn_tokens_reset_amount = 1
  };
  creature_data[CREATURE_TYPE_GOLEM] = (struct creature_t){
    .creature_id = CREATURE_TYPE_GOLEM,
    .display = 'O',
    .name = "Golem",
    .exam_text = "This is a golem.",
    .max_health = 30,
    .strength = 3,
    .takeTurn = &defaultTakeTurnCallback,
    .turn_tokens = 1,
    .turn_tokens_reset_amount = 1
  };

  creature_data[CREATURE_TYPE_EYE] = (struct creature_t){
    .creature_id = CREATURE_TYPE_EYE,
    .display = 'e',
    .name = "Floating Eye",
    .exam_text = "This is an eyball floating over the floor.",
    .is_flying = true,
    .max_health = 120,
    .strength = 1,
    .takeTurn = &defaultTakeTurnCallback,
    .turn_tokens = 1,
    .turn_tokens_reset_amount = 1
  };
  creature_data[CREATURE_TYPE_DEMON] =  (struct creature_t){
    .creature_id = CREATURE_TYPE_DEMON,
    .display = 'E',
    .name = "Demon",
    .exam_text = "This is a demon.",
    .is_flying = true,
    .max_health = 200,
    .strength = 18,
    .takeTurn = &defaultTakeTurnCallback,
    .turn_tokens = 1,
    .turn_tokens_reset_amount = 1
  };
  creature_data[CREATURE_TYPE_SENTINEL] = (struct creature_t){

    .creature_id = CREATURE_TYPE_SENTINEL,
    .display = 'T',
    .name = "Sentinel",
    .exam_text = "This is a sentinel.",
    .max_health = 100,
    .strength = 10,
    .takeTurn = &defaultTakeTurnCallback,
    .turn_tokens = 1,
    .turn_tokens_reset_amount = 1
  };
  creature_data[CREATURE_TYPE_GRIFFON] = (struct creature_t){

    .creature_id = CREATURE_TYPE_GRIFFON,
    .display = 'G',
    .name = "Griffon",
    .exam_text = "This is a griffon.",
    .is_flying = true,
    .max_health = 80,
    .strength = 5,
    .takeTurn = &defaultTakeTurnCallback,
    .turn_tokens = 1,
    .turn_tokens_reset_amount = 1
  };
  creature_data[CREATURE_TYPE_FOG] = (struct creature_t){
    .creature_id = CREATURE_TYPE_FOG,
    .display = (((int)' ') | COLOR_PAIR(CP_BLACK_WHITE)),
    .name = "Fog",
    .exam_text = "It is fog. Something about it seems strange.",
    .is_flying = true,
    .max_health = 10,
    .strength = 1,
    .takeTurn = &defaultTakeTurnCallback,
    .turn_tokens = 1,
    .turn_tokens_reset_amount = 1
  };
  creature_data[CREATURE_TYPE_PLANT] = (struct creature_t){
    .creature_id = CREATURE_TYPE_PLANT,
    .display = 'f',
    .is_immobile = true,
    .name = "Fungus",
    .exam_text = "This is a magic fungus with legs.",
    .max_health = 8,
    .strength = 3,
    .perception = 1,
    .luck = 1,
    .takeTurn = &defaultTakeTurnCallback,
    .turn_tokens = 1,
    .turn_tokens_reset_amount = 1
  };
}

clist_t *clist_new(creature_t *creature){
  clist_t *to_return = Calloc(1, sizeof(struct creaturelist_t));
  to_return->creature = creature;
  
  return to_return;
}

/* Add next to the end of the given list. If list is NULL, attempts to set the
 * pointer to next.
 */
void clist_add(clist_t *list, clist_t *next){
  if(list == NULL){
    quit("Can't Creature to NULL clist");
  }
  else if(list->next == NULL){
    list->next = next;
  }
  else{
    clist_add(list->next, next);
  }
}

/* Removes the indexth creature from a given clist. If index is negative, it is
 * treated as zero. Returns the removed creature.
 */
creature_t *clist_remove_by_index(clist_t *list, int index){
  if(list == NULL){
    return NULL;
  }
  creature_t *removed = NULL;
  //If it's this element
  if(index <= 0){
    removed = list->creature;
    //If it's the only element left
    if(list->next == NULL){
      list->creature = NULL;
      free(list);
      list = NULL;
    }
    //Otherwise, make its data the next node's
    else{
      clist_t *temp = list->next;
      list->creature = temp->creature;
      list->next = list->next->next;
      free(temp);
    }
  }
  //If it's the next element
  else if(index == 1 && list->next != NULL){
    clist_t *nextnext = list->next->next;
    removed = clist_remove_by_index(list->next, 0);
    list->next = nextnext;
  }
  else{
    removed = clist_remove_by_index(list->next, index-1);
  }
  
  return removed;
}

/* Removes the given creature from a given clist. Returns the removed 
 * creature. If the given creature could not be found, returns NULL.
 */
creature_t *clist_remove_by_creature(clist_t *list, creature_t *creature){
  if(list == NULL){
    return NULL;
  }

  creature_t *removed = NULL;
  
  //If we've found the node to remove
  if(creatures_equal(list->creature, creature)){
    removed = clist_remove_by_index(list, 0);
  }
  //If it's the next one and the next one's the last one
  else if(list->next !=NULL && creatures_equal(list->next->creature, creature)
	  && list->next->next == NULL){
    removed = clist_remove_by_index(list->next, 0);
    list->next = NULL;
  }
  else{
    removed = clist_remove_by_creature(list->next, creature);
  }

  return removed;
}

creature_t *clist_get_creature(clist_t *list){
  return list->creature;
}

clist_t *clist_next(clist_t *list){
  return list->next;
}

creature_t *creature_create_from_data(int index){
  //Error checking
  if(index < 0 || index > CREATURE_TYPE_MAX){
    quit("Error: Cannot create creature with unknown id");}

  //Actually create the creature here
  struct creature_t *to_return = 
    (struct creature_t*)Calloc(1, sizeof(struct creature_t));
  memcpy(to_return, &creature_data[index], sizeof(struct creature_t));

  //Return the creature
  return to_return;
}

/* Creates a creature with the given id, sets up its stats, and places it 
 * randomly on the map.
 */
creature_t *creature_spawn(int creature_id, struct map_t *map){
  if(map == NULL){
    quit("Error: Cannot spawn creature on NULL Map");}
  if(creature_id < 0 || creature_id > CREATURE_TYPE_MAX){
    quit("Error: Cannot spawn creature with unknown id");}

  creature_t *c = creature_create_from_data(creature_id);
  set_level(c, 1);
  set_health(c, get_max_health(c));
  creature_place_randomly_on_map(c, map);
  map_add_creature(map, c);
  c->inventory = inventory_new();

  switch(c->creature_id){
  case CREATURE_TYPE_AVIAN :
    c->body = gen_owl(false);
    break;
  case CREATURE_TYPE_FELINE :
    c->body = gen_cat(false);
    break;
  case CREATURE_TYPE_INSECT :
    c->body = gen_hornet(false);
    break;
  case CREATURE_TYPE_PLANT :
    c->body = gen_fungus(false);
    break;
  case CREATURE_TYPE_RODENT :
    c->body = gen_rat(false);
    break;
  default:
    c->body = NULL;
  }
  
  return c;
}

bool creatures_equal(creature_t *first, creature_t *second){
  if(first == NULL || second == NULL){
    return false;
  }
  if(first == second){
    return true;
  }

  return first->x == second->x && first->y == second->y
    && !strcmp(first->name, second->name);
}

/* Returns whether the creature is alseep or not. If the creature is NULL,
 * returns false.
 */
bool creature_is_asleep(creature_t *c){
  if(c == NULL){
    return false;
  }
  return c->is_asleep;
}

/* Returns whether the creature is blind or not. If the creature is NULL,
 * returns false.
 */
bool creature_is_blind(creature_t *c){
  if(c == NULL){
    return false;
  }
  return c->is_blind;
}

/* Returns whether the creature is flying or not. If the creature is NULL,
 * returns false.
 */
bool creature_is_flying(creature_t *c){
  if(c == NULL){
    return false;
  }
  return c->is_flying;
}

/* Returns whether the creature is capable of moving or not. If the creature
 * is NULL, return false.
 */
bool creature_is_immobile(creature_t *c){
  if(c == NULL || c->body == NULL){
    return false;
  }
  return c->is_immobile;
}

bool creature_is_out_of_turns(creature_t *c){
  return c == NULL || c->turn_tokens <= 0;
}

/* Returns whether the creature is telepathic of moving or not. If the creature
 * is NULL, return false.
 */
bool creature_is_telepathic(creature_t *c){
  if(c == NULL || c->body == NULL){
    return false;
  }
  return c->is_telepathic;
}

/* This method returns true if the given target creature can be seen by the seer
 * creature and false otherwise. Assumes that both creatures are on the same map
 */
bool creature_is_visible(creature_t *target, creature_t *seer){
  if(target == NULL || seer == NULL){
    quit("ERROR: Cannot get visibility of NULL creature.");
  }

  //Check if the target is in the seer's visible range
  if(!in_range(target, seer)){
    return false;}

  //If the seer is the player, do a wall-check. Return true otherwise.
  return map_tile_is_visible(cur_map, target->x, target->y, seer);
}

body_part_t *creature_get_body(creature_t *c){
  return c == NULL ? NULL : c->body;
}

int creature_get_class(creature_t *c){
  if(c == NULL){quit("Error: Cannot get class of NULL Creature.");}
  return c->class;
}

/* Sets the given x and y integers to the x and y coordinate pair that the
 * given creature is at. Returns -1, -1 if the creature is NULL.
 */
void creature_get_coord(creature_t *c, int *x, int *y){
  if(c == NULL){
    *x = -1;
    *y = -1;
  }
  else{
    *x = c->x;
    *y = c->y;
  }
}

/* If there is a creature at a given position in a given list of creatures,
 * return that creature. Otherwise, return NULL.
 */
struct creature_t *get_creature_at_position(int x, int y, clist_t *l){
  for(clist_t *cur = l; cur != NULL; cur = clist_next(cur)){
    creature_t *creature = clist_get_creature(cur);
    
    if(creature != NULL && creature->x == x && creature->y == y){
      return creature;
    }
  }
  return NULL;
}

/* Adds a given item to a given creature's inventory. Does nothing if the given
 * creature is NULL.
 */
bool creature_add_item_to_inventory(creature_t *c, item_t *item){
  int weight = item_weight(item);
  if(weight > (creature_get_max_carry_weight(c) - c->cur_carry_weight)){
    if(creatures_equal(c, player)){
      msg_add("You are currently carrying too much to lift this.");
    }
    return false;
  }
  
  /*Check if the inventory is currently empty. If so, we'll be creating a
   * temporary first item that we'll delete when done.
   */
  bool empty = creature_inv_empty(c);
  if(empty){
    inventory_add(c->inventory, NULL);
  }
  
  if(c != NULL && inventory_add(c->inventory, item)){
    c->cur_carry_weight += weight;

    /* If we added one empty item, remove it now. */
    if(empty){
      inventory_purge(c->inventory);
    }
    return true;
  }
  return false;
}

/* This method handles how a creature takes damage.
 */
void damage_creature(creature_t *target, char *source, int dmg, int type){
  if(target == NULL){
    return;
  }

  int choice = -1;
  damage_body_part(&choice, NULL, target, target->body, dmg, type);
  
  target->health -= dmg;
  if(target->health <= 0){
    if(target->kill == NULL){
      defaultKillCallback(target, cur_map);}
    else{
      (target->kill)(target, cur_map);}
  }

  //Damage message
  if(dmg == 0){
    msg_addf("%s is hit, but it does nothing!", target->name);
  }else if(source == NULL){
    msg_addf("%s is hit!", target->name);}
  else{
    msg_addf("%s is hit by %s!", target->name, source);}

  //If the target is the player, stop quickmove
  if(target == player){
    qckmv = false;}
}

void creature_display_inventory(creature_t *c){
  if(c != NULL){
    display_inventory(c->inventory);
  }
  else{
    msg_add("No items.");
  }
}

/* Attempts to examine the given creature. If the creature could not be
 * examined for some reason, gives the message "Unknown".
 */
void creature_examine(creature_t *c){
  msg_add(c == NULL || c->exam_text == NULL ? "Unknown" : c->exam_text);
}

/* Attempts to free the given creature. If the creature is NULL, does nothing.
 * Returns the creature's body.
 */
body_part_t *creature_free(creature_t *c){
  body_part_t *to_return;
  if(c != NULL){
    to_return = c->body;
    intlist_free(c->breathables);
    intlist_free(c->consumables);
    intlist_free(c->intrinsics);
    intlist_free(c->resistances);
    free(c);
  }
  return to_return;
}

/* Returns whether or not the given creature has any items or not in its
 * inventory. Does not count equipped items. If the creature is NULL, returns
 * true.
 */
bool creature_inv_empty(creature_t *c){
  return c == NULL || inventory_empty(c->inventory);
}

/* Kills the given creature. Does nothing if the creature is NULL.
 */
void creature_kill(creature_t *c, map_t *map){
  if(c != NULL){
    if(c->kill == NULL){
      defaultKillCallback(c, map);
    }
    else{
      c->kill(c, map);
    }
  }
}

/* Lets the given creature pathfind for a turn. Does nothing if the creature
 * is NULL.
 */
void creature_pathfind(creature_t *c, map_t *map){
  if(c != NULL){
    if(c->pathfind == NULL){
      defaultPathfindCallback(c, map);
    }
    else{
      c->pathfind(c, map);
    }
  }
}

void creature_place_randomly_helper(creature_t *c, map_t *map, bool passable){
  int w = map_get_width(map),
    h = map_get_height(map),
    x = rand() % w,
    y = rand() % h;
  
  //Keep searching for a tile that the creature can pass through
  while(tile_data[map_get_tile(map, x, y)].passable != passable){
    x = rand()%w;
    y = rand()%h;
  }
  c->x = x; 
  c->y = y;
}

void creature_place_at_coord(creature_t *c, map_t *map, int x, int y){
  if(c != NULL){
    c->x = x;
    c->y = y;
  }
}

/* This method places the given creature on the given map
 * randomly if it was placed for the first time,
 * at the stairs otherwise.
 */
void creature_place_randomly_on_map(creature_t *creature, map_t *map){
  creature_place_randomly_helper(creature, map, true);
}

void creature_place_randomly_in_walls(creature_t *creature, map_t *map){
  creature_place_randomly_helper(creature, map, false);
}

/* Records the direction that the creature's last known position was in, using
 * the numpad directional system. 
 * move_x is how much the creature moved to the right. 
 * move_y is how much the creature moved downward.
 */
void creature_record_movement(creature_t *c, int move_x, int move_y){
  if(move_y < 0){
    c->last_position = 2;
  }
  else if(move_y == 0){
    c->last_position = 5;
  }
  else if(move_y > 0){
    c->last_position = 8;
  }
  
  if(move_x < 0){
    move_x = -1;
  }
  else if(move_x > 0){
    move_x = 1;
  }
  
  c->last_position -= move_x;
}

/* This method will return a value that determines how far a given creature is
   currently able to see. This value is determined by their perception,
   intelligence, luck, hunger, and level.
*/
int creature_see_distance(struct creature_t* creature){
  if(creature == NULL){
    quit("Error: Cannot get see-distance of NULL creature");}
  
  // Now we can calculate the correct value
  int to_return = (!player->is_blind
		   * !player->is_asleep
		   * (get_intelligence(creature)
		      + get_luck(creature)
		      + (get_level(creature) / 2))
		   * get_perception(creature)
		   / ((creature->max_hunger - creature->hunger) / 100 + 1));
  return to_return;
}

bool creature_set_name(creature_t* c, char* n){
  if(c == NULL || n == NULL){
    return false;
  }
  else if(c->name == NULL){
    c->name = Calloc(MAX_NAME_LEN, sizeof(char));
  }

  strcpy(c->name, n);

  return true;
}

void creature_take_break(creature_t *c){
  c->turn_tokens +=
    c->turn_tokens_reset_amount <= 0 ? 1 : c->turn_tokens_reset_amount;
}

/* Lets the given creature take its turn. Does nothing if the creature is NULL
 * or if the creature is exhausted.
 */
void creature_take_turn(creature_t *c, map_t *map){
  if(c != NULL && c->turn_tokens > 0){
    if(c->takeTurn == NULL){
      defaultTakeTurnCallback(c, map);
    }
    else{
      c->takeTurn(c, map);
    }

    c->turn_tokens--;
  }
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
  tile_t move_tile = tile_data[map_get_tile(cur_map, x, y)];
  //Quick check to see if the tile allows passing
  if(!move_tile.passable){return false;}

  //Get all the tiles surrounding the one we're looking at
  tile_t u = tile_data[map_get_tile(cur_map, x, y-1)];
  tile_t d = tile_data[map_get_tile(cur_map, x, y+1)];
  tile_t r = tile_data[map_get_tile(cur_map, x+1, y)];
  tile_t l = tile_data[map_get_tile(cur_map, x-1, y)];

  // Also cannot pass through closed door.
  if((cmd==cmd_data[CMD_UP_LEFT] && ((map_tile_is_door(d.id)
				      && !d.passable
				      && !r.passable)
				     ||(map_tile_is_door(r.id)
					&& !r.passable
					&& !d.passable)))
     ||(cmd==cmd_data[CMD_UP_RIGHT]&&((map_tile_is_door(d.id)
				       && !d.passable
				       && !l.passable)
				      ||(map_tile_is_door(l.id)
					 && !l.passable
					 && !d.passable)))
     ||(cmd==cmd_data[CMD_DOWN_LEFT]&&((map_tile_is_door(u.id)
					&& !u.passable
					&& !r.passable)
				       ||(map_tile_is_door(r.id)
					  && !r.passable
					  && !u.passable)))
     ||(cmd==cmd_data[CMD_DOWN_RIGHT]&&((map_tile_is_door(u.id)
					 && !u.passable
					 && !l.passable)
					||(map_tile_is_door(l.id)
					   && !l.passable
					   && !u.passable))))
    {
      return false;
    }
  return true;

  /* For the creature to be able to pass, if they're trying to pass through a
   * corner, they cannot be caryying too much.
   */
  if(creature->cur_carry_weight > PASS_WEIGHT 
     && ((cmd==KEY_HOME && !d.passable && !r.passable) 
	 || (cmd==KEY_PPAGE && !d.passable && !l.passable) 
	 || (cmd==KEY_END && !u.passable && !r.passable) 
	 || (cmd==KEY_NPAGE && !u.passable && !l.passable))){
    //If we were trying to move the player, print out a quick alert
    if(creature == player){
      msg_add("You are too heavy to pass through.");}
    return false;
  }
}

/* This method calculates how much damage a creature will do. It takes into
 * account its weapon, its health, and its stranth, etc.
 */
int creature_get_damage(struct creature_t* creature){
  if(creature == NULL){
    quit("ERROR: Cannot get damage of NULL creature.");
  }
  double to_return = 0;
  item_t *weapon = get_weapon(creature->body);
  if(weapon != NULL){
    /* Base weapon damage is a function of strength, health, and weapon damage
     */
    to_return += weapon->damage * 
      (creature->strength / 2) *
      (((double)creature->health
	/ (double)get_max_health(creature) < .5) ? 1 : .8);
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

/* Returns the symbol associated with the given creature. If the creature does
 * not have one, or the creature is NULL, returns '?'.
 */
int creature_get_display(creature_t *c){
  return c == NULL || c->display == 0 ? '?' : c->display;
}

/* Returns the direction that the creature was last in, using the numpad
 * directional system. For example, 8 is up, 1 is down to the left, etc.
 * Returns -1 if completely unknown.
 */
int creature_get_last_position(creature_t *c){
  return c == NULL ? -1 : c->last_position;
}

/* Returns an integer representing how much the given creature can carry.
 * If the creature is NULL, returns -1.
 */
int creature_get_max_carry_weight(creature_t *c){
  if(c == NULL){
    return -1;
  }
  return c->strength > 0 ? c->strength * 10 : 10;
}

/* Returns the name associated with the given creature. If the creature does not
 * have one, or the creature is NULL, returns "Unknown".
 */
char *creature_get_name(creature_t *c){
  return c == NULL || c->name == NULL ? "Unknown" : c->name;
}

int creature_get_skill_with_weapon(creature_t *c){
  return get_dexterity(c);
}

/* Returns the given creature's type. If the creature is NULL, returns 
 * CREATURE_TYPE_UNKNOWN.
 */
int creature_get_type(creature_t *c){
  return c == NULL ? CREATURE_TYPE_UNKNOWN : c->creature_id;
}

/* Sets the given creature's display to the given display value.
 */
void creature_set_display(creature_t *c, int disp){
  if(c != NULL){
    c->display = disp;
  }
}

void set_level(struct creature_t* c, int l){
  if(c != NULL){
    c->level = l;}
}

void set_exam_text(struct creature_t *c, char *e){
  if(c != NULL){
    c->exam_text = e;}
}

void set_class(struct creature_t *c, int class){
  if(c != NULL){
    c->class = class;}
}

void set_blindness(struct creature_t* c, bool b){
  if(c != NULL){
    c->is_blind = b;}
}

void set_asleep(struct creature_t* c, bool b){
  if(c != NULL){
    c->is_asleep = b;}
}

void set_immobile(struct creature_t *c, bool b){
  if(c != NULL){
    c->is_immobile = b;}
}

void set_telepathic(struct creature_t *c, bool b){
  if(c != NULL){
    c->is_telepathic = b;}
}

void set_strength(struct creature_t* c, int s){
  if(c != NULL){
    c->strength = s;}
}

void set_perception(struct creature_t* c, int p){
  if(c != NULL){
    c->perception = p;}
}

void set_endurance(struct creature_t* c, int e){
  if(c != NULL){
    c->endurance = e;}
}

void set_charisma(struct creature_t* c, int ch){
  if(c != NULL){
    c->charisma = ch;}
}

void set_intelligence(struct creature_t* c, int i){
  if(c != NULL){
    c->intelligence = i;}
}

void set_agility(struct creature_t* c, int a){
  if(c != NULL){
    c->agility = a;}
}

void set_luck(struct creature_t* c, int l){
  if(c != NULL){
    c->luck = l;}
}

void set_dexterity(struct creature_t *c, int d){
  if(c != NULL){
    c->dexterity = d;
  }
}

void set_gold(struct creature_t* c, int g){
  if(c != NULL){
    c->gold = g;}
}

 
void set_health(struct creature_t* c, int h){
  if(c != NULL){
    c->health = h;}
}

void set_max_health(struct creature_t *c, int mh){
  if(c == NULL){
    quit("Error: Cannot set values of NULL Creature.");}
  c->max_health = mh;
}

void set_max_hunger(struct creature_t* c, int m){
  if(c != NULL){
    c->max_hunger = m;}
}

void set_hunger(struct creature_t* c, int h){
  if(c != NULL){
    c->hunger = h;}
}

int get_health(struct creature_t* c){
  if(c == NULL || c->body == NULL){
    return 0;}
  return body_part_health(c->body);
}

int get_max_health(struct creature_t* c){
  if(c == NULL){
    return 0;}
  return c->max_health;
}

int get_hunger(struct creature_t *c){
  if(c == NULL){
    return 0;}
  return c->hunger;
}

int get_max_hunger(struct creature_t *c){
  if(c == NULL){
    return 0;}
  return c->max_hunger;
}

bool is_blind(struct creature_t *c){
  if(c == NULL){quit("Cannot get status of NULL Creature");}
  return c->is_blind;
}

bool is_asleep(struct creature_t *c){
  if(c == NULL){quit("Cannot get status of NULL Creature");}
  return c->is_asleep;
}

bool is_telepathic(struct creature_t *c){
  if(c == NULL){quit("Cannot get status of NULL Creature");}
  return c->is_telepathic;
}

int get_level(struct creature_t* c){
  if(c == NULL){quit("Cannot get status of NULL Creature");}
  return c-> level;
}

int get_strength(struct creature_t* c){
  if(c == NULL){quit("Error: Cannot get values of NULL Creature.");}
  return c->strength + class_str_bonus(c->class);
}

int get_perception(struct creature_t* c){
  if(c == NULL){quit("Error: Cannot get values of NULL Creature.");}
  return c->perception + class_per_bonus(c->class);
}

int get_endurance(struct creature_t* c){
  if(c == NULL){quit("Error: Cannot get values of NULL Creature.");}
  return c->endurance + class_end_bonus(c->class);
}

int get_charisma(struct creature_t* c){
  if(c == NULL){quit("Error: Cannot get values of NULL Creature.");}
  return c->charisma + class_cha_bonus(c->class);
}

int get_intelligence(struct creature_t* c){
  if(c == NULL){quit("Error: Cannot get values of NULL Creature.");}
  return c->intelligence + class_int_bonus(c->class);
}

int get_agility(struct creature_t* c){
  if(c == NULL){quit("Error: Cannot get values of NULL Creature.");}
  return c->agility + class_agi_bonus(c->class);
}

int get_luck(struct creature_t* c){
  if(c == NULL){quit("Error: Cannot get values of NULL Creature.");}
  return c->luck + class_luc_bonus(c->class);
}

int get_dexterity(struct creature_t *c){
  if(c == NULL){return 0;}
  return c->dexterity + class_dex_bonus(c->class);
}

int get_gold(struct creature_t* c){
  if(c == NULL){quit("Error: Cannot get values of NULL Creature.");}
  return c->gold;
}


void add_health(struct creature_t* c, int h){
  if(c != NULL){
    c->health+=h;}
}

void add_breathable(struct creature_t* creature, int type){
  if(creature != NULL){
    intlist_add(creature->breathables, type);
  }
}

void add_consumable(struct creature_t* creature, int type){
  if(creature != NULL){
    intlist_add(creature->consumables, type);
  }
}

void add_intrinsic(struct creature_t* creature, int type){
  if(creature != NULL){
    intlist_add(creature->consumables, type);
  }
}

void add_resistance(struct creature_t* creature, int type){
  if(creature != NULL){
    intlist_add(creature->consumables, type);
  }
}

void remove_breathable(struct creature_t* creature, int type){
  if(creature != NULL){
    intlist_remove(creature->breathables, type);
  }
}

void remove_consumable(struct creature_t* creature, int type){
  if(creature != NULL){
    intlist_remove(creature->consumables, type);
  }
}

void remove_instrinsic(struct creature_t* creature, int type){
  if(creature != NULL){
    intlist_remove(creature->intrinsics, type);
  }
}

void remove_resistance(struct creature_t* creature, int type){
  if(creature != NULL){
    intlist_remove(creature->resistances, type);
  }
}


/*
 * PLAYER INFORMATION BELOW THIS LINE
 */

void playerTakeTurnCallback(struct creature_t* creature,
			    struct map_t* map){
  map_reveal(map, creature_see_distance(player));
  draw_map(map);
  draw_status(map, creature);

  int plr_mv_to_x = player->x,
    plr_mv_to_y = player->y;

  //Use timeout so that we can get the mouse movement
  timeout(1);
  do{
    /* If the player is quickmoving, keep doing that. 
     * Else, look for keyboard input.
     */
    if(qckmv){
      cmd = qckmv_cmd;
    }
    //Handle macro playback
    else if(playing_macro){
      cmd = get_next_cmd();
    }else{
      cmd = getch();
      if(cmd != ERR){
	record_cmd(cmd);
      }
    }

    display_mouse();
    
  }while(cmd == 27
	 || !analyze_cmd(cmd, &plr_mv_to_x, &plr_mv_to_y));
  timeout(-1);
  
  /* Ensure that the player is light enough to pass through corners,
       that they are not behind a closed door.
  */
  if(creature_can_move_to(player, plr_mv_to_x, plr_mv_to_y, cmd)){
    //Now we check to see if they're moving into a creature
    struct creature_t *creature_in_way = NULL;
    for(clist_t *cur = map_get_creatures(cur_map);
	cur != NULL && creature_in_way == NULL;
	cur = clist_next(cur)){
      creature_t *cur_creature = clist_get_creature(cur);
      if(cur_creature->x == plr_mv_to_x && cur_creature->y == plr_mv_to_y
	 && cur_creature != player){
	creature_in_way = cur_creature;}
    }

    //And attacking if so
    if(creature_in_way){
      int temp = 0;
      damage_body_part(&temp, player, creature_in_way,
		       creature_in_way->body,
		       creature_get_damage(player),
		       DMG_BLUNT);
    }
    else{
      //If no creature in way, just move there.
      player->x=plr_mv_to_x;
      player->y=plr_mv_to_y;
    }
    //Maybe increment turn counter
    player->turn_tokens--;
    if(player->turn_tokens == 0){num_turns++;}
  }
  else{
    qckmv = false;
    int tile = map_get_tile(map, plr_mv_to_x, plr_mv_to_y);
    //If the creature couldn't move to that spot, see if it's a door and open it
    if(tile == TILE_DOOR_CLOSE){
      open_tile(map, player->x, player->y, cmd);
      //Maybe increment turn counter
      player->turn_tokens--;
      if(player->turn_tokens == 0){num_turns++;}
    }
    //If it's otherwise not passable, they should choose to do something else
    else if(!tile_data[tile].passable){
      playerTakeTurnCallback(creature, map);
    }
  }

  if(qckmv){
    qckmv = qckmv_continue(cur_map, plr_mv_to_x, plr_mv_to_y, qckmv_cmd);}
}

void player_init(){
  player = Calloc(1, sizeof(struct creature_t));
  player->display = '@' | COLOR_PAIR(CP_YELLOW_BLACK);
  player->exam_text = "This is you!";
  
  //Get player's name
  player->name = (char*)calloc(PLAYER_NAME_SIZE+1, sizeof(char));
  strncpy(player->name, msg_prompt("What is your name? "), PLAYER_NAME_SIZE);
  if(!strcmp(player->name, "")){
    player->name = "Lan";}

  player->class = CLASS_NEWB;
  set_strength(player, 1);
  set_perception(player, 1);
  set_endurance(player, 1);
  set_charisma(player, 1);
  set_intelligence(player, 1);
  set_luck(player, 1);
  set_dexterity(player, 1);
  set_health(player, 10);
  set_max_health(player, get_health(player));
  set_max_hunger(player, 200);
  set_hunger(player, player->max_hunger);
  set_gold(player, 5);
  set_level(player, 1);
  
  add_breathable(player, BREATHE_AIR);
  add_consumable(player, CONSUME_ANIMAL);
  add_consumable(player, CONSUME_INSECT);
  add_consumable(player, CONSUME_FOOD);
  add_consumable(player, CONSUME_DRUG);
  add_consumable(player, CONSUME_WATER);
  add_consumable(player, CONSUME_ALCOHOL);
  add_consumable(player, CONSUME_POTION);
  player->takeTurn = &playerTakeTurnCallback;
  player->kill = &game_over;

  player->turn_tokens = class_stamina(player->class);
  player->turn_tokens_reset_amount = player->turn_tokens;

  player->body = gen_human();
  player->inventory = inventory_new();
  
  cmd = 0;
}
