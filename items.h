#ifndef ITEMS_H
#define ITEMS_H

#include <stdbool.h>

/****************
 * USEFUL TYPES *
 ****************/
typedef struct item_t item_t;
typedef enum item_type_t {
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
  IATTR_IS_WEAPON,
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

/***********************
 * FUNCTION PROTOTYPES *
 ***********************/

void items_init();

item_t *new_item_from_type(item_type_t t);
void free_item(item_t *i);

void item_add_attribute(item_t *t, item_attribute_t a);
void item_remove_attribute(item_t *t, item_attribute_t a);
bool item_has_attribute(item_t *i, item_attribute_t a);

#endif
