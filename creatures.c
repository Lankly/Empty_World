#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <curses.h>
#include "creatures.h"
#include "bodies.h"
#include "colors.h"
#include "helpers.h"
#include "items.h"
#include "lists.h"
#include "maps.h"
#include "trees.h"

/*********
 * TYPES *
 *********/
BTREE(attribute_tree_t)
LINKED_LIST(inventory_t)

struct creature_t{
  char *name;
  class_t class;
  species_t species;
  int lvl;
  int x, y;

  int health, stamina;

  /**
   * Nutrition has possible values of 0 - 700.
   * Nutrition is gained from eating food. The amount of food that a creature
   * can eat is determined by their body size. A large creature should gain no
   * more than 100 nutrition in a single turn.
   * 1 nutrition should be lost per turn. 2 if the creature is exercising.
   *
   * Nutrition Levels:
   * 0 and below indicates that the creature has died from starvation.
   * 1 - 100 indicates extreme hunger. (Slowed by 2 per turn)
   * 101 - 200 indicates very hungry. (Slowed by 1 per turn)
   * 201 - 400 indicates hungry.
   * 401 - 500 indicates fullness.
   * 501 - 600 indicates satiation. (Slowed by 1 per turn)
   * 601 - 700 indicates hypersatiation. (Slowed by 2 per turn, cannot eat)
   */
  int nutrition;

  /**
   * Abilities are all on a normal curve, as follows:
   * Standard deviation is 15
   * Average is 50
   * There is a 68% chance of any creature falling between 35 and 65
   * There is a 95% chance of any creature falling between 10 and 80
   * There is a 99.7% chance of any creature falling between -5 and 105
   */
  int strength;
  int perception;
  int endurance;
  int charisma;
  int intelligence;
  int agility;
  int luck;
  
  attribute_tree_t *attributes;
  body_t *body;
  creature_t *owner;            /* Used for pets */
  inventory_t *inventory;
  title_t title;
};


/******************************
 * HELPER FUNCTION PROTOTYPES *
 ******************************/
void move_creature(creature_t *c, map_t *m);
dir_t move_creature_toward_target(creature_t *c, map_t *m, dijkstra_t *d);
/**
 * Each of these should return a valid direction for the given creature to move
 * on the given map. They should be called only when the given creature has no
 * target to move to, when they're just wandering.
 */
dir_t move_creature_default(creature_t *c, map_t *m);
dir_t move_creature_along_walls(creature_t *c, map_t *m);
dir_t move_creature_through_walls(creature_t *c, map_t *m);

/**
 * These functions reevaluate certain attributes.
 */
void reevaluate_immobility(creature_t *c);
void reevaluate_flying(creature_t *c);
void reevaluate_hovering(creature_t *c);

/*********************
 * PRIVATE VARIABLES *
 *********************/
/**
 * To be used only for creating the player.
 */
creature_t class_templates[MAX_CLASS];


/************************
 * FUNCTION DEFINITIONS *
 ************************/

void creatures_init(){
  class_templates[CLASS_PRIEST] = (creature_t){
    .charisma = 55,
    .perception = 55
  };
  class_templates[CLASS_SPY] = (creature_t){
    .agility = 55,
    .perception = 55
  };
  class_templates[CLASS_TINKER] = (creature_t){
    .intelligence = 55,
    .perception = 55
  };
  class_templates[CLASS_WARRIOR] = (creature_t){
    .strength = 55,
    .endurance = 55
  };
}

creature_t *new_creature(){
  creature_t *ret = Calloc(1, sizeof(creature_t));

  ret->attributes = Calloc(1, sizeof(attribute_tree_t));
  
  return ret;
}

creature_t *new_creature_by_species(species_t s){
  creature_t *ret = new_creature();

  ret->body = new_body_by_species(s);
  
  return ret;
}

void creature_take_turn(creature_t *c, map_t *m){
  if(c == NULL || m == NULL){
    return;
  }

  //If stamina too low, wait and recover some.
  if(c->stamina < 0){
    c->stamina += c->endurance + 10;
    return;
  }

  //Player has a special turn helper function.
  if(is_player(c)){
    player_take_turn(c, m);
    return;
  }

  move_creature(c, m);
}

void add_attribute(creature_t *c, attribute_t a){
  if(c == NULL){
    return;
  }

  c->attributes = tree_insert(c->attributes, &a, &int_cmp);
}

void add_attributes(creature_t *c, int num_attributes, ...){
  if(num_attributes < 0){
    return;
  }

  va_list args;
  va_start(args, num_attributes);

  for(int i = 0; i < num_attributes; i++){
    attribute_t arg = va_arg(args, attribute_t);

    add_attribute(c, arg);
  }
  
  va_end(args);
}

void remove_attribute(creature_t *c, attribute_t a){
  if(c == NULL){
    return;
  }

  c->attributes = tree_remove(c->attributes, &a, &int_cmp);
}

void remove_attributes(creature_t *c, int num_attributes, ...){
  if(num_attributes < 0){
    return;
  }

  va_list args;
  va_start(args, num_attributes);

  for(int i = 0; i < num_attributes; i++){
    attribute_t arg = va_arg(args, attribute_t);
    remove_attribute(c, arg);
  }
  
  va_end(args);
}

bool has_attribute(creature_t *c, attribute_t a){
  if(c == NULL){
    return false;
  }
  
  return tree_search(c->attributes, &a, int_cmp) != NULL;
}

bool has_attributes(creature_t *c, int num_attributes, ...){
  if(num_attributes < 0){
    return true;
  }

  va_list args;
  va_start(args, num_attributes);
  bool result = true;
  
  for(int i = 0; i < num_attributes; i++){
    attribute_t arg = va_arg(args, attribute_t);

    result = result && has_attribute(c, arg);
  }
  
  va_end(args);

  return result;
}

bool has_extrinsic(creature_t *c, attribute_t a){
  if(c == NULL || c->inventory == NULL){
    return false;
  }

  for(inventory_t *cur = c->inventory; cur != NULL; cur = ll_next(cur)){
    item_t *item = ll_elem(cur);

    if(grants(item, a)){
      return true;
    }
  }
  
  return false;
}

int creature_get_target(creature_t *c, map_t *m){
  //TODO
  return 0;
}

bool creature_teleport(creature_t *c, map_t *m, int x, int y){
  if(c == NULL || m == NULL || x < 0 || y < 0){
    return false;
  }
  //TODO
  //Check if tile is blocked

  return false;
}

void creature_add_nutrition(creature_t *c, int amount){
  if(c == NULL){
    return;
  }

  c->nutrition += amount;
}

void creature_display_stats(creature_t *c){
  //TODO
}

int creature_get_icon(creature_t *c){
  int base = '\0';
  if(c == NULL){
    return base;
  }

  switch(c->species){
  case SPECIES_CANINE:
    base = 'd';
    break;
  case SPECIES_FELINE:
    base = 'c';
    break;
  case SPECIES_RODENT:
    base = 'r';
    break;
  case SPECIES_DWARF:
  case SPECIES_HUMAN:
  case SPECIES_ELF:
    base = '@';
    break;
  default:
    break;
  }

  int color_pair;
  switch(c->title){
  case TITLE_MINOR:
    color_pair = COLOR_PAIR(CP_GREEN_BLACK);
    break;
  case TITLE_MAJOR:
    color_pair = COLOR_PAIR(CP_BLUE_BLACK);
    break;
  case TITLE_MASTER:
    color_pair = COLOR_PAIR(CP_RED_BLACK);
    break;
  default:
    break;
  }

  if(is_player(c)){
    color_pair = COLOR_PAIR(CP_YELLOW_BLACK);
  }
  
  return base | color_pair;
}


/*******************************
 * PLAYER FUNCTION DEFINITIONS *
 *******************************/

creature_t *new_player(class_t c, species_t s){
  creature_t *player = new_creature_by_species(s);

  body_t *temp = player->body;
  memcpy(player, class_template + c, sizeof(creature_t));
  player->body = temp;
  
  return player;
}

bool is_player(creature_t *c){
  if(c == NULL){
    return false;
  }

  return has_attribute(c, ATTR_IS_PLAYER);
}


/*******************************
 * HELPER FUNCTION DEFINITIONS *
 *******************************/

void move_creature(creature_t *c, map_t *m){
  //Determine which direction to move
  dir_t dir;
  if(has_attribute(c, ATTR_MOVES_ALONG_WALLS)){
    dir = move_creature_along_walls(c, m);
  }
  else if(has_attribute(c, ATTR_MOVES_THROUGH_WALLS)){
    dir = move_creature_through_walls(c, m);
  }
  else{
    dir = move_creature_default(c, m);
  }

  //Figure out how the direction translates to a position
  if(dir == DIR_UP || dir == DIR_UL || dir == DIR_UR){
    c->y--;
  }
  else if(dir == DIR_DOWN || dir == DIR_LL || dir == DIR_LR){
    c->y++;
  }
  if(dir == DIR_LEFT || dir == DIR_UL || dir == DIR_LL){
    c->x--;
  }
  else if(dir == DIR_RIGHT || dir == DIR_UR || dir == DIR_LR){
    c->x++;
  }
}

dir_t move_creature_towards_target(creature_t *c, map_t *m){
  //TODO
}

dir_t move_creature_default(creature_t *c, map_t *m){
  //TODO
}

dir_t move_creature_along_walls(creature_t *c, map_t *m){
  //TODO
}

dir_t move_creature_through_walls(creature_t *c, map_t *m){
  //TODO
}

void reevaluate_immobility(creature_t *c){
  remove_attribute(c, ATTR_IS_IMMOBILE);
  bool cant_move = false;

  cant_move = cant_move || has_attribute(c, ATTR_IS_STUNNED);
  cant_move = cant_move || has_attribute(c, ATTR_IS_FROZEN);
  cant_move = cant_move || has_attribute(c, ATTR_IS_PINNED);

  //Can move if has at least two wings
  bool mw = has_attribute(c, ATTR_HAS_MANY_WINGS);
  bool lrw = has_attributes(c, 2, ATTR_HAS_LEFT_WING, ATTR_HAS_RIGHT_WING);
  
  //Can move if has at least two feet
  bool ml = has_attribute(c, ATTR_HAS_MANY_LEGS);
  bool lrf = has_attributes(c, 2, ATTR_HAS_LEFT_FOOT, ATTR_HAS_RIGHT_FOOT);

  //Can move if has at least two hands
  bool ma = has_attribute(c, ATTR_HAS_MANY_ARMS);
  bool lrh = has_attributes(c, 2, ATTR_HAS_LEFT_HAND, ATTR_HAS_RIGHT_HAND);

  //Check wings, hands, and feet
  if(!mw && !lrw && !ml && !lrf && !ma && !lrh){
    cant_move = true;
  }
  
  if(cant_move){
    add_attribute(c, ATTR_IS_IMMOBILE);
  }
}

void reevaluate_flying(creature_t *c){
  remove_attribute(c, ATTR_IS_FLYING);
  bool can_fly = false;

  //Can fly if has at least two wings
  bool mw = has_attribute(c, ATTR_HAS_MANY_WINGS);
  bool lrw = has_attributes(c, 2, ATTR_HAS_LEFT_WING, ATTR_HAS_RIGHT_WING);

  //Check wings, hands, and feet
  if(mw || lrw){
    can_fly = true;
  }
  
  if(can_fly){
    add_attribute(c, ATTR_IS_IMMOBILE);
  }
}
