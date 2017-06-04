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
