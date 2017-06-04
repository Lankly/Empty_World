#ifndef ITEMS_H
#define ITEMS_H

/*******************
 * TYPE PROTOTYPES *
 *******************/
typedef struct item_t item_t;

/****************
 * USEFUL TYPES *
 ****************/
typedef enum item_type_t {
  /* WEAPONS */
  ITYPE_BOW,
  ITYPE_CROSSBOW,
  ITYPE_DAGGER,
  ITYPE_LONGBOW,
  ITYPE_LONGSWORD,
  ITYPE_RAPIER,
  ITYPE_SLING,
  ITYPE_SWORD,
  MAX_ITEM_TYPE
} item_type_t;
typedef enum beatitude_t {
  BUC_UNCURSED,
  BUC_BLESSED,
  BUC_CURSED
} beatitude_t;
typedef enum item_attribute_t {
  IATTR_IS_WEARABLE,
  IATTR_IS_WIELDABLE,
  MAX_ITEM_ATTRIBUTE
} item_attribute_t;
typedef enum attack_type_t {
  BLUNT,
  SLASHING,
  STABBING,
  MAX_ATTACK_TYPE
} attack_type_t;
typedef enum dmg_type_t {
  DMG_FIRE,
  DMG_ICE,
  DMG_PHYSICAL,
  MAX_DMG_TYPE
} dmg_type_t;

/*********
 * LISTS *
 *********/

#include "lists.h"
LINKED_LIST(ilist_t)


/************
 * INCLUDES *
 ************/

#include <stdbool.h>
#include "creatures.h"


/***********************
 * FUNCTION PROTOTYPES *
 ***********************/

void items_init();

item_t *new_item_from_type(item_type_t t);
void free_item(item_t *i);

/* ATTRIBUTE FUNCTIONS */

/**
 * Adds an attribute to a given item. Does nothing in that item already has that
 * attribute, or if the item is NULL.
 * @param i A valid item.
 * @param a An attribute to give to the item.
 */
void item_add_attribute(item_t *i, item_attribute_t a);

/**
 * Adds many attributes to a given item at once.
 * @param i A valid item.
 * @param num_attributes The number of attributes that follow.
 */
void item_add_attributes(item_t *i, int num_attributes, ...);

/**
 * Removes an attribute from a given item. Does nothing if that item did not
 * already have that attribute, or if the creature is NULL.
 * @param i A valid item.
 * @param a The attribute to remove.
 */
void item_remove_attribute(item_t *i, item_attribute_t a);

/**
 * Removes all the given attributes from the given item.
 * @param i A valid item.
 * @param num_attributes The number of atributes that follow.
 */
void item_remove_attributes(item_t *i, int num_attributes, ...);

/**
 * Determines whether a given item has a given attribute. Also returns false if
 * the item is NULL.
 * @param i A valid item.
 * @param a The attribute to check for.
 * @returns True if the item has the attribute.
 */
bool item_has_attribute(item_t *i, item_attribute_t a);

/**
 * Determines whether a given item has all the given attributes. Also returns
 * false if the item is NULL.
 * @param i A valid item.
 * @param num_attributes The number of attributes that follow.
 * @returns True if the item has all the attributes.
 */
bool item_has_attributes(item_t *i, int num_attributes, ...);

/**
 * Returns true if the given item grants the given attribute to the user.
 * @param i A valid item.
 * @param a The desired creature attribute to check for.
 * @returns True if found, false otherwise, or if the item is NULL.
 */
bool grants(item_t *i, attribute_t a);

/**
 * Determines whether a given item grants all the given attributes. Also returns
 * false if the item is NULL.
 * @param i A valid item.
 * @param num_attributes The number of creature attributes that follow.
 * @returns True if the item grants all the attributes.
 */
bool grants_many(item_t *i, int num_attributes, ...);

#endif
