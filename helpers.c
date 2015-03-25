#include "helpers.h"
#include "colors.h"
#include "creature.h"
#include "items.h"
#include "inventory.h"
#include "map.h"
#include "player.h"
#include "status.h"
#include "tiles.h"
#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Helper Methods

void quit(const char* error_msg){
  endwin();
  printf(error_msg);
  exit(1);
}

int get_coord(int x,int y,int width){
  return y*width+x;
}

char* str_lowercase(char* str){
  char* ret = Calloc(strlen(str)+1,sizeof(char));
  for(int i=0;i<strlen(str);i++){
    ret[i]=tolower(str[i]);
  }
  return ret;
}

bool str_is_num(char* str){
  for(int i=0;i<strlen(str);i++){
    if(!isdigit(str[i])){
      return false;
    }
  }
  return true;
}

void* Calloc(int items, int size)
{
  void* ret = calloc(items, size);
  if (ret == 0)
  {
    quit("Error: out of memory\n");
  }
  return ret;
}

int display(display_list_t* list,int width,int height){
  //One padded row
  move(0,0);
  display_list_node_t* next =list->first; 
  for(int i=0;i<width;i++){addch(' ');}
  //Now for each element in the list
  for(int i=0;i<height && i<list->size;i++){
    move(i,0);addch(' ');addch('a'+i);addch(' ');
    char* str = next->data;
    for(int j=0;j<width;j++){
      if(j<strlen(str)){addch(str[j]);}
      else{addch(' ');}
    }
    next=next->next;
  }
  return getch()-'a';
}

void analyze_cmd(int cmd, int*x, int* y);
/* This function handles what to do when the open action is executed. It will
 * attempt to turn a closed door into an open door at this time. 
 * TODO: Include the ability to open items on the floor like chests.
 */
void open_tile(){
  int open_cmd=getch();
  int open_x=player->x,open_y=player->y;
  analyze_cmd(open_cmd,&open_x,&open_y);
  
  int otile=map_get_tile(cur_map,open_x,open_y);
  if(tile_data[otile].openable){
    if(otile==TILE_DOOR_CLOSE){
      map_set_tile(cur_map,open_x,open_y,TILE_DOOR_OPEN);
    }else if(otile==TILE_DOOR_OPEN){
      msg_add("That door is already open.");
    }else if(otile==TILE_DOOR_BROKEN){
      msg_add("That door is broken.");
    }
  }else if(open_x==player->x && open_y==player->y){
    msg_add("Invalid direction.");
  }else{msg_add("That cannot be opened.");}
}

/* This function handles what to do when the close action is executed. It will
 * attempt to turn an open door into a closed door at this time.
 */
void close_tile(){
  int close_cmd=getch();
  int close_x = player->x, close_y = player->y;
  analyze_cmd(close_cmd, &close_x, &close_y);
  
  int ctile = map_get_tile(cur_map, close_x, close_y);
  if(tile_data[ctile].openable){
    if(ctile == TILE_DOOR_OPEN){
      map_set_tile(cur_map, close_x, close_y, TILE_DOOR_CLOSE);
    }else if(ctile == TILE_DOOR_CLOSE){
      msg_add("That door is already closed.");
    }else if(ctile == TILE_DOOR_BROKEN){
	msg_add("That door is broken");
    }
  }else{msg_add("That cannot be closed.");}
}

/* This function handles picking an item up off of the floor.
 */
void pickup_tile(){
  int count = count_items(cur_map,player->x,player->y);
  item_t* to_add;
  /* This is the case where the player has tried to pick up an item,
   * but there is nothing on the ground  beneath them.
   */
  if(count==0){
    msg_add("No items to pick up!");
    return;
  }
  /* This is the case where the there is only one item on the ground
   * beneath the player.
   */
  else if(count==1){
    to_add = remove_item(cur_map, player->x, player->y, 0);
  }
  /* This is the case where there are multiple items on the ground 
   * beneath the player. It will take the user to a screen where they
   * can select which item it is that they're trying to pick up.
   */
  else if(count>1){
    to_add=remove_item(cur_map,
		       player->x,
		       player->y,
		       items_display(cur_map, player->x, player->y));
  }
  inventory_add(player,to_add);
}

void debug(){
  char* debug_cmd=msg_prompt("~ ");
  char* debug_cmd_lower=str_lowercase(debug_cmd);
  if(!strcmp(debug_cmd_lower,"place")){
    free(debug_cmd_lower);
    free(debug_cmd);
    char* debug_cmd=msg_prompt("Place where? ");
    char* debug_cmd_lower=str_lowercase(debug_cmd);
    int place_x=player->x;int place_y=player->y;
    if(!strcmp(debug_cmd_lower,"nw")){
      place_x=player->x-1;
      place_y=player->y-1;}
    else if(!strcmp(debug_cmd_lower,"n")){
      place_y=player->y-1;}
    else if(!strcmp(debug_cmd_lower,"ne")){
      place_x=player->x+1;
      place_y=player->y-1;}
    else if(!strcmp(debug_cmd_lower,"w")){
      place_x=player->x-1;}
    else if(!strcmp(debug_cmd_lower,"e")){
      place_x=player->x+1;}
    else if(!strcmp(debug_cmd_lower,"sw")){
      place_x=player->x-1;
      place_y=player->y+1;}
    else if(!strcmp(debug_cmd_lower,"s")){
      place_y=player->y+1;}
    else if(!strcmp(debug_cmd_lower,"se")){
      place_x=player->x+1;
      place_y=player->y+1;}
    
    free(debug_cmd_lower);
    free(debug_cmd);
    debug_cmd=msg_prompt("Place what? ");
    debug_cmd_lower=str_lowercase(debug_cmd);
    if(!strcmp(debug_cmd_lower,"tile")){
      free(debug_cmd_lower);
      free(debug_cmd);
      debug_cmd=msg_prompt("TILE ID: ");
      debug_cmd_lower=str_lowercase(debug_cmd);
      if(str_is_num(debug_cmd_lower)){
	map_set_tile(cur_map,place_x,place_y,atoi(debug_cmd)); 
	msg_add("Done");
      }
    }else if(!strcmp(debug_cmd_lower,"item")){
      free(debug_cmd_lower);
      free(debug_cmd);
      debug_cmd=msg_prompt("ITEM ID: ");
      debug_cmd_lower=str_lowercase(debug_cmd);
      if(str_is_num(debug_cmd_lower)){
	add_item(cur_map,
		 place_x,
		 place_y,
		 (struct item_t*)item_create_from_data(atoi(debug_cmd)));
	//msg_add("Done");
      }
    }
    free(debug_cmd_lower);
    free(debug_cmd);
  }
  else if(!strcmp(debug_cmd_lower,"print items")){
    endwin();
    struct item_map_t* cur_items = cur_map->items;
    for(int j=0; j<cur_map->height; j++){
      for(int i=0; i<cur_map->width; i++){
	if(cur_items!=NULL 
	   && cur_items->size!=0 
	   && cur_items->x==i 
	   && cur_items->y==j){
	  printf("%c (%d,%d) [%d,%d]\n",
		 get_top_item_sym_from_stack(cur_items),
		 cur_items->x, cur_items->y, i, j);
	  cur_items=cur_items->next;
	}
      }
    }
    exit(0);
  }
}

/* This function will process user input and make the game respond to that.
 */
void analyze_cmd(int cmd, int* x, int* y){
  if(cmd == KEY_UP){*y-=1;}
  else if(cmd == KEY_DOWN){*y+=1;}
  else if(cmd == KEY_LEFT){*x-=1;}
  else if(cmd == KEY_RIGHT){*x+=1;}
  else if(cmd == KEY_HOME){ //Upper Left of keypad
    *x-=1;*y-=1;
  }
  else if(cmd == KEY_PPAGE){ //Upper Right of keypad
    *x+=1;*y-=1;
  }
  else if(cmd == KEY_END){ //Lower Left of keypad
    *x-=1;*y+=1;
  }
  else if(cmd == KEY_NPAGE){ //Lower Right of keypad
    *x+=1;*y+=1;
  }
  else if(cmd == KEY_B2){//5 on keypad (qkmv command)
    qckmv_cmd = getch();
    if(qckmv_cmd == KEY_HOME 
       || qckmv_cmd == KEY_UP 
       || qckmv_cmd == KEY_PPAGE 
       || qckmv_cmd == KEY_LEFT 
       || qckmv_cmd == KEY_RIGHT 
       || qckmv_cmd == KEY_END 
       || qckmv_cmd == KEY_DOWN 
       || qckmv_cmd == KEY_NPAGE){
      qckmv = true;
      analyze_cmd(qckmv_cmd, x, y);
    }
  }
  else if(cmd=='o'){
    //Open command
    open_tile();
  }
  else if(cmd=='C'){
    //Close command for doors
    close_tile();
  }else if(cmd==','){
    pickup_tile();
  }else if(cmd=='i'){
    display_inventory();
  }else if(cmd=='~'){//Debug
    debug();
  }
}

bool qckmv_continue(struct map_t* map, int x, int y, int qckmv_cmd){
  int cur_tile = map->tiles[y*map->width+x];
  int ul=map->tiles[(y-1)*map->width+x-1];
  int ur=map->tiles[(y-1)*map->width+x+1];
  int dl=map->tiles[(y+1)*map->width+x-1];
  int dr=map->tiles[(y+1)*map->width+x+1];
  int u=map->tiles[(y-1)*map->width+x];
  int d=map->tiles[(y+1)*map->width+x];
  int l=map->tiles[y*map->width+x-1];
  int r=map->tiles[y*map->width+x+1];

  //Check for at-corner in corridor
  if(cur_tile == TILE_CORRIDOR){
    if(qckmv_cmd==KEY_UP 
       && (ul==TILE_CORRIDOR 
	   || ur==TILE_CORRIDOR)){
      return false;
    }
    if(qckmv_cmd==KEY_DOWN 
       && (dl==TILE_CORRIDOR 
	   || dr==TILE_CORRIDOR)){
      return false;
    }
    if(qckmv_cmd==KEY_LEFT 
       && (ul==TILE_CORRIDOR 
	   || dl==TILE_CORRIDOR)){
      return false;
    }
    if(qckmv_cmd==KEY_RIGHT 
       && (ur==TILE_CORRIDOR 
	   || dr==TILE_CORRIDOR)){
      return false;
    }
  }

  //Check for change in tile type during quickmove
  if(qckmv_cmd==KEY_UP
     && (u!=cur_tile 
	 || tile_data[ul].stopme 
	 || tile_data[ur].stopme)){
    return false;
  }
  if(qckmv_cmd==KEY_DOWN
     && (d!=cur_tile 
	 || tile_data[dl].stopme 
	 || tile_data[dr].stopme)){
    return false;
  }
  if(qckmv_cmd==KEY_LEFT
     && (l!=cur_tile 
	 || tile_data[dl].stopme 
	 || tile_data[ul].stopme)){
    return false;
  }
  if(qckmv_cmd==KEY_RIGHT
     && (r!=cur_tile 
	 || tile_data[dr].stopme 
	 || tile_data[ur].stopme)){
    return false;
  }
  if((qckmv_cmd==KEY_HOME && ul!=cur_tile)
     || (qckmv_cmd==KEY_PPAGE && ur!=cur_tile)
     || (qckmv_cmd==KEY_END && dl!=cur_tile)
     || (qckmv_cmd==KEY_NPAGE && dr!=cur_tile)){
    return false;
  }
  return true;
}

void game_init(int seed){
  initscr(); color_init(); cbreak(); noecho();
  keypad(stdscr, true); curs_set(0);  

  //Initialize game data
  tile_data_init();
  item_data_init();
  player_init();
  inventory_init(player);
  status_init();

  //Setup floor
  cur_map = (map_t*)Calloc(1,sizeof(map_t));
  map_init(cur_map, TERMINAL_WIDTH, TERMINAL_HEIGHT-3,DEFAULT_ITEMS_STACK_SIZE);
  map_draw_random_rooms(cur_map);
  map_cleanup(cur_map);
  map_draw_borders(cur_map);

  qckmv_cmd=0;
  qckmv=false;
}

void draw_map(struct map_t* map){
  item_map_t* items=map->items;

  for(int j=0; j<map->height; j++){
    move(j,0);
    for(int i=0; i<map->width; i++){
      //Draw top item on each item stack if there is one
      if(items!=NULL 
	 && items->size!=0 
	 && items->x==i 
	 && items->y==j){
	addch(get_top_item_sym_from_stack(items));
	items=items->next;
      }
      else{addch(tile_data[map_get_tile(map,i,j)].display);}
    }
  }
  mvaddch(player->y,player->x,'@' | COLOR_PAIR(CP_YELLOW_BLACK));
  draw_status();
  refresh();
}
