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

/**
 * Execute's a turn for a given creature. Does nothing if the map or the
 * creature was NULL.
 */
void creature_take_turn(creature_t *c, map_t *m);

/* Attribute-related functions */

/**
 * Adds an attribute to a given creature. Does nothing if that creature already
 * has that attribute, or if the creature is NULL.
 */
void add_attribute(creature_t *c, attribute_t a);
/**
 * Adds many attributes to a given creature at once.
 */
void add_attributes(creature_t *c, int num_attributes, ...);
/**
 * Removes an attribute from a given creature. Does nothing if that creature did
 * not already have that attribute, or if the creature is NULL.
 */
void remove_attribute(creature_t *c, attribute_t a);
/**
 * Removes all the given attributes from the given creature.
 */
void remove_attributes(creature_t *c, int num_attributes, ...);

/**
 * Returns true if the given creature has the given attribute. Also returns
 * false if the creature is NULL.
 */
bool has_attribute(creature_t *c, attribute_t a);

/**
 * Returns true if the given creature has all the given attributes. Also returns
 * false if the creature is NULL.
 */
bool has_attributes(creature_t *c, int num_attributes, ...);

/**
 * Returns true if the given creature is granted the given attribute by an item
 * in their inventory.
 */
bool has_extrinsic(creature_t *c, attribute_t a);

/* Damage-related functions */

/**
 * Handles everything associated with dealing a specific creature damage,
 * including whether or not that damage was lethal, and whether it should be
 * resisted.
 */
void creature_take_damage(creature_t *c, int amount, dmg_type_t t);

/* Map-related functions */

/**
 * Returns the coordinates of the most important thing that the given creature
 * can see.
 */
int creature_get_target(creature_t *c, map_t *m);

/**
 * Teleports a given creature to the given coordinates on the given map. If the
 * creature or map is NULL, if the coordinates are out of bounds, or if the
 * space is blocked, does not move the creature and returns false;
 */
bool creature_teleport(creature_t *c, map_t *m, int x, int y);

/* Nutrition-related functions */

/**
 * Adds (or subtracts, if amount is negative) amount from the given creature's
 * internal nutrition.
 */
void creature_add_nutrition(creature_t *c, int amount);

/* Stat-related functions */

/**
 * Prints a window with the given creature's stats.
 */
void creature_display_stats(creature_t *c);

/* Other functions */

/**
 * Returns an ncurses-printable character representing the given creature. If it
 * could not be determined, returns '\0' instead.
 */
int creature_get_icon(creature_t *c);


/******************************
 * PLAYER FUNCTION PROTOTYPES *
 ******************************/

creature_t *new_player(class_t c, species_t s);

void display_status_line(creature_t *player, int row);

bool is_player(creature_t *c);

void player_take_turn(creature_t *p, map_t *m);

#endif
