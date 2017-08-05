#ifndef CREATURES_H
#define CREATURES_H

/*******************
 * TYPE PROTOTYPES *
 *******************/
//typedef intrinsics_t extrinsics_t; /* Uncomment this when intrinsics exist */
typedef struct creature_t creature_t;


/**********************
 * USEFUL DEFINITIONS *
 **********************/

/****************
 * USEFUL TYPES *
 ****************/

typedef enum attribute_t {
  ATTR_IS_PLAYER,

  /* Body parts (To be used in coordination with bodies.h) */
  ATTR_HAS_BRAIN,
  ATTR_HAS_HEAD,
  ATTR_HAS_MANY_EYES,    ATTR_HAS_RIGHT_EYE,   ATTR_HAS_LEFT_EYE,
  ATTR_HAS_MANY_ARMS,    ATTR_HAS_RIGHT_ARM,   ATTR_HAS_LEFT_ARM,
  ATTR_HAS_MANY_LEGS,    ATTR_HAS_RIGHT_LEG,   ATTR_HAS_LEFT_LEG,
  ATTR_HAS_RIGHT_HAND,   ATTR_HAS_LEFT_HAND,
  ATTR_HAS_RIGHT_PINKIE, ATTR_HAS_LEFT_PINKIE,
  ATTR_HAS_RIGHT_RING,   ATTR_HAS_LEFT_RING,
  ATTR_HAS_RIGHT_MIDDLE, ATTR_HAS_LEFT_MIDDLE,
  ATTR_HAS_RIGHT_FIRST,  ATTR_HAS_LEFT_FIRST,
  ATTR_HAS_RIGHT_THUMB,  ATTR_HAS_LEFT_THUMB,
  ATTR_HAS_RIGHT_FOOT,   ATTR_HAS_LEFT_FOOT,
  ATTR_HAS_MANY_WINGS,   ATTR_HAS_LEFT_WING,   ATTR_HAS_RIGHT_WING,
  ATTR_HAS_BLOOD,        ATTR_HAS_JELLY,
  ATTR_HAS_FEATHERS,     ATTR_HAS_FUR,         ATTR_HAS_HAIR,
  ATTR_HAS_SKELETON,     ATTR_HAS_EXOSKELETON, ATTR_HAS_CHASSIS,
  ATTR_HAS_TEETH,        ATTR_HAS_CLAWS,       ATTR_HAS_FANGS,

  /* Curses */
  ATTR_CURSE_OF_BAD_LUCK,      ATTR_CURSE_OF_FLYING, ATTR_CURSE_OF_POLYMORPH,
  ATTR_CURSE_OF_TELEPORTATION,

  /* Diseases */
  ATTR_CRYOASTHENIA, ATTR_POLYFORMIASIS, ATTR_PYROASTHENIA,
  
  /* Food & Diet Section */
  ATTR_IS_CARNAVOR,  ATTR_IS_HERBAVOR, ATTR_IS_METALVORE,
  ATTR_IS_PLANT,     ATTR_IS_PREY,     ATTR_IS_PREDATOR,  ATTR_IS_APEX_PREDATOR,
  ATTR_IS_POISONOUS, ATTR_IS_ROTTEN,
  ATTR_IS_STARVING,  ATTR_IS_HUNGRY,   ATTR_IS_SATIATED,  ATTR_IS_OVERSATIATED,
  
  /* Material Section */
  ATTR_IS_STRAW,    ATTR_IS_COPPER,     ATTR_IS_SILVER, ATTR_IS_GOLD,
  ATTR_IS_PLATINUM, ATTR_IS_ADAMANTINE, ATTR_IS_STEEL,
  
  /* Mobility Section */
  ATTR_IS_IMMOBILE,       ATTR_IS_FLYING,           ATTR_IS_HOVERING,
  ATTR_MOVES_ALONG_WALLS, ATTR_MOVES_THROUGH_WALLS,
  ATTR_IS_STUNNED,        ATTR_IS_FROZEN,           ATTR_IS_PINNED,
  
  /* Resistances */
  RESIST_FIRE, RESIST_ICE, RESIST_PHYSICAL, RESIST_PSIONIC,

  /* Pronouns ("It" by default, "You" if ATTR_IS_PLAYER) */
  ATTR_HE, ATTR_SHE, ATTR_THEY,
  
  MAX_ATTRIBUTE
} attribute_t;
typedef enum class_t {
  CLASS_PRIEST,
  CLASS_SPY,
  CLASS_TINKER,
  CLASS_WARRIOR,
  MAX_CLASS
} class_t;
typedef enum species_t {
  SPECIES_CANINE,
  SPECIES_DWARF,
  SPECIES_ELF,
  SPECIES_FELINE,
  SPECIES_HUMAN,
  SPECIES_RODENT,
  MAX_SPECIES
} species_t;
typedef enum title_t{
  TITLE_NONE,
  TITLE_MINOR,
  TITLE_MAJOR,
  TITLE_MASTER,
  MAX_TITLE
} title_t;


/*********
 * LISTS *
 *********/
#include "lists.h"
LINKED_LIST(clist_t)


/************
 * INCLUDES *
 ************/

#include <stdbool.h>
#include "items.h"
#include "maps.h"


/***********************
 * FUNCTION PROTOTYPES *
 ***********************/

/**
 * Initializes all the internal data for creatures. This MUST be called
 * before creating any new creatures from templates.
 */
void creatures_init();

creature_t *new_creature();
creature_t *new_creature_by_species(species_t s);

/* Attribute-related functions */

/**
 * Adds an attribute to a given creature. Does nothing if that creature already
 * has that attribute, or if the creature is NULL.
 * @param c A valid creature.
 * @param a An attribute to give to the creature.
 */
void add_attribute(creature_t *c, attribute_t a);
/**
 * Adds many attributes to a given creature at once.
 * @param c A valid creature.
 * @param num_attributes The number of attributes that follow.
 */
void add_attributes(creature_t *c, int num_attributes, ...);
/**
 * Removes an attribute from a given creature. Does nothing if that creature did
 * not already have that attribute, or if the creature is NULL.
 * @param c A valid creature
 * @param a An attribute to remove.
 */
void remove_attribute(creature_t *c, attribute_t a);
/**
 * Removes all the given attributes from the given creature.
 * @param c A valid creature.
 * @param num_attributes The number of atributes that follow.
 */
void remove_attributes(creature_t *c, int num_attributes, ...);

/**
 * Determines whether a given creature has a given attribute. Also returns false
 * if the creature is NULL.
 * @param c A valid creature.
 * @param a An attribute to check for.
 * @returns True if the creature has the attribute.
 */
bool has_attribute(creature_t *c, attribute_t a);

/**
 * Determines whether a given creature has all the given attributes. Also
 * returns false if the creature is NULL.
 * @param c A valid creature.
 * @param num_attributes The number of attributes that follow.
 * @returns True if the creature has all the attributes.
 */
bool has_attributes(creature_t *c, int num_attributes, ...);

/**
 * Determines whether the given creature is granted the given attribute by an
 * item in its inventory.
 * @param c A valid creature.
 * @param a An attribute to check for.
 */
bool has_extrinsic(creature_t *c, attribute_t a);/* Damage-related functions */

/**
 * Handles everything associated with dealing a specific creature damage,
 * including whether or not that damage was lethal, and whether it should be
 * resisted.
 * @param c A valid creature.
 * @param amount The amount of damage to attempt to deal to the creature.
 * @param t The type of damage.
 */
void creature_take_damage(creature_t *c, int amount, dmg_type_t t);

/* Data-related functions */

/**
 * Returns an index into a 2-D representation of a map, representing the given
 * creature's current location, or -1 if the creature or map was NULL. The map
 * is required for the conversion of the xy pair into a 1-D index.
 * @param c A valid creature.
 * @param m A valid map.
 * @returns An index representing the creature's current coordinates, or -1.
 */
int creature_get_coord(creature_t *c, map_t *m);

/**
 * Returns the given creature's x-coordinate for the map it's currently on.
 * @param c A valid creature.
 * @returns The x-coordinate, or -1.
 */
int creature_get_xcoord(creature_t *c);

/**
 * Returns the given creature's y-coordinate for the map it's currently on.
 * @param c A valid creature.
 * @returns The y-coordinate, or -1.
 */
int creature_get_ycoord(creature_t *c);

/**
 * Set's a given creature's coordinates. Does nothing if the creature is NULL,
 * or if x or y is less than zero.
 * @param c A valid creature.
 * @param x The x-coordinate.
 * @param y The y-coordinate.
 */
void creature_set_coord(creature_t *c, int x, int y);

/**
 * Returns an integer representing the given creature with NCURSES color
 * information. If the creature is NULL, returns ' ' instead.
 * @param c A valid creature.
 * @returns An integer that NCURSES can display.
 */
int creature_get_display(creature_t *c);

/* Map-related functions */

/**
 * Returns the coordinates of the most important thing that the given creature
 * can see. If no target could be found, returns -1.
 * @param c A valid creature.
 * @param m The map that creature is on.
 * @returns A coordinate 
 */
dijkstra_t *creature_get_targets(creature_t *c, map_t *m);

/* Nutrition-related functions */

/**
 * Adds (or subtracts, if amount is negative) amount from the given creature's
 * internal nutrition.
 * @param c A valid creature.
 * @param amount The amount of nutrition to add.
 */
void creature_add_nutrition(creature_t *c, int amount);

/* Stat-related functions */

/**
 * Prints a window with the given creature's stats.
 * @param c A valid creature.
 */
void creature_display_stats(creature_t *c);

/* Turn-related functions */

/**
 * Determines whether or not a given creature is exhausted or not. If it is, its
 * turn should be skipped and creature_take_break() should be called.
 * @param c A valid creature.
 * @returns True if the creature is exhausted. False if it is not or it is NULL.
 */
bool creature_is_out_of_turns(creature_t *c);

/**
 * Refreshes a given creature's stamina. Should be called if the creature did
 * nothing on the previous turn.
 * @param c A valid creature.
 */
void creature_take_break(creature_t *c);

/**
 * Execute's a turn for a given creature. Does nothing if the map or the
 * creature was NULL.
 * @param c A valid creature.
 * @param m A valid map.
 */
void creature_take_turn(creature_t *c, map_t *m);

/* Other functions */

/**
 * Returns an ncurses-printable character representing the given creature. If it
 * could not be determined, returns '\0' instead.
 * @param c A valid creature.
 * @returns A character that ncurses can print.
 */
int creature_get_icon(creature_t *c);


/******************************
 * PLAYER FUNCTION PROTOTYPES *
 ******************************/

/**
 * Creates a new player. Will prompt the player for input to pick their class,
 * pick their species, and assign their starting stats.
 * @returns A valid creature marked as a player.
 */
creature_t *new_player();

void display_status_line(creature_t *player, int row);

/**
 * Determines whether or not the given creature is marked as a player.
 * Just checks for the ATTR_IS_PLAYER attribute.
 * @param A valid creature.
 * @returns True if the creature is a player.
 */
bool is_player(creature_t *c);

/**
 * A special function that allows the player to take their turn instead of
 * acting like a normal creature.
 * @param p The player.
 * @param m The map the player is currently on.
 */
void player_take_turn(creature_t *p, map_t *m);

#endif
