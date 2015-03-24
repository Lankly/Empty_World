#include "helpers.h"
#include "colors.h"
#include "creature.h"
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

void analyze_cmd(int cmd, int* x, int* y){
  if(cmd == KEY_UP){*y-=1;
  }else if(cmd == KEY_DOWN){*y+=1;
  }else if(cmd == KEY_LEFT){*x-=1;
  }else if(cmd == KEY_RIGHT){*x+=1;
  }else if(cmd == KEY_HOME){ //Upper Left of keypad
    *x-=1;*y-=1;
  }else if(cmd == KEY_PPAGE){ //Upper Right of keypad
    *x+=1;*y-=1;
  }else if(cmd == KEY_END){ //Lower Left of keypad
    *x-=1;*y+=1;
  }else if(cmd == KEY_NPAGE){ //Lower Right of keypad
    *x+=1;*y+=1;
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
}
