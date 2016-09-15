#ifndef CREATURE_H
#define CREATURE_H

typedef struct creaturelist_t clist_t;
typedef struct creature_t creature_t;

#include <stdbool.h>
#include "bodies.h"
#include "classes.h"
#include "creature_callbacks.h"
#include "inventory.h"
#include "items.h"
#include "map.h"

typedef void (*creatureTakeTurnCallback)(struct creature_t *creature,
					 map_t *map);
typedef void (*creaturePathfindCallback)(struct creature_t *creature,
					 map_t *map);
typedef void (*creatureKillCallback)(struct creature_t *creature,
				     map_t *map);

creature_t* player;

#define PLAYER_NAME_SIZE 12

//DEFINE CREATURE TYPES
#define CREATURE_TYPE_UNKNOWN  0
#define CREATURE_TYPE_SPAWNER  1
#define CREATURE_TYPE_HUMAN    2
#define CREATURE_TYPE_CANINE   3
#define CREATURE_TYPE_FELINE   4
#define CREATURE_TYPE_RODENT   5
#define CREATURE_TYPE_AVIAN    6
#define CREATURE_TYPE_INSECT   7
#define CREATURE_TYPE_GOBLIN   8
#define CREATURE_TYPE_ORC      9
#define CREATURE_TYPE_MINDFLAYER 10
#define CREATURE_TYPE_DEMON    11
#define CREATURE_TYPE_SKELETON 12
#define CREATURE_TYPE_SENTINEL 13
#define CREATURE_TYPE_GRIFFON  14
#define CREATURE_TYPE_FOG      15
#define CREATURE_TYPE_PLANT    16
#define CREATURE_TYPE_EQUINE   17
#define CREATURE_TYPE_GOLEM    18
#define CREATURE_TYPE_EYE      19
#define CREATURE_TYPE_MAX      19

//EXTRINSICS,INTRINSICS LIST
#define TRINSIC_UNKNOWN 0
#define TRINSIC_FLYING 1
#define TRINSIC_HOVER  2
#define TRINSIC_RES_FIRE 3
#define TRINSIC_RES_ICE  4
#define TRINSIC_RES_LIGHTNING 5
#define TRINSIC_TELEPATHY 6
#define TRINSIC_BLIND_TELEPATHY 7
#define TRINSIC_TELEKINESIS 8
#define TRINSIC_DARKVISION  9
#define TRINSIC_LASER_VISION  10
#define TRINSIC_TELEPORTATION 11
#define TRINSIC_CONT_TELEPORTATION 12
#define TRINSIC_POLYMORPH 13
#define TRINSIC_CONT_POLYMORPH 14
#define TRINSIC_MAX 14

//CONSUMABLES TYPES LIST
#define CONSUME_UNKNOWN 0
#define CONSUME_ANIMAL  1
#define CONSUME_SAPIENT 2
#define CONSUME_INSECT  3
#define CONSUME_FOOD    4
#define CONSUME_DRUG    5
#define CONSUME_WATER   6
#define CONSUME_ALCOHOL 7
#define CONSUME_POTION  8
#define CONSUME_BLOOD   9
#define CONSUME_NONFOOD 10
#define CONSUME_MAX 10

//BREATHABLE TYPES LIST
#define BREATHE_UNKNOWN 0
#define BREATHE_AIR 1
#define BREATHE_WATER 2
#define BREATHE_POISON 3
#define BREATHE_LAVA 4
#define BREATHE_MAX 4

/**********************************
 *    CREATURE LIST FUNCTIONS     *
 **********************************/\

/* Creates and returns a new creature list */
clist_t *clist_new(creature_t *creature);
/* Appends one creature list to another */
clist_t *clist_add(clist_t *list, clist_t *next);
clist_t *clist_add_creature(clist_t *list, creature_t *c);
/* Returns the next node in a creature list */
clist_t *clist_next(clist_t *list);
/* Returns the creature stored at a given node in a creature list */
creature_t *clist_get_creature(clist_t *list);
/* Removes and returns the creature at the given index of a given creature list
 * If the index goes out of bounds, returns NULL.
 */
creature_t *clist_remove_by_index(clist_t *list, int index);
/**/
creature_t *clist_remove_by_creature(clist_t *list, creature_t *creature);



/**********************************
 *      CREATURE FUNCTIONS        *
 **********************************/

/* Initializes all the default creatures. */
void creature_data_init();
/* Creates and returns one of the default creatures. Takes
   in a creature id from the enumeration of CREATURE_TYPEs.
 */
creature_t *creature_create_from_data(int id);

/* Returns true if a creature is allowed to exist at a given coordinate */
bool creature_can_move_to(creature_t *creature, int x, int y, int cmd);
/* Returns true if the two given creatures are the same creature */
bool creatures_equal(creature_t *first, creature_t *second);
bool creature_is_asleep(creature_t *c);
bool creature_is_blind(creature_t *c);
bool creature_is_flying(creature_t *c);
bool creature_is_immobile(creature_t *c);
bool creature_is_out_of_turns(creature_t *c);
bool creature_is_telepathic(creature_t *c);

/* Returns true if the target creature is visible to the seeing creature */
bool creature_is_visible(creature_t *target, creature_t *seer);

/* Add an item to the creature's inventory. Returns true if successful */
bool creature_add_item_to_inventory(creature_t *c, item_t *item);

/* Damages a creature. Meant for use by non-creature sources */
void damage_creature(creature_t *target, char *source, int dmg, int type);

/* Displays the inventory of a given creature */
void creature_display_inventory(creature_t *c);

/* Displays the examine-text of a creature */
void creature_examine(creature_t *c);

/* Free's the memory of a given creature. */
body_part_t *creature_free(creature_t *c);

bool creature_inv_empty(creature_t *c);

/* Handles how a creature dies on its given map. */
void creature_kill(creature_t *c, map_t *map);

/* Activate's the given creature's pathfinding algorithm on the given map */
void creature_pathfind(creature_t *c, map_t *map);

/* Places the given creature on the given map on the given coordinates */
void creature_place_at_coord(creature_t *c, map_t *map, int x, int y);

/* Places the given creature on a random walkable tile on the given map */
void creature_place_randomly_on_map(creature_t *c, map_t *map);

/* Places the given creature on a random non-walkable tile on the given map */
void creature_place_randomly_in_walls(creature_t *c, map_t *map);

/* Records the direction of the last known position of the given creature */
void creature_record_movement(creature_t *c, int move_x, int move_y);

/* Returns how far into the fog of war that the given creature can see */
int creature_see_distance(creature_t *c);

/* Creates, returns, and places a creature on
   a given map based on the given creature id.
 */
creature_t *creature_spawn(int creature_id, map_t *map);

/* Replentishes the given creature's stamina */
void creature_take_break(creature_t *c);

/* Activate's the given creature's turn algorithm */
void creature_take_turn(creature_t *c, map_t *map);



/*------ GETTERS AND SETTERS ------*/

body_part_t *creature_get_body(creature_t *c);
int   creature_get_class(creature_t *c);
void  creature_get_coord(creature_t *c, int *x, int *y, int *z);
creature_t *get_creature_at_position(int x, int y, clist_t *l);
int   creature_get_damage(creature_t *c);
int   creature_get_display(creature_t *c);
int   creature_get_last_position(creature_t *c);
int   creature_get_max_carry_weight(creature_t *c);
char *creature_get_name(creature_t *c);
void creature_get_overworld_coord(creature_t *c, int *x, int *y, int *z);
int   creature_get_skill_with_weapon(creature_t *c);
int   creature_get_type(creature_t *c);

/* Very similar to creature_place_at_coord, but with no checks */
void creature_set_coord(creature_t *c, int x, int y);
void creature_set_display(creature_t *c, int disp);
bool creature_set_name(creature_t *c, char *n);

void set_blindness(creature_t *c, bool b);
void set_unconscious(creature_t *c, bool b);
void set_immobile(creature_t *c, bool b);
void set_telepathic(creature_t *c, bool b);
void set_level(creature_t *c, int l);
void set_exam_text(creature_t *c, char *e);
void set_class(creature_t *c, int class);
void set_gold(creature_t *c, int g);

void set_strength(creature_t *c, int s);
void set_perception(creature_t *c, int p);
void set_endurance(creature_t *c, int e);
void set_charisma(creature_t *c, int ch);
void set_intelligence(creature_t *c, int i);
void set_agility(creature_t *c, int a);
void set_luck(creature_t *c, int l);
void set_dexterity(creature_t *c, int d);

void set_health(creature_t *c, int h);
void set_max_health(creature_t *c, int mh);
void set_max_hunger(creature_t *c, int m);
void set_hunger(creature_t *c, int h);

int get_level(creature_t *c);
int get_dlevel(creature_t *c);
int get_weight(creature_t *c);
int get_gold(creature_t *c);

int get_health(creature_t *c);
int get_max_health(creature_t *c);
int get_hunger(creature_t *c);
int get_max_hunger(creature_t *c);

bool is_unconscious(creature_t *c);

int get_strength(creature_t *c);
int get_perception(creature_t *c);
int get_endurance(creature_t *c);
int get_charisma(creature_t *c);
int get_intelligence(creature_t *c);
int get_agility(creature_t *c);
int get_luck(creature_t *c);
int get_dexterity(creature_t *c);

void add_health(creature_t *c, int h);

void add_breathable(creature_t *creature, int type);
void add_consumable(creature_t *creature, int type);
void add_intrinsic(creature_t *creature, int type);
void add_resistance(creature_t *creature, int type);

void remove_breathable(creature_t *creature, int type);
void remove_consumable(creature_t *creature, int type);
void remove_intrinsic(creature_t *creature, int type);
void remove_resistance(creature_t *creature, int type);

/*------ PLAYER FUNCTIONS ------*/

void player_init();

#endif
