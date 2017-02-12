#ifndef CREATURES_H
#define CREATURES_H

#include <stdbool.h>
#include "items.h"
#include "maps.h"

/**********************
 * USEFUL DEFINITIONS *
 **********************/
#define extrinsics_t intrinsics_t


/****************
 * USEFUL TYPES *
 ****************/
typedef struct creature_t creature_t;

typedef enum attribute_t {
  ATTR_IS_PLAYER,
  /* Diet Section */
  ATTR_IS_CARNAVOR,
  ATTR_IS_HERBAVOR,
  ATTR_IS_OMNIVORE, /* OMNIVORE in this context is just CARN + HERB */
  ATTR_IS_METALVORE,
  ATTR_IS_POISONOUS,
  /* Food Chain Section */
  ATTR_IS_PLANT,
  ATTR_IS_PREY,
  ATTR_IS_PREDATOR,
  ATTR_IS_APEX_PREDATOR,
  /* Material Section */
  ATTR_IS_FEATHERED,
  ATTR_IS_FLESHY,
  /** Metal Sub-section**/
  ATTR_IS_COPPER,
  ATTR_IS_SILVER,
  ATTR_IS_GOLD,
  ATTR_IS_PLATINUM,
  ATTR_IS_ADAMANTINE,
  ATTR_IS_STEEL,
  /* Mobility Section */
  ATTR_IS_FLYING,
  ATTR_IS_IMMOBILE,
  ATTR_MOVES_ALONG_WALLS,
  ATTR_MOVES_THROUGH_WALLS,
  /* Resistances */
  RESIST_FIRE,
  RESIST_ICE,
  RESIST_PHYSICAL,
  RESIST_PSIONIC,
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
void creature_add_attribute(creature_t *c, attribute_t a);
/**
 * Removes an attribute from a given creature. Does nothing if that creature did
 * not already have that attribute, or if the creature is NULL.
 */
void creature_remove_attribute(creature_t *c, attribute_t a);

/**
 * Returns true if the given creature has the given attribute. Returns false if
 * the creature is NULL.
 */
bool creature_has_attribute(creature_t *c, attribute_t a);

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

/**
 * Returns true if the creature's internal nutrition value is low.
 */
bool creature_is_hungry(creature_t *c);

/**
 * Returns true if the creature's internal nutrition value is high.
 */
bool creature_is_satiated(creature_t *c);

/**
 * Returns true if the creature's internal nutrition value is very high.
 */
bool creature_is_oversatiated(creature_t *c);

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
