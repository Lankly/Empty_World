#include "colors.h"
#include "creature.h"
#include "creature_callbacks.h"
#include "items.h"
#include "inventory.h"
#include "map.h"
#include "player.h"
#include "status.h"
#include "tiles.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>

/* Initializes all the possible default creatures. Certain fields will not be
 * initialized, because we can't know what they'll be in advance, such as level,
 * since that will be determined by the floor number and the player's level.
 */
void creature_data_init(){
  creature_data[CREATURE_UNKNOWN] = (struct creature_t){
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_SPAWNER] = (struct creature_t){
    .corpse_type = CORPSE_SPAWNER,
    .creature_id = CREATURE_SPAWNER,
    .display = '%' | COLOR_PAIR(CP_GREEN_BLACK),
    .exam_text = "???",
    .is_immobile = true,
    .is_blind = true,
    .health = 100,
    .takeTurn = &spawnerTakeTurnCallback
  };
  creature_data[CREATURE_HUMAN_WARRIOR] = (struct creature_t){
    .corpse_type = CORPSE_HUMAN,
    .display = '@',
    .name = "Human Warrior",
    .exam_text = "This is a human warrior.",
    .health = 30,
    .strength = 2,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_HUMAN_BOWMAN] = (struct creature_t){
    .corpse_type = CORPSE_HUMAN,
    .display = '@',
    .name = "Human Bowman.",
    .exam_text = "This is a human bowman.",
    .health = 20,
    .strength = 1,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_HUMAN_PIKEMAN] = (struct creature_t){
    .corpse_type = CORPSE_HUMAN,
    .display = '@',
    .name = "Human Pikeman",
    .exam_text = "This is a human pikeman.",
    .health = 30,
    .strength = 1,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_HUMAN_MONK] = (struct creature_t){
    .corpse_type = CORPSE_HUMAN,
    .display = '@',
    .name = "Human Monk",
    .exam_text = "This is a human monk.",
    .health = 25,
    .strength = 1,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_HUMAN_WIZARD] = (struct creature_t){
    .corpse_type = CORPSE_HUMAN,
    .display = '@',
    .name = "Human Wizard",
    .exam_text = "This is a human wizard.",
    .health = 20,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_WERECREATURE] = (struct creature_t){
    .corpse_type = CORPSE_WERE,
    .display = 'W',
    .name = "Werecreature",
    .exam_text = "This is a were-something.",
    .health = 50,
    .strength = 5,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_GOBLIN_WARRIOR] = (struct creature_t){
    .corpse_type = CORPSE_GOBLIN,
    .display = 'G',
    .name = "Goblin Warrior",
    .exam_text = "This is a goblin warrior.",
    .health = 35,
    .strength = 3,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_GOBLIN_BOWMAN] = (struct creature_t){
    .corpse_type = CORPSE_GOBLIN,
    .display = 'G',
    .name = "Goblin Bowman",
    .exam_text = "This is a goblin bowman.",
    .health = 25,
    .strength = 2,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_GOBLIN_PIKEMAN] = (struct creature_t){
    .corpse_type = CORPSE_GOBLIN,
    .display = 'G',
    .name = "Goblin Pikeman",
    .exam_text = "This is a goblin pikeman.",
    .health = 35,
    .strength = 3,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_GOBLIN_WIZARD] = (struct creature_t){
    .corpse_type = CORPSE_GOBLIN,
    .display = 'G',
    .name = "Goblin Wizard",
    .exam_text = "This is a goblin wizard.",
    .health = 25,
    .strength = 1,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_ORC_WARRIOR] = (struct creature_t){
    .corpse_type = CORPSE_ORC,
    .display = 'R',
    .name = "Orc Warrior",
    .exam_text = "This is an orc warrior.",
    .health = 40,
    .strength = 4,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_ORC_BOWMAN] = (struct creature_t){
    .corpse_type = CORPSE_ORC,
    .display = 'R',
    .name = "Orc Bowman",
    .exam_text = "This is an orc bowman.",
    .health = 30,
    .strength = 3,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_ORC_PIKEMAN] = (struct creature_t){
    .corpse_type = CORPSE_ORC,
    .display = 'R',
    .name = "Orc Pikeman",
    .exam_text = "This is an orc pikeman.",
    .health = 40,
    .strength = 4,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_ORC_WIZARD] = (struct creature_t){
    .corpse_type = CORPSE_ORC,
    .display = 'R',
    .name = "Orc Wizard",
    .exam_text = "This is an orc wizard.",
    .health = 30,
    .strength = 1,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_MINDFLAYER] = (struct creature_t){
    .corpse_type = CORPSE_MINDFLAYER,
    .display = 'M',
    .name = "Mindflayer",
    .exam_text = "This is a mindflayer.",
    .can_fly = true,
    .health = 120,
    .strength = 15,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_SKELETON_WARRIOR] = (struct creature_t){
    .corpse_type = CORPSE_SKELETON,
    .display = 'S',
    .name = "Skeleton Warrior",
    .exam_text = "This is a skeleton Warrior.",
    .health = 10,
    .strength = 2,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_SKELETON_BOWMAN] = (struct creature_t){
    .corpse_type = CORPSE_SKELETON,
    .display = 'S',
    .name = "Skeleton Bowman",
    .exam_text = "This is a skeleton bowman.",
    .health = 10,
    .strength = 2,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_SKELETON_PIKEMAN] = (struct creature_t){
    .corpse_type = CORPSE_SKELETON,
    .display = 'S',
    .name = "Skeleton Pikeman",
    .exam_text = "This is a skeleton pikeman.",
    .health = 10,
    .strength = 2,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_PUPPY] = (struct creature_t){
    .corpse_type = CORPSE_CANINE,
    .display = 'd',
    .name = "Puppy",
    .exam_text = "This is a cute little puppy!",
    .health = 12,
    .strength = 1,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_DOG] = (struct creature_t){
    .corpse_type = CORPSE_CANINE,
    .display = 'D',
    .name = "Dog",
    .exam_text = "This is a dog.",
    .health = 30,
    .strength = 4,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_WARDOG] = (struct creature_t){
    .corpse_type = CORPSE_CANINE,
    .display = 'D',
    .name = "Wardog",
    .exam_text = "This is a trained wardog.",
    .health = 100,
    .strength = 10,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_KITTEN] = (struct creature_t){
    .corpse_type = CORPSE_FELINE,
    .display = 'c',
    .name = "Kitten",
    .exam_text = "This is a cute little kitten!",
    .health = 5,
    .strength = 1,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_HOUSECAT] = (struct creature_t){
    .corpse_type = CORPSE_FELINE,
    .display = 'C',
    .name = "Housecat",
    .exam_text = "This is a housecat.",
    .health = 9,
    .strength = 3,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_WARCAT] = (struct creature_t){
    .corpse_type = CORPSE_FELINE,
    .display = 'C',
    .name = "Warcat",
    .exam_text = "This is a trained warcat.",
    .health = 30,
    .strength = 8,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_LION] = (struct creature_t){
    .corpse_type = CORPSE_FELINE,
    .display = 'C',
    .name = "Lion",
    .exam_text = "This is a ferocious lion.",
    .health = 50,
    .strength = 5,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_PUMA] = (struct creature_t){
    .corpse_type = CORPSE_FELINE,
    .display = 'C',
    .name = "Puma",
    .exam_text = "This is a deadly puma.",
    .health = 60,
    .strength = 6,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_WOLF] = (struct creature_t){
    .corpse_type = CORPSE_CANINE,
    .display = 'D',
    .name = "Wolf",
    .exam_text = "This is a wild wolf.",
    .health = 10,
    .strength = 7,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_HORSE] = (struct creature_t){
    .corpse_type = CORPSE_EQUUS,
    .display = 'H',
    .name = "Horse",
    .exam_text = "This is a horse. How did it get in a cave?",
    .health = 30,
    .strength = 8,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_UNICORN] = (struct creature_t){
    .corpse_type = CORPSE_EQUUS,
    .display = 'U',
    .name = "Unicorn",
    .exam_text = "This is a unicorn.",
    .health = 80,
    .strength = 9,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_UNICORN_GOOD] = (struct creature_t){
    .corpse_type = CORPSE_EQUUS,
    .display = 'U',
    .name = "Unicorn",
    .exam_text = "This is a unicorn.",
    .health = 70,
    .strength = 8,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_UNICORN_EVIL] = (struct creature_t){
    .corpse_type = CORPSE_EQUUS,
    .display = 'U',
    .name = "Unicorn",
    .exam_text = "This is a unicorn.",
    .health = 90,
    .strength = 10,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_PAPER_GOLEM] = (struct creature_t){
    .corpse_type = CORPSE_GOLEM,
    .display = 'O',
    .name = "Paper Golem",
    .exam_text = "This is a golem made of paper.",
    .health = 30,
    .strength = 3,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_ROCK_GOLEM] = (struct creature_t){
    .corpse_type = CORPSE_GOLEM,
    .display = 'O',
    .name = "Rock Golem",
    .exam_text = "This is a golem made of rock.",
    .health = 50,
    .strength = 4,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_SILVER_GOLEM] = (struct creature_t){
    .corpse_type = CORPSE_GOLEM,
    .display = 'O',
    .name = "Silver Golem",
    .exam_text = "This is a golem made of silver.",
    .health = 80,
    .strength = 5,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_GOLD_GOLEM] = (struct creature_t){
    .corpse_type = CORPSE_GOLEM,
    .display = 'O',
    .name = "Gold Golem",
    .exam_text = "This is a golem made of gold.",
    .health = 70,
    .strength = 6,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_IRON_GOLEM] = (struct creature_t){
    .corpse_type = CORPSE_GOLEM,
    .display = 'O',
    .name = "Iron Golem",
    .exam_text = "This is a golem made of iron.",
    .health = 100,
    .strength = 7,
    .takeTurn = &defaultTakeTurnCallback
  };
  creature_data[CREATURE_FLOATING_EYE] = (struct creature_t){
    .corpse_type = CORPSE_EYE,
    .display = 'E',
    .name = "Floating Eye",
    .exam_text = "This is an eyball floating over the floor.",
    .can_fly = true,
    .health = 120,
    .strength = 1,
    .takeTurn = &defaultTakeTurnCallback
  };
}

struct creature_t *creature_create_from_data(int index){
  //Error checking
  if(index < 0 || index > CREATURE_MAX){
    quit("Error: Cannot create creature with unknown id");}

  //Actually create the creature here
  struct creature_t *to_return = (struct creature_t*)Calloc(1, sizeof(struct creature_t));
  memcpy(to_return, &creature_data[index], sizeof(struct creature_t));

  //Return the creature
  return to_return;
}

void creature_spawn(int creature_id, struct map_t *map){
  if(map == NULL){
    quit("Error: Cannot spawn creature on NULL Map");}
  if(creature_id < 0 || creature_id > CREATURE_MAX){
    quit("Error: Cannot spawn creature with unknown id");}

  struct creature_t *c = creature_create_from_data(creature_id);
  c->dlevel = map->dlevel;
  c->level = map->dlevel;
  creature_place_on_map(c, map);
  map_add_creature(map, c);
}

/* This method will kill the creature and place its corpse and items
 */
void creature_kill(struct creature_t* creature){
  if(creature==NULL){quit("ERROR: Cannot kill NULL Creature");}
  item_t* corpse = item_create_from_data(creature->corpse_type);
  add_item(cur_map, creature->x, creature->y, corpse, false);

  if(creature->inventory!=NULL){
    while(creature->inventory->first!=NULL){
      item_t* next = creature->inventory->first->item;
      if(next!=NULL && rand()%2){
	add_item(cur_map, creature->x, creature->y, next, false);
      }
      inventory_node_t* to_remove = creature->inventory->first;
      creature->inventory->first = to_remove->next;
      free(to_remove);
    }
    free(creature->inventory);
  }
  msg_addf("%s is slain!", creature->name);
  map_remove_creature(cur_map, creature);

  //Is this game over?
  if(creature == player){
    draw_map(cur_map);
    draw_status(cur_map);
    game_over();
  }
  else{
    free(creature->name);
    free(creature);
  }

}

/* This method handles how a creature takes damage.
 */
void damage_creature(struct creature_t *target, char *source, int dmg){
  if(target == NULL){return;}
  
  target->health -= dmg;
  if(target->health<=0){
    creature_kill(target);}

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
  int to_return = (!player->is_blind
		   * !player->is_asleep
		   * (creature->intelligence
		      + creature->luck
		      + creature->level)
		   * creature->perception 
		   / ((creature->max_hunger - creature->hunger) / 100 + 1));
  return to_return;
}

/* This method returns true if the given target creature can be seen by the seer
 * creature and false otherwise.
 */
bool creature_is_visible(struct creature_t *target, struct creature_t *seer){
  if(target == NULL || seer == NULL){
    quit("ERROR: Cannot get visibility of NULL creature.");
  }

  //If on different floors, return false
  if(target->dlevel != seer->dlevel){
    return false;}

  //Check if the target is in the seer's visible range
  if(!in_range(target, seer)){
    return false;}

  //If the seer is the player, do a wall-check. Return true otherwise.
  return seer == player ? map_tile_is_visible(cur_map, target->x, target->y)
    : true;
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
     && ((cmd==KEY_HOME && !d.passable && !r.passable) 
	 || (cmd==KEY_PPAGE && !d.passable && !l.passable) 
	 || (cmd==KEY_END && !u.passable && !r.passable) 
	 || (cmd==KEY_NPAGE && !u.passable && !l.passable))){
    //If we were trying to move the player, print out a quick alert
    if(creature == player){
      msg_add("You are too heavy to pass through.");}
    return false;
  }
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

}

/* This method calculates how much damage a creature will do. It takes into
 * account its weapon, its health, and its stranth, etc.
 */
int creature_get_damage(struct creature_t* creature){
  if(creature == NULL){
    quit("ERROR: Cannot get damage of NULL creature.");
  }

  //If no inventory, just use stranth
  if(creature->inventory == NULL){
    return creature->strength;}
  
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

void set_blindness(struct creature_t* c, bool b){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  c->is_blind = b;
}

void set_unconscious(struct creature_t* c, bool b){
  if(c == NULL){quit("Error: Cannot set values of NULL Creature.");}
  c->is_asleep = b;
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
  if(c == NULL){quit("Error: Cannot get values of NULL Creature.");}
  return c->name;
}

int get_health(struct creature_t* c){
  if(c == NULL){quit("Error: Cannot get values of NULL Creature.");}
  return c-> health;
}

int get_level(struct creature_t* c){
  if(c == NULL){quit("Error: Cannot get values of NULL Creature.");}
  return c-> level;
}

int get_dlevel(struct creature_t* c){
  if(c == NULL){quit("Error: Cannot get values of NULL Creature.");}
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
