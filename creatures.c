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

/* Attribute-related functions */

void add_attribute(creature_t *c, attribute_t a){
  if(c == NULL){
    return;
  }

  c->attributes = tree_insert(c->attributes, &a, int_cmp);
}

void add_attributes(creature_t *c, int num_attributes, ...){
  if(num_attributes < 1){
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

/* Map-related functions */

int creature_get_coord(creature_t *c, map_t *m){
  if(c == NULL || m == NULL){
    return -1;
  }

  return get_coord_in_arr(c->x, c->y, map_get_width(m));
}

dijkstra_t *creature_get_targets(creature_t *c, map_t *m){
  if(c == NULL || m == NULL){
    return NULL;
  }
  
  clist_t *creatures = map_get_creatures(m);
  ilist_t *items = map_get_items(m);

  for(clist_t *cur = creatures; cur != NULL; cur = ll_next(cur)){
    
  }
  
  for(ilist_t *cur = items; cur != NULL; cur = ll_next(cur)){
    
  }
  
  return 0;
}

/* Nutrition-related functions */

void creature_add_nutrition(creature_t *c, int amount){
  if(c == NULL){
    return;
  }

  c->nutrition += amount;
}

/* Stat-related functions */

void creature_display_stats(creature_t *c){
  //TODO
}

/* Turn-related functions */

bool creature_is_out_of_turns(creature_t *c){
  return c == NULL || c->stamina < 1;
}

void creature_take_break(creature_t *c){
  if(c == NULL){
    return;
  }

  c->stamina = c->endurance * c->lvl;
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

/* Other functions */

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

creature_t *new_player(){
  species_t s = SPECIES_HUMAN;
  class_t c = CLASS_WARRIOR;
  
  creature_t *player = new_creature_by_species(s);

  body_t *temp = player->body;
  memcpy(player, class_templates + c, sizeof(creature_t));
  player->body = temp;
  
  return player;
}

bool is_player(creature_t *c){
  if(c == NULL){
    return false;
  }

  return has_attribute(c, ATTR_IS_PLAYER);
}

void player_take_turn(creature_t *p, map_t *m){
  if(p == NULL || !has_attribute(p, ATTR_IS_PLAYER) || m == NULL){
    return;
  }

  //TODO
  getch();
}

/*******************************
 * HELPER FUNCTION DEFINITIONS *
 *******************************/

void move_creature(creature_t *c, map_t *m){
  dijkstra_t *priorities = dijkstra_new(m);
  if(has_attribute(c, ATTR_MOVES_ALONG_WALLS)){
    int coord = map_get_nearest_wall(m, c->x, c->y);
    dijkstra_add_value(priorities, 100,
                       coord % map_get_width(m), coord / map_get_height(m));
  }

  dijkstra_calculate(priorities);
  
  int *weights = dijkstra_map(priorities);
  if(weights != NULL){
    //TODO: Determine which space is the best to move to.
  }
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
