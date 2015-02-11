#include "items.h"
#include "status.h"
#include "map.h"
#include "helpers.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

void item_data_init(){
  item_data[ITEM_UNKNOWN]=(item_t){.id=ITEM_UNKNOWN,.exam_text="This item does not exist."};
  item_data[ITEM_IRON_SWORD]=(item_t){.id=ITEM_IRON_SWORD,.dmg_type0=DMG_BLUNT,.display=')',.exam_text="It is a plain sword made of iron.",.gold_value=5,.size=1,.material=MAT_IRON,.weight=10,.is_weapon=true};

  //    1234567890-=qwertyuiop[]\asdfghjkl;'zxcvbnm,./
  //    !@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:"ZXCVBNM<>?
  //    ±²³´µ¶·¸¹°­½ñ÷åòôùõéïðÛÝÜáóäæçèêëì»§úøãöâîí¬®¯
  //    ¡À£¤¥Þ¦ª¨©ß«Ñ×ÅÒÔÙÕÉÏÐûýüÁÓÄÆÇÈÊËÌº¢ÚØÃÖÂÎÍ¼¾¿

}

void items_map_init(item_map_t* items,int x,int y){
  items->x=x;
  items->y=y;
}

int count_items(map_t* map,int x,int y){
  if(x<0 || y<0 || get_coord(x,y,map->width)>map->width*map->height){
    quit("Error: Items Index Out of Bounds");
  }
  //We will loop through this item stack until found
  for(item_map_t* i=map->items; i!=NULL; i=i->next){
    //y first to be compatible with the main game loop
    if(i->y>y || (i->y==y && i->x>x)){break;}
    if(i->y==y && i->x==x){
      return i->size;
    }
  }
  return 0;
}

bool add_item(map_t* map,int x,int y,item_t* item){
  if(map==NULL){quit("Error: NULL map");}
  if(item->size>map->max_item_height){return false;}
  item_map_t* cur = NULL;
  if(x<0 || y<0 || get_coord(x,y,map->width)>map->width*map->height){
    quit("Error: Items Index Out of Bounds");
  }
  //Case where no items exist yet
  else if(map->items==NULL){
    item_map_t* items=(item_map_t*)Calloc(1,sizeof(item_map_t));
    items_map_init(items,x,y);
    map->items=items;
    items->first=Calloc(1,sizeof(item_map_node_t));
    items->first->item=item;
    items->size+=item->size;
    return true;
  }
  //Now we loop through to find where to insert our data
  else{
    item_map_t* last;
    for(item_map_t* i=map->items; i!=NULL; i=i->next){
      last = i;
      if(i->y > y || (i->y==y && i->x>x)){
	item_map_t* items=(item_map_t*)Calloc(1,sizeof(item_map_t));
	items_map_init(items,x,y);
	if(i->prev!=NULL){i->prev->next=items;}//Case where inserted first
	items->prev=i->prev;
	items->next=i;
	i->prev=items;
	cur=items;break;
      }
      //Linked List already exists at x,y
      else if(i->y==y && i->x==x){
	cur=i;break;
      }
    }
    //Case where it is inserted last
    if(cur==NULL){
          
      msg_addf("(%d,%d) - %d",x,y,(int)time(NULL));

      item_map_t* items=(item_map_t*)Calloc(1,sizeof(item_map_t));
      items_map_init(items,x,y);
      last->next=items;
      items->prev=last;
      cur=items;
    }
  }
  //If there is no more room in this stack or it cannot fit
  if(cur->size+item->size > map->max_item_height){
    return false;
  }
  //Finally, add the item
  item_map_node_t* insert = (item_map_node_t*)Calloc(1,sizeof(item_map_node_t));
  insert->item=item;
  insert->next=cur->first;
  cur->first=insert;
  cur->size+=item->size;
  return true;
}

item_t* remove_item(map_t* map,int x,int y,int index){
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
  cur->size-=to_return->size;
  free(cur_node);
  return to_return;
}

char get_item_sym(map_t* map,int x,int y,int index){
  if(get_coord(x,y,map->width)>map->width*map->height){
    quit("Error: Items Index Out of Bounds");
  }

  //Navigate to the correct item stack on the map
  item_map_t* cur=NULL;
  for(item_map_t* i=map->items;i!=NULL;i=i->next){
    if(i->y>y || (i->y==y && i->x>x)){break;}
    cur=i;
    if(i->y==y && i->x==x){break;}
  }
  if(cur==NULL){quit("Error: NULL item stack");}

  //Navigate to the correct item on the stack
  item_map_node_t* cur_node = cur->first;
  for(int i=0;i<index;i++){
    cur_node=cur_node->next;
    if(cur==NULL){quit("Error: Stack Index out of bounds");}
  }
  //Return the symbol from here
  char to_return=cur_node->item->display;
  return to_return;  
}

char get_top_item_sym(map_t* map,int x,int y){
  return get_item_sym(map,x,y,0);
}

char get_top_item_sym_from_stack(item_map_t* items){
  if(items==NULL){quit("Error: NULL items stack");}
  if(items->first==NULL){quit("Error: No first item");}
  return items->first->item->display;
}

item_t* item_create_from_data(int index){
  if(index<0 || index>ITEM_MAX){
    quit("Error: invalid item index");
  }

  item_t* ret = (item_t*)Calloc(1,sizeof(item_t));
  memcpy(ret,&item_data[index],sizeof(item_t));

  return ret;
}

int items_display(map_t* map,int x,int y){
  if(map==NULL){quit("Error: Null map");}
  if(map->items==NULL){quit("Error: Invalid items map");}
  if(get_coord(x,y,map->width)>map->width*map->height){
    quit("Error: Items Index Out of Bounds");
  }

  //Navigate to the correct item stack on the map
  item_map_t* cur=NULL;
  for(item_map_t* i=map->items;i!=NULL;i=i->next){
    if(i->y>y || (i->y==y && i->x>x)){break;}
    cur=i;
    if(i->y==y && i->x==x){break;}
  }
  if(cur==NULL){return -1;}

  item_map_node_t* cur_node = cur->first;
  display_list_t* items = Calloc(1,sizeof(display_list_t));
  display_list_node_t* to_add=Calloc(1,sizeof(display_list_node_t));
  to_add->data=cur_node->item->exam_text;
  items->first=to_add;
  cur_node=cur_node->next;
  while(cur_node!=NULL){
    to_add->next=Calloc(1,sizeof(display_list_node_t));
    to_add->data=cur_node->item->exam_text;
    to_add=to_add->next;
    cur_node=cur_node->next;
  }
  return display(items,map->width,map->height);
}
