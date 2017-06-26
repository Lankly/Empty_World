#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <curses.h>
#include <limits.h>
#include <time.h>
#include "creatures.h"
#include "bodies.h"
#include "colors.h"
#include "display.h"
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
  int display, display_color, display_color_alt;

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

  dir_t facing;
  
  attribute_tree_t *attributes;
  body_t *body;
  creature_t *owner;            /* Used for pets */
  inventory_t *inventory;
  title_t title;
};

typedef enum {
  CMD_WAIT,
  /* Directions */
  CMD_UP,
  CMD_DOWN,
  CMD_LEFT,
  CMD_RIGHT,
  CMD_UP_LEFT,
  CMD_UP_RIGHT,
  CMD_DOWN_LEFT,
  CMD_DOWN_RIGHT,
  /* Display */
  CMD_CYCLE_DISPLAY_MODE,
  /* Other */
  CMD_EXTENDED,
  CMD_MAX
} commands_t;

/* KEEP THIS ALPHABETICAL BY MAPPING */
typedef enum {
  CMD_EXT_USE_HJKL,
  CMD_EXT_USE_KEYPAD,
  CMD_EXT_USE_NUMPAD,
  CMD_EXT_MAX
} commands_extended_t;

/******************************
 * HELPER FUNCTION PROTOTYPES *
 ******************************/
void move_creature(creature_t *c, map_t *m);

/**
 * Moves a given creature one tile in the given direction. If it fails, returns
 * false.
 * @param c A valid creature.
 * @param m A valid map.
 * @param dir A valid direction.
 * @returns true if successful, false otherwise.
 */
bool creature_move_dir(creature_t *c, map_t *m, dir_t dir);

/**
 * Map all the commands to their default keys.
 */
void cmd_map_init();

/**
 * Will read in an extended command from the player and execute it if possible.
 * @param p The player.
 * @param m The map the player is on.
 * @returns True if the command executed successfully and that command should
 *          use up an entire turn.
 */
bool execute_extended(creature_t *p, map_t *m);

/**
 * These functions reevaluate certain attributes.
 */
void reevaluate_immobility(creature_t *c);
void reevaluate_flying(creature_t *c);
void reevaluate_hovering(creature_t *c);

/**
 * Either remaps every cmd passed to it to the given key or does nothing. The
 * first command will be mapped to the first key, and so on.
 * @param cmds The commands to remap.
 * @param keys The keys that each command will map to.
 * @param count The total number of commands and keys.
 * @returns true if every command was able to be remapped.
 */
bool remap_keys(commands_t *cmds, int *keys, int count);

/*********************
 * PRIVATE VARIABLES *
 *********************/
/**
 * To be used only for creating the player.
 */
creature_t class_templates[MAX_CLASS];

/**
 * To be used to figure out which command the user has just given.
 */
int cmd_map[CMD_MAX];

const char *ext_cmd_map[CMD_EXT_MAX];

/************************
 * FUNCTION DEFINITIONS *
 ************************/

void creatures_init(){
  cmd_map_init();
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

  switch(s){
  case SPECIES_HUMAN:
    ret->display = '@';
    break;
  default:
    ret->display = '?';
  }
  //Display color should be set elsewhere.
  
  return ret;
}

/* Attribute-related functions */

void add_attribute(creature_t *c, attribute_t a){
  if(c == NULL){
    return;
  }

  attribute_t *heap_reference = Calloc(1, sizeof(attribute_t));
  *heap_reference = a;
  
  c->attributes = tree_insert(c->attributes, heap_reference, int_cmp);
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

  //Check if it's in the tree first
  void *in_tree = tree_search(c->attributes, &a, &int_cmp);
  if(in_tree != NULL){
    c->attributes = tree_remove(c->attributes, &a, &int_cmp);
    free(in_tree);
  }
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

  attribute_t *tmp = Calloc(1, sizeof(attribute_t));
  *tmp = a;
  
  bool to_return = tree_search(c->attributes, tmp, int_cmp) != NULL;
  free(tmp);

  return to_return;
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

/* Data-related functions */

int creature_get_coord(creature_t *c, map_t *m){
  if(c == NULL || m == NULL){
    return -1;
  }

  return get_coord_in_arr(c->x, c->y, map_get_width(m));
}

void creature_set_coord(creature_t *c, int x, int y){
  if(c == NULL || x < 0 || y < 0){
    return;
  }

  c->x = x;
  c->y = y;
}

int creature_get_display(creature_t *c){
  if(c == NULL){
    return ' ';
  }

  int display_color;
  if(compatibility_mode_on()){
    display_color = c->display_color_alt;
  }
  else{
    display_color = c->display_color;
  }
  
  return c->display | COLOR_PAIR(display_color);
}

/* Map-related functions */

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

  c->stamina += c->endurance * c->lvl;

  c->stamina = MIN(c->stamina, c->endurance * c->lvl * 3);
}

void creature_take_turn(creature_t *c, map_t *m){
  if(c == NULL || m == NULL){
    return;
  }

  //If stamina too low, wait and recover some.
  if(c->stamina < 0){
    creature_take_break(c);
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

  //Variables that we need to preserve before we overwrite with class.
  body_t *temp = player->body;
  int disp = player->display;
  
  memcpy(player, class_templates + c, sizeof(creature_t));

  //Restore those variables
  player->body = temp;
  player->display = disp;

  //Set location (middle of screen, which is okay on desert)
  player->x = get_pane_width(PANE_PRIMARY) / 2;
  player->y = get_pane_height(PANE_PRIMARY) / 2;
  
  player->lvl = 1;
  player->display_color = CP_YELLOW_BLACK;
  player->display_color_alt = CP_YELLOW_BLACK;

  add_attribute(player, ATTR_IS_PLAYER);

  write_to_pane(PANE_ALERT, (int[]){ 'H', 'e', 'l', 'l', 'o', '!', 0 }, 0, 0);
  
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
  
  bool cmd_successful = false;
  while(!cmd_successful){
    //Need to do the write once here in addition to below, or it will lag
    write_to_pane(PANE_PRIMARY, map2display(m)
                  , map_get_width(m), map_get_height(m));
    refresh();
    
    int c = ERR, last_second = time(NULL);
    while((c = getch()) == ERR){
      int cur_time = time(NULL);
      if(cur_time != last_second){
        last_second = cur_time;
        write_to_pane(PANE_PRIMARY, map2display(m)
                      , map_get_width(m), map_get_height(m));
        refresh();
      }
    }
    
    if(c == cmd_map[CMD_WAIT]){
      creature_take_break(p);
      cmd_successful = true;
    }
    else if(c == cmd_map[CMD_UP] || c == KEY_UP){ /* Arrow keys are special */
      cmd_successful = creature_move_dir(p, m, DIR_UP);
    }
    else if(c == cmd_map[CMD_DOWN] || c == KEY_DOWN){
      cmd_successful = creature_move_dir(p, m, DIR_DOWN);
    }
    else if(c == cmd_map[CMD_LEFT] || c == KEY_LEFT){
      cmd_successful = creature_move_dir(p, m, DIR_LEFT);
    }
    else if(c == cmd_map[CMD_RIGHT] || c == KEY_RIGHT){
      cmd_successful = creature_move_dir(p, m, DIR_RIGHT);
    }
    else if(c == cmd_map[CMD_UP_LEFT]){
      cmd_successful = creature_move_dir(p, m, DIR_UL);
    }
    else if(c == cmd_map[CMD_UP_RIGHT]){
      cmd_successful = creature_move_dir(p, m, DIR_UR);
    }
    else if(c == cmd_map[CMD_DOWN_LEFT]){
      cmd_successful = creature_move_dir(p, m, DIR_LL);
    }
    else if(c == cmd_map[CMD_DOWN_RIGHT]){
      cmd_successful = creature_move_dir(p, m, DIR_LR);
    }
    else if(c == cmd_map[CMD_CYCLE_DISPLAY_MODE]){
      cycle_display_mode();
    }
    else if(c == cmd_map[CMD_EXTENDED]){
      cmd_successful = execute_extended(p, m);
    }
  }
}

/*******************************
 * HELPER FUNCTION DEFINITIONS *
 *******************************/

void cmd_map_init(){
  cmd_map[CMD_WAIT] = '.';
  cmd_map[CMD_UP] = '8';
  cmd_map[CMD_DOWN] = '2';
  cmd_map[CMD_LEFT] = '4';
  cmd_map[CMD_RIGHT] = '6';
  cmd_map[CMD_UP_LEFT] = '7';
  cmd_map[CMD_UP_RIGHT] = '9';
  cmd_map[CMD_DOWN_LEFT] = '1';
  cmd_map[CMD_DOWN_RIGHT] = '3';
  cmd_map[CMD_EXTENDED] = '#';
  cmd_map[CMD_CYCLE_DISPLAY_MODE] = '|';
  ext_cmd_map[CMD_EXT_USE_HJKL] = "use-hjkl";
  ext_cmd_map[CMD_EXT_USE_KEYPAD] = "use-keypad";
  ext_cmd_map[CMD_EXT_USE_NUMPAD] = "use-numpad";
}

bool execute_extended(creature_t *p, map_t *m){
  if(p == NULL || m == NULL){
    return false;
  }

  //Need extended commands list in heap-form
  char **autocomplete = Calloc(CMD_EXT_MAX, sizeof(char *));
  for(size_t i = 0; i < CMD_EXT_MAX; i++){
    size_t cur_len = strlen(ext_cmd_map[i]);
    autocomplete[i] = Calloc(cur_len + 1, sizeof(char));
    memcpy(autocomplete[i], ext_cmd_map[i], cur_len);
  }

  
  bool to_return = false;
  char *input = get_input(16, (int[]){'#', ' ', 0}, autocomplete);
    
  if(!strcmp(input, ext_cmd_map[CMD_EXT_USE_HJKL])){
    remap_keys((commands_t[]){
        CMD_UP, CMD_DOWN, CMD_LEFT, CMD_RIGHT,
          CMD_UP_LEFT, CMD_UP_RIGHT, CMD_DOWN_LEFT, CMD_DOWN_RIGHT}
      , (int []){'k', 'j', 'h', 'l', 'y', 'u', 'b', 'n'}
      , 8);
  }
  //KEY_A1 ... KEY_C3 do not work in all terminal emulators.
  else if(!strcmp(input, ext_cmd_map[CMD_EXT_USE_KEYPAD])){
    remap_keys((commands_t[]){
        CMD_UP, CMD_DOWN, CMD_LEFT, CMD_RIGHT,
          CMD_UP_LEFT, CMD_UP_RIGHT, CMD_DOWN_LEFT, CMD_DOWN_RIGHT}
      , (int []){ 0, 1, 2, 3, KEY_A1, KEY_A3, KEY_C1, KEY_C3 }
      , 8);
  }
  else if(!strcmp(input, ext_cmd_map[CMD_EXT_USE_NUMPAD])){
    remap_keys((commands_t[]){
        CMD_UP, CMD_DOWN, CMD_LEFT, CMD_RIGHT,
          CMD_UP_LEFT, CMD_UP_RIGHT, CMD_DOWN_LEFT, CMD_DOWN_RIGHT}
      , (int []){'8', '2', '4', '6', '7', '9', '1', '3'}
      , 8);
  }

  //Free things
  free(input);
  for(size_t i = 0; i < CMD_EXT_MAX; i++){
    free(autocomplete[i]);
  }
  free(autocomplete);

  //Return whether or not this action should occupy the player's turn.
  return to_return;
}

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

bool creature_move_dir(creature_t *c, map_t *m, dir_t dir){
  int check_coord = INT_MIN;
  switch(dir){
  case CMD_UP:
    check_coord = get_coord_in_arr(c->x, c->y - 1, map_get_width(m));
    c->facing = DIR_UP;
    //fall-through
  case CMD_DOWN:
    if(check_coord == INT_MIN){
      check_coord = get_coord_in_arr(c->x, c->y + 1, map_get_width(m));
      c->facing = DIR_DOWN;
    }
  case CMD_LEFT:
    if(check_coord == INT_MIN){
      check_coord = get_coord_in_arr(c->x - 1, c->y, map_get_width(m));
      c->facing = DIR_LEFT;
    }
  case CMD_RIGHT:
    if(check_coord == INT_MIN){
      check_coord = get_coord_in_arr(c->x + 1, c->y, map_get_width(m));
      c->facing = DIR_RIGHT;
    }
  case CMD_UP_LEFT:
    if(check_coord == INT_MIN){
      check_coord = get_coord_in_arr(c->x - 1, c->y - 1, map_get_width(m));
      c->facing = DIR_UL;
    }
  case CMD_UP_RIGHT:
    if(check_coord == INT_MIN){
      check_coord = get_coord_in_arr(c-> x + 1, c->y - 1, map_get_width(m));
      c->facing = DIR_UR;
    }
  case CMD_DOWN_LEFT:
    if(check_coord == INT_MIN){
      check_coord = get_coord_in_arr(c->x - 1, c->y + 1, map_get_width(m));
      c->facing = DIR_LL;
    }
  case CMD_DOWN_RIGHT:
    if(check_coord == INT_MIN){
      check_coord = get_coord_in_arr(c->x + 1, c->y + 1, map_get_width(m));
      c->facing = DIR_LR;
    }
    int map_width = map_get_width(m);
    //-1 means that the coordinate was out-of-bounds on the left, right, and top
    if(check_coord != -1
       //Check for out-of-bounds on bottom
       && check_coord < (map_width * map_get_height(m))
       //Check for physically blocked tile
       && !map_coord_is_blocked(m, check_coord)
       ){
      creature_set_coord(c, check_coord % map_width, check_coord / map_width);
      return true;
    }
  default:
    return false;
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

bool remap_keys(commands_t *cmds, int *keys, int count){
  if(cmds == NULL || keys == NULL || count < 1){
    return false;
  }

  //Make sure that there are no duplicate keys being passed in.
  for(int i = 0; i < count; i++){
    for(int j = i + 1; j < count; j++){
      if(keys[i] == keys[j]){
        return false;
      }
    }
  }
  
  //Before we make ANY changes, we need to check that ALL are possible.
  for(int i = 0; i < count; i++){
    int cur_key = keys[i];

    //Check all commands for this key
    for(int j = 0; j < CMD_MAX; j++){
      if(cmd_map[j] == cur_key){
        bool found = false;
        
        //Need to make sure it's not one of the cmds being remapped        
        for(int k = 0; k < count; k++){
          if(cmds[k] == (size_t)j){
            found = true;
            break;
          }
        }
        
        if(!found){
          return false;
        }
      }
    }
  }

  //Now actually do it
  for(int i = 0; i < count; i++){
    cmd_map[cmds[i]] = keys[i];
  }
  
  return true;
}
