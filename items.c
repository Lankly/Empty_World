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
#include "player.h"

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

  item_data[CORPSE_HUMAN] = (item_t){
    .id = CORPSE_HUMAN,
    .display = '@' | COLOR_PAIR(CP_DARK_GREY_BLACK),
    .name = "Human Corpse",
    .exam_text = "It is the corpse of a human.",
    .size = 1,
    .use = &defaultUseCallback,
    .consume = &defaultConsumeCallback,
    .zap = &defaultZapCallback
  };
  item_data[CORPSE_ORC] = (item_t){
    .id = CORPSE_ORC,
    .display = 'R' | COLOR_PAIR(CP_DARK_GREY_BLACK),
    .name = "Orc Corpse",
    .exam_text = "It is the corpse of an orc.",
    .size = 1,
    .use = &defaultUseCallback,
    .consume = &defaultConsumeCallback,
    .zap = &defaultZapCallback
  };
  item_data[CORPSE_GOBLIN] = (item_t){
    .id = CORPSE_GOBLIN,
    .display = 'G' | COLOR_PAIR(CP_DARK_GREY_BLACK),
    .name = "Goblin Corpse",
    .exam_text = "It is the corpse of a goblin.",
    .size = 1,
    .use = &defaultUseCallback,
    .consume = &defaultConsumeCallback,
    .zap = &defaultZapCallback
  };
  item_data[CORPSE_SKELETON] = (item_t){
    .id = CORPSE_SKELETON,
    .display = 'S' | COLOR_PAIR(CP_DARK_GREY_BLACK),
    .name = "Skeleton",
    .exam_text = "It a skeleton (Thankfully inanimate).",
    .size = 1,
    .use = &defaultUseCallback,
    .consume = &defaultConsumeCallback,
    .zap = &defaultZapCallback
  };
  item_data[CORPSE_CANINE] = (item_t){
    .id = CORPSE_CANINE,
    .display = 'd' | COLOR_PAIR(CP_DARK_GREY_BLACK),
    .name = "Canine Corpse",
    .exam_text = "It is the corpse of a canine.",
    .size = 1,
    .use = &defaultUseCallback,
    .consume = &defaultConsumeCallback,
    .zap = &defaultZapCallback
  };
  item_data[CORPSE_FELINE] = (item_t){
    .id = CORPSE_FELINE,
    .display = 'c' | COLOR_PAIR(CP_DARK_GREY_BLACK),
    .name = "Feline Corpse",
    .exam_text = "It is the corpse of a feline.",
    .size = 1,
    .use = &defaultUseCallback,
    .consume = &defaultConsumeCallback,
    .zap = &defaultZapCallback
  };
  item_data[CORPSE_INSECT] = (item_t){
    .id = CORPSE_INSECT,
    .display = 'h' | COLOR_PAIR(CP_DARK_GREY_BLACK),
    .name = "Hornet Corpse",
    .exam_text = "It is the corpse of a hornet. Yay!",
    .size = 1,
    .use = &defaultUseCallback,
    .consume = &defaultConsumeCallback,
    .zap = &defaultZapCallback
  };
  item_data[CORPSE_AVIAN] = (item_t){
    .id = CORPSE_AVIAN,
    .display = 'o' | COLOR_PAIR(CP_DARK_GREY_BLACK),
    .name = "Owl Corpse",
    .exam_text = "It is the corpse of an owl.",
    .size = 1,
    .use = &defaultUseCallback,
    .consume = &defaultConsumeCallback,
    .zap = &defaultZapCallback
  };
  item_data[CORPSE_DEMON] = (item_t){
    .id = CORPSE_DEMON,
    .display = 'E' | COLOR_PAIR(CP_DARK_GREY_BLACK),
    .name = "Demon Corpse",
    .exam_text = "It is the corpse of a demon.",
    .size = 1,
    .use = &defaultUseCallback,
    .consume = &defaultConsumeCallback,
    .zap = &defaultZapCallback
  };
  item_data[CORPSE_MINDFLAYER] = (item_t){
    .id = CORPSE_MINDFLAYER,
    .display = 'M' | COLOR_PAIR(CP_DARK_GREY_BLACK),
    .name = "Mindflayer Corpse",
    .exam_text = "It is the corpse of a mindflayer. Something about it unnerves you.",
    .size = 1,
    .use = &defaultUseCallback,
    .consume = &defaultConsumeCallback,
    .zap = &defaultZapCallback
  };
  item_data[CORPSE_GRIFFON] = (item_t){
    .id = CORPSE_GRIFFON,
    .display = 'G' | COLOR_PAIR(CP_DARK_GREY_BLACK),
    .name = "Griffon Corpse",
    .exam_text = "It is the corpse of a griffon.",
    .size = 1,
    .use = &defaultUseCallback,
    .consume = &defaultConsumeCallback,
    .zap = &defaultZapCallback
  };
  item_data[CORPSE_SENTINEL] = (item_t){
    .id = CORPSE_SENTINEL,
    .display = 'T' | COLOR_PAIR(CP_DARK_GREY_BLACK),
    .name = "Sentinel Corpse",
    .exam_text = "It is the corpse of a sentinel.",
    .size = 1,
    .use = &defaultUseCallback,
    .consume = &defaultConsumeCallback,
    .zap = &defaultZapCallback
  };
  item_data[CORPSE_PLANT] = (item_t){
    .id = CORPSE_PLANT,
    .display = 'f' | COLOR_PAIR(CP_DARK_GREY_BLACK),
    .name = "Fungus Corpse",
    .exam_text = "It is a pile of moss.",
    .size = 1,
    .use = &defaultUseCallback,
    .consume = &defaultConsumeCallback,
    .zap = &defaultZapCallback
  };
  item_data[CORPSE_EYE] = (item_t){
    .id = CORPSE_EYE,
    .display = 'e' | COLOR_PAIR(CP_DARK_GREY_BLACK),
    .name = "Eyeball",
    .exam_text = "It is an eyeball. Sure does look tasty... What?",
    .size = 1,
    .use = &defaultUseCallback,
    .consume = &defaultConsumeCallback,
    .zap = &defaultZapCallback
  };
  item_data[CORPSE_SPAWNER] = (item_t){
    .id = CORPSE_SPAWNER,
    .display = '%' | COLOR_PAIR(CP_DARK_GREY_BLACK),
    .name = "??? Corpse",
    .exam_text = "It is the corpse of a...",
    .size = 1,
    .use = &defaultUseCallback,
    .consume = &defaultConsumeCallback,
    .zap = &defaultZapCallback
  };
  item_data[CORPSE_EQUUS] = (item_t){
    .id = CORPSE_EQUUS,
    .display = 'H' | COLOR_PAIR(CP_DARK_GREY_BLACK),
    .name = "Horse Corpse",
    .exam_text = "It is the corpse of a horpse.",
    .size = 1,
    .use = &defaultUseCallback,
    .consume = &defaultConsumeCallback,
    .zap = &defaultZapCallback
  };

  //    1234567890-=qwertyuiop[]\asdfghjkl;'zxcvbnm,./
  //    !@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:"ZXCVBNM<>?
  //    ±²³´µ¶·¸¹°­½ñ÷åòôùõéïðÛÝÜáóäæçèêëì»§úøãöâîí¬®¯
  //    ¡À£¤¥Þ¦ª¨©ß«Ñ×ÅÒÔÙÕÉÏÐûýüÁÓÄÆÇÈÊËÌº¢ÚØÃÖÂÎÍ¼¾¿
}

void item_map_init(struct item_map_t *items, int x, int y){
  items->x=x;
  items->y=y;
}

int count_items(struct map_t *map, int x, int y){
  if(x < 0 || y < 0 || get_coord(x, y, map->width) > map->width*map->height){
    quit("Error: count_items Index Out of Bounds");
  }

  //We will loop through the list of items stacks until we find the one we want
  for(item_map_t *i = map->items; i != NULL; i = i->next){
    if(i->y == y && i->x == x){
      int to_return = 0;

      //Once found, we loop through that stack and count it
      for(struct item_map_node_t *cur = i->first; cur != NULL; cur = cur->next){
	to_return += !cur->item->immovable;}
      return to_return;
    }
  }
  return 0;
}

/* Adds an item to the given map at the given coordinates. reveal is to tell
 * if we are adding this to the list of known items or not.
 */
bool add_item(struct map_t* map, int x, int y, struct item_t* item, bool reveal){
  if(map==NULL){quit("Error: NULL map");}
  if(item->size > map->max_item_height){return false;}
  item_map_t *cur = NULL;
  if(x<0 || y<0 || get_coord(x,y,map->width) > map->width*map->height){
    return false;
  }

  //Case where no items exist yet
  if((!reveal && map->items == NULL) || (reveal && map->known_items == NULL)){
    item_map_t* items = (item_map_t*)Calloc(1, sizeof(item_map_t));
    item_map_init(items, x, y);
    if(reveal){
      map->known_items = items;}
    else{
      map->items = items;}

    items->first = Calloc(1,sizeof(item_map_node_t));
    items->first->item = item;
    items->size += item->size;
    return true;
  }
  //Now we loop through to find where to insert our data
  else{
    item_map_t* last;
    for(item_map_t* i = reveal ? map->known_items : map->items;
	i != NULL;
	i = i->next){
      last = i;
      //If we've gone too far, create a new spot for our data
      if(i->y > y || (i->y == y && i->x > x)){
	item_map_t* items = (item_map_t*)Calloc(1,sizeof(item_map_t));
	item_map_init(items, x, y);

	//Need to make sure this isn't first element
	if(i->prev != NULL){
	  i->prev->next=items;}
	else if(reveal){
	  map->known_items = items;}
	else{
	  map->items = items;}

	items->prev = i->prev;
	items->next = i;
	i->prev = items;
	cur = items; 
	break;
      }
      //Linked List already exists at x,y
      else if(i->y==y && i->x==x){
	cur=i;break;
      }
    }
    //Case where it is inserted last
    if(cur == NULL){
      item_map_t* items = (item_map_t*)Calloc(1,sizeof(item_map_t));
      item_map_init(items, x, y);
      last->next = items;
      items->prev = last;
      cur = items;
    }
  }
  //If there is no more room in this stack or it cannot fit
  if(cur->size+item->size > map->max_item_height){
    return false;
  }
  //Finally, add the item
  item_map_node_t* insert = (item_map_node_t*)Calloc(1,sizeof(item_map_node_t));
  insert->item = item;
  insert->next = cur->first;
  cur->first = insert;
  
  //If the item can't be moved, don't bother adding it to the stack size
  if(!item->immovable){
    cur->size += item->size;}
  return true;
}

//Removes an item from the map
item_t* remove_item(struct map_t* map,int x,int y,int index){
  if(get_coord(x,y,map->width)>map->width*map->height){
    quit("Error: Items Index Out of Bounds");
  }

  //Find the specific item stack on the map
  item_map_t* cur=NULL;
  for(item_map_t* i=map->items;i!=NULL;i=i->next){
    if(i->y>y || (i->y==y && i->x>x)){break;}
    cur=i;
    if(i->y==y && i->x==x){break;}
  }
  if(cur==NULL){quit("Error: NULL item stack");}

  //Navigate to the correct item on the stack
  item_map_node_t* last = NULL;
  item_map_node_t* cur_node = cur->first;
  for(int i=0;i<index;i++){
    last=cur_node;
    cur_node=cur_node->next;    
    if(cur==NULL){quit("Error: Stack Index out of bounds");}
  }
  //Grab the item off it, remove its node from the stack
  item_t* to_return=cur_node->item;
  if(last==NULL){
    cur->first=cur_node->next;
  }
  else{
    last->next=cur_node->next;
  }

  /* If item is immovable, it couldn't have been picked up, so we don't have to
   * worry about it here.
   */
  cur->size -= to_return->size;

  free(cur_node);
  return to_return;
}

/* This function unequips the given item if it can,
 * and then frees that item. It does not care whether the
 * item was in the player's inventory or not when it frees the item
 */
void destroy_item(struct item_t* item){
  unequip_by_item(player,item);
  free(item);
}

/* This function returns a char representing the item at the given index on
 * the item stack at the given coordinates in the given map.
 */
int get_item_sym(struct map_t* map,int x,int y,int index){
  if(get_coord(x, y, map->width) > map->width*map->height){
    quit("Error: Items Index Out of Bounds");
  }

  //Navigate to the correct item-stack on the map
  item_map_t *cur=NULL;
  for(item_map_t *i = map->items; i != NULL; i = i->next){
    if(i->y > y || (i->y == y && i->x > x)){
      break;}
    if(i->y == y && i->x == x){
      cur = i;}
  }
  if(cur==NULL){return -1;}

  //Navigate to the correct item on the stack
  item_map_node_t* cur_node = cur->first;
  for(int i=0;i<index;i++){
    cur_node=cur_node->next;
    if(cur==NULL){quit("Error: Stack Index out of bounds");}
  }
  //Return the symbol from here
  char to_return = cur_node->item->display;
  return to_return;  
}

/* This function returns the top item in an item stack by calling
 * get_item_sym() with the index=0.
 */
int get_top_item_sym(struct map_t* map,int x,int y){
  return get_item_sym(map,x,y,0);
}

/* This function returns the top item in a given item stack. It 
 * checks to make sure it can first, though.
 */
int get_top_item_sym_from_stack(struct item_map_t* items){
  if(items==NULL){quit("Error: NULL items stack");}
  if(items->first==NULL){quit("Error: No first item");}
  
  return items->first->item->display;
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

item_t *create_corpse(char *name, int type, 
			     int display, int class){
  //Create basic  corpse
  struct item_t *corpse = item_create_from_data(ITEM_UNKNOWN);
  corpse->display = display | COLOR_PAIR(CP_DARK_GREY_BLACK);
  corpse->name = Calloc(MAX_NAME_LEN + 1, sizeof(char));
  corpse->exam_text = Calloc(MAX_MSG_LEN + 1, sizeof(char));
  corpse->size = 1;
  
  /* Customize corpse name, exam text */

  strcat(corpse->name, name);

  strcat(corpse->exam_text, "It is the corpse of a");
  //a -> an where necessary
  if(corpse->name[0]  ==  'a'  || corpse->name[0] == 'A'
     || corpse->name[0] == 'e' || corpse->name[0] == 'E'
     || corpse->name[0] == 'i' || corpse->name[0] == 'I'
     || corpse->name[0] == 'o' || corpse->name[0] == 'O'
     || corpse->name[0] == 'u' || corpse->name[0] == 'U'){
    strcat(corpse->exam_text, "n");}
  strcat(corpse->exam_text, " ");
  strcat(corpse->exam_text, corpse->name);

  if(class != CLASS_UNKNOWN){
    strcat(corpse->name, " ");
    strcat(corpse->name, class_data[class].name);}


  strcat(corpse->exam_text, ".");

  //If  is same type as player, add frowny face
  strcat(corpse->name, " Corpse");
  if(player->creature_id == type
     || player->were_type == type){
    strcat(corpse->exam_text, " :(");}

  return corpse;
}

/* This will display all the items on a given tile of the map to the player.
 * The player will then select one of them with a keypress and the index of 
 * that item on the item-stack will be returned.
 */
int items_display(struct map_t* map,int x,int y){

  //TODO

  return 0;
}
