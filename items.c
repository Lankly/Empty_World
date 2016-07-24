#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "classes.h"
#include "colors.h"
#include "items.h"
#include "item_callbacks.h"
#include "status.h"
#include "inventory.h"
#include "map.h"
#include "helpers.h"

void item_data_init(){
  item_data[ITEM_UNKNOWN] = (item_t){
    .id = ITEM_UNKNOWN,
    .exam_text = "This item does not exist.",
    .use = &defaultUseCallback,
    .consume = &defaultConsumeCallback,
    .zap = &defaultZapCallback
  };
  item_data[ITEM_DOWN_STAIR] = (item_t){
    .id = ITEM_DOWN_STAIR,
    .display = '>',
    .exam_text = "It is the stair down to the next floor.",
    .gold_value = -1,
    .size = 5,
    .stopme = true,
    .immovable = true,
    .material = MAT_STONE,
    .weight = 1000,
    .use = &downStairUseCallback,
    .consume = &defaultConsumeCallback,
    .zap = &defaultZapCallback
  };
  item_data[ITEM_UP_STAIR] = (item_t){
    .id = ITEM_UP_STAIR,
    .display = '<',
    .exam_text = "It is the stair up to the previous floor.",
    .gold_value = -1,
    .size = 5,
    .stopme = true,
    .immovable = true,
    .material = MAT_STONE,
    .weight = 1000,
    .use = &upStairUseCallback,
    .consume = &defaultConsumeCallback,
    .zap = &defaultZapCallback
  };
  item_data[ITEM_IRON_SWORD] = (item_t){
    .id = ITEM_IRON_SWORD,
    .display = ')',
    .exam_text = "It is a plain sword made of iron.",
    .gold_value = 5,
    .size = 1,
    .material = MAT_IRON,
    .weight = 10,
    .use = &ironSwordUseCallback,
    .consume = &defaultConsumeCallback,
    .zap = &defaultZapCallback
  };

  //    1234567890-=qwertyuiop[]\asdfghjkl;'zxcvbnm,./
  //    !@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:"ZXCVBNM<>?
  //    ±²³´µ¶·¸¹°­½ñ÷åòôùõéïðÛÝÜáóäæçèêëì»§úøãöâîí¬®¯
  //    ¡À£¤¥Þ¦ª¨©ß«Ñ×ÅÒÔÙÕÉÏÐûýüÁÓÄÆÇÈÊËÌº¢ÚØÃÖÂÎÍ¼¾¿
}

void item_map_init(item_map_t *items, int x, int y){
  if(items == NULL){
    items = Calloc(1, sizeof(item_map_t *));
  }
  items->x = x;
  items->y = y;
}

/* Adds an item to a given item_map_t. Assumes that there's enough room on the
 * stack for the item. Adds the item as the last element, since that's
 * considered the top of the pile.
 */
void item_map_add(item_map_t *items, item_t *item){
  if(items != NULL && item != NULL){
    //Create the itemlist node for the item
    itemlist_t *new = Calloc(1, sizeof(itemlist_t));
    new->item = item;

    //Find its place
    itemlist_t *cur = items->first;
    if(cur == NULL){
      //If first, add it to the beginning
      items->first = new;
    }
    else{
      //Otherwise, find the end
      while(cur->next != NULL){
	cur = cur->next;
      }
      //And add it there
      cur->next = new;
    }
  }
}

/* This function returns the top item in a given item stack. If there is no
 * such item, returns a NULL-terminator.
 */
int get_top_item_sym_from_stack(item_map_t* items){
  if(items != NULL){
    for(itemlist_t *cur = items->first; cur != NULL; cur = cur->next){
      if(cur->next == NULL){
	return cur->item->display;
      }
    }
  }
  return 0;
}

int count_items(struct map_t *map, int x, int y){
  if(map == NULL){
    return 0;
  }

  //We will loop through the list of items stacks until we find the one we want
  for(item_map_t *i = map_get_items(map); i != NULL; i = i->next){
    if(i->y == y && i->x == x){
      int to_return = 0;

      //Once found, we loop through that stack and count it
      for(struct itemlist_t *cur = i->first; cur != NULL; cur = cur->next){
	to_return += !cur->item->immovable;}
      return to_return;
    }
  }
  return 0;
}

/* Removes the indexth item from the pile at (x,y). 0-indexed.
 */
item_t *remove_item(item_map_t *map, int x, int y, int index){
  if(map == NULL || map->y > y){
    return NULL;
  }
  
  if(map->y == y && map->x == x){
    item_t *found = NULL;
    //If we're removing from the front
    if(index == 0){
      //Attempt to remove the first item
      if(map->first != NULL){
	found = map->first->item;
	itemlist_t *temp = map->first;
	map->first = map->first->next;
	free(temp);
      }
      return found;
    }

    //Otherwise, loop to find the one (before the one) to remove
    itemlist_t *cur = map->first;
    for(int i = 0; i < index - 1; i++){
      if(cur->next == NULL){
	return NULL;
      }
      cur = cur->next;
    }
    
    //Then remove it
    itemlist_t *temp = cur->next;
    cur->next = cur->next->next;
    found = temp->item;
    free(temp);
    return found;
  }

  return remove_item(map->next, x, y, index);
}

int item_weight(item_t *item){
  return item == NULL ? 0 : item->weight;
}

/* Returns the custom verb for a given weapon, if there is one. If there isn't,
 * simply returns the verb "hit".
 */
char *get_dmg_verb(item_t *weapon){
  return weapon == NULL || weapon->dmg_verb == NULL ? "hit" : weapon->dmg_verb;
}

/* Returns the name of the given item. If the item is NULL or has no name,
 * "Unknown" is returned.
 */
char *item_name(item_t *item){
  return item == NULL || item->name == NULL ? "Unknown" : item->name;
}

/* This item creates a deep copy of an item from the item_data array
 * using the given index to determine which item it will copy.
 */
item_t* item_create_from_data(int index){
  if(index<0 || index>ITEM_MAX){
    quit("Error: invalid item index");
  }

  item_t* ret = (item_t*)Calloc(1,sizeof(item_t));
  memcpy(ret,&item_data[index],sizeof(item_t));

  return ret;
}

/* This will display all the items on a given tile of the map to the player.
 * The player will then select one of them with a keypress and the index of 
 * that item on the item-stack will be returned.
 */
int items_display(struct map_t* map,int x,int y){

  //TODO

  return 0;
}

bool item_set_name(item_t *i, char *n){
  if(i == NULL || n == NULL){
    return false;
  }
  else if(i->name == NULL){
    i->name = Calloc(MAX_NAME_LEN, sizeof(char));
  }

  strcpy(i->name, n);

  return true;
}
