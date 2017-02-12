#include <stdlib.h>
#include "items.h"
#include "trees.h"

/*********
 * TYPES *
 *********/

BTREE(attributes_t)

struct item_t {
  char *name;
  item_type_t type;

  beatitude_t buc_status;
  attributes_t *attributes;
};


/*********************
 * PRIVATE VARIABLES *
 *********************/
item_t item_templates[MAX_ITEM_TYPE];


/************************
 * FUNCTION DEFINITIONS *
 ************************/
