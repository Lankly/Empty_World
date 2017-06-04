#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "items.h"
#include "trees.h"

/*********
 * TYPES *
 *********/

BTREE(attributes_t)
BTREE(iattributes_t)

struct item_t {
  char *name;
  item_type_t type;

  int index; /* Used for both coordinate on the map and place in inventory */

  beatitude_t buc_status;
  iattributes_t *attributes;
  attributes_t *extrinsics;
};


/*********************
 * PRIVATE VARIABLES *
 *********************/
item_t item_templates[MAX_ITEM_TYPE];


/*******************************
 * HELPER FUNCTION DEFINITIONS *
 *******************************/

item_t *item_new();


/************************
 * FUNCTION DEFINITIONS *
 ************************/

void items_init(){
  /* WEAPONS */
  item_templates[ITYPE_BOW] = (item_t){
    .name = "Bow",
    .type = ITYPE_BOW
  };
  item_add_attributes(&item_templates[ITYPE_BOW], 1,
                      IATTR_IS_WIELDABLE);
  
  item_templates[ITYPE_CROSSBOW] = (item_t){
    .name = "Crossbow",
    .type = ITYPE_CROSSBOW
  };
  item_add_attributes(&item_templates[ITYPE_CROSSBOW], 1,
                      IATTR_IS_WIELDABLE);
  
  item_templates[ITYPE_BOW] = (item_t){
    .name = "Bow",
    .type = ITYPE_BOW
  };
  item_add_attributes(&item_templates[ITYPE_BOW], 1,
                      IATTR_IS_WIELDABLE);
  
  item_templates[ITYPE_DAGGER] = (item_t){
    .name = "Dagger",
    .type = ITYPE_DAGGER
  };
  item_add_attributes(&item_templates[ITYPE_DAGGER], 1,
                      IATTR_IS_WIELDABLE);
  
  item_templates[ITYPE_LONGBOW] = (item_t){
    .name = "Longbow",
    .type = ITYPE_LONGBOW
  };
  item_add_attributes(&item_templates[ITYPE_LONGBOW], 1,
                      IATTR_IS_WIELDABLE);
  
  item_templates[ITYPE_LONGSWORD] = (item_t){
    .name = "Longsword",
    .type = ITYPE_LONGSWORD
  };
  item_add_attributes(&item_templates[ITYPE_LONGSWORD], 1,
                      IATTR_IS_WIELDABLE);
  
  item_templates[ITYPE_RAPIER] = (item_t){
    .name = "Rapier",
    .type = ITYPE_RAPIER
  };
  item_add_attributes(&item_templates[ITYPE_RAPIER], 1,
                      IATTR_IS_WIELDABLE);
  //TODO: Finish these
}

item_t *new_item_from_type(item_type_t t){
  item_t *to_return = item_new();

  memcpy(to_return, &item_templates[t], sizeof(item_t));

  to_return->attributes = tree_copy(item_templates[t].attributes);
  
  return to_return;
}

void free_item(item_t *i){
  if(i == NULL){
    return;
  }

  if(i->attributes != NULL){
    free(i->attributes);
  }
  free(i);
}

/* ATTRIBUTE FUNCTIONS */

void item_add_attribute(item_t *i, item_attribute_t a){
  if(i == NULL){
    return;
  }

  i->attributes = tree_insert(i->attributes, &a, int_cmp);
}

void item_add_attributes(item_t *i, int num_attributes, ...){
  if(i == NULL || num_attributes < 1){
    return;
  }

  va_list args;
  va_start(args, num_attributes);

  for(int j = 0; j < num_attributes; j++){
    item_attribute_t arg = va_arg(args, item_attribute_t);

    item_add_attribute(i, arg);
  }
  
  va_end(args);
}

void item_remove_attribute(item_t *i, item_attribute_t a){
  if(i == NULL){
    return;
  }

  i->attributes = tree_remove(i->attributes, &a, &int_cmp);
}

void item_remove_attributes(item_t *i, int num_attributes, ...){
  if(i == NULL || num_attributes < 0){
    return;
  }

  va_list args;
  va_start(args, num_attributes);

  for(int j = 0; j < num_attributes; j++){
    item_attribute_t arg = va_arg(args, item_attribute_t);
    item_remove_attribute(i, arg);
  }
  
  va_end(args);
}

bool item_has_attribute(item_t *i, item_attribute_t a){
  if(i == NULL){
    return false;
  }
  
  return tree_search(i->attributes, &a, int_cmp) != NULL;
}

bool item_has_attributes(item_t *i, int num_attributes, ...){
  if(i == NULL || num_attributes < 0){
    return true;
  }

  va_list args;
  va_start(args, num_attributes);
  bool result = true;
  
  for(int j = 0; j < num_attributes; j++){
    item_attribute_t arg = va_arg(args, item_attribute_t);

    result = result && item_has_attribute(i, arg);
  }
  
  va_end(args);

  return result;
}

/* DATA FUNCTIONS */

int item_get_index(item_t *i){
  if(i == NULL){
    return -1;
  }

  return i->index;
}

void item_set_index(item_t *i, int index){
  if(i == NULL){
    return;
  }

  i->index = index;
}

/* EXTRINSIC FUNCTIONS */

bool grants(item_t *i, attribute_t a){
  if(i == NULL){
    return false;
  }

  return tree_search(i->extrinsics, &a, int_cmp) != NULL;
}

bool grants_many(item_t *i, int num_attributes, ...){
  if(i == NULL || num_attributes < 0){
    return true;
  }

  va_list args;
  va_start(args, num_attributes);
  bool result = true;
  
  for(int j = 0; j < num_attributes; j++){
    attribute_t arg = va_arg(args, attribute_t);

    result = result && grants(i, arg);
  }
  
  va_end(args);

  return result;
}

/*******************************
 * HELPER FUNCTION DEFINITIONS *
 *******************************/

item_t *item_new(){
  return Calloc(1, sizeof(item_t));
}
