#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <time.h>
#include "helpers.h"
#include "map.h"
#include "tiles.h"
#include "colors.h"
#include "status.h"

#define TERMINAL_WIDTH  80
#define TERMINAL_HEIGHT 24

#define PASS_WEIGHT 50

bool qckmv_continue(map_t* map,int x, int y, int qckmv_cmd){
  int cur_tile = map->tiles[y*map->width+x];
  int ul=map->tiles[(y-1)*map->width+x-1];
  int ur=map->tiles[(y-1)*map->width+x+1];
  int dl=map->tiles[(y+1)*map->width+x-1];
  int dr=map->tiles[(y+1)*map->width+x+1];
  int u=map->tiles[(y-1)*map->width+x];
  int d=map->tiles[(y+1)*map->width+x];
  int l=map->tiles[y*map->width+x-1];
  int r=map->tiles[y*map->width+x+1];
  //Check for at corner in corridor
  if(cur_tile == TILE_CORRIDOR){
    if(qckmv_cmd==KEY_UP && (ul==TILE_CORRIDOR || ur==TILE_CORRIDOR) ){return false;}
    if(qckmv_cmd==KEY_DOWN && (dl==TILE_CORRIDOR || dr==TILE_CORRIDOR) ){return false;}
    if(qckmv_cmd==KEY_LEFT && (ul==TILE_CORRIDOR || dl==TILE_CORRIDOR) ){return false;}
    if(qckmv_cmd==KEY_RIGHT && (ur==TILE_CORRIDOR || dr==TILE_CORRIDOR) ){return false;}
  }
  //Check for change in tile type during quickmove
  if(qckmv_cmd==KEY_UP    && (u!=cur_tile || tile_data[ul].stopme || tile_data[ur].stopme)){return false;}
  if(qckmv_cmd==KEY_DOWN  && (d!=cur_tile || tile_data[dl].stopme || tile_data[dr].stopme)){return false;}
  if(qckmv_cmd==KEY_LEFT  && (l!=cur_tile || tile_data[dl].stopme || tile_data[ul].stopme)){return false;}
  if(qckmv_cmd==KEY_RIGHT && (r!=cur_tile || tile_data[dr].stopme || tile_data[ur].stopme)){return false;}
  if(qckmv_cmd==KEY_HOME && ul!=cur_tile){return false;}
  if(qckmv_cmd==KEY_PPAGE && ur!=cur_tile){return false;}
  if(qckmv_cmd==KEY_END && dl!=cur_tile){return false;}
  if(qckmv_cmd==KEY_NPAGE && dr!=cur_tile){return false;}
  return true;
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

//Main
int main(int argc, char** argv){
  //Setup
  int seed = time(NULL);
  printf("Seed: %d\n", seed);
  srand(seed);
  //srand();
  //KNOWN TESTING SEEDS
  //1401917882
  initscr();
  color_init();
  cbreak();
  noecho();
  keypad(stdscr, true);
  curs_set(0);
  tile_data_init();

  //Variables
  int player_x=0, player_y=0, cmd, qckmv_cmd=0;
  bool qckmv=false; //Quick-Move
  map_t* cur_map;
  cur_map = (map_t*)Calloc(1,sizeof(map_t));
  map_init(cur_map, TERMINAL_WIDTH, TERMINAL_HEIGHT-3);
  
  //Setup player stats
  set_health(100);
  set_encumberment(0);
  set_level(1);
  set_dlevel(0);
  set_name("Lan");
  char welcome_msg[81];
  sprintf(welcome_msg, "Welcome to the game, %s!", get_name());
  msg_add(welcome_msg);

  //Setup floor
  map_draw_random_rooms(cur_map);
  map_cleanup(cur_map);
  map_draw_borders(cur_map);

  //Place character randomly
  while(cur_map->tiles[player_y*cur_map->width+player_x]!=TILE_FLOOR){
    player_x=rand() % cur_map->width;
    player_y=rand() % cur_map->height;
  }

  //Main Game Loop
  while(true){
    for(int j=0; j<cur_map->height; j++){
      move(j,0);
      for(int i=0; i<cur_map->width; i++){
	addch(tile_data[map_get_tile(cur_map,i,j)].display);
      }
    }
    mvaddch(player_y,player_x,'@' | COLOR_PAIR(CP_YELLOW_BLACK));
    draw_status();
    refresh();
    
    int plr_mv_to_x=player_x, plr_mv_to_y=player_y;
    
    //If the player is quickmoving, keep doing that. Else, look for keyboard input.
    if(qckmv){cmd = qckmv_cmd;}
    else{cmd = getch();}

    analyze_cmd(cmd, &plr_mv_to_x, &plr_mv_to_y);
    if(cmd == KEY_B2){//5 on keypad (qkmv command)
      qckmv_cmd=getch();
      if(qckmv_cmd==KEY_HOME || qckmv_cmd==KEY_UP || qckmv_cmd==KEY_PPAGE || qckmv_cmd==KEY_LEFT || qckmv_cmd==KEY_RIGHT || qckmv_cmd==KEY_END || qckmv_cmd==KEY_DOWN || qckmv_cmd==KEY_NPAGE){
	qckmv=true;
	analyze_cmd(qckmv_cmd, &plr_mv_to_x, &plr_mv_to_y);
      }
    }//Open command
    else if(cmd=='o'){
      int open_cmd=getch();
      int open_x=player_x,open_y=player_y;
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
      }else if(open_x==player_x && open_y==player_y){
	msg_add("Invalid direction.");
      }else{msg_add("That cannot be opened.");}
    }//Close command for doors
    else if(cmd=='C'){
      int close_cmd=getch();
      int close_x=player_x,close_y=player_y;
      analyze_cmd(close_cmd,&close_x,&close_y);

      int ctile=map_get_tile(cur_map,close_x,close_y);
      if(tile_data[ctile].openable){
	if(ctile==TILE_DOOR_OPEN){
	  map_set_tile(cur_map,close_x,close_y,TILE_DOOR_CLOSE);
	}else if(ctile==TILE_DOOR_CLOSE){
	  msg_add("That door is already closed.");
	}else if(ctile==TILE_DOOR_BROKEN){
	  msg_add("That door is broken");
	}
      }else{msg_add("That cannot be closed.");}
    }else if(cmd=='~'){//Debug
      int debug_cmd=getch();
      bool edit=false;
      int edit_tile=TILE_UNKNOWN;
      if(debug_cmd=='w'){edit=true; edit_tile=TILE_WALL;}
      else if(debug_cmd=='f'){edit=true;edit_tile=TILE_FLOOR;}
      else if(debug_cmd=='W'){
	int inc_cmd=getch(); 
	if(inc_cmd=='+'){
	  add_weight(10);
	  msg_add("10 weight added.");
	}else if(inc_cmd=='-'){
	  add_weight(-10);
	  msg_add("10 weight removed.");
	}
      }
      else if(debug_cmd=='x'){//Display coordinates
	char* coord = (char*)Calloc(15,sizeof(char));
	sprintf(coord,"X:%d,Y:%d",player_x,player_y);
	msg_add(coord);
	free(coord);
      }
      if(edit){
	int dir_cmd=getch();
	if(dir_cmd==KEY_HOME){map_set_tile(cur_map,player_x-1,player_y-1,edit_tile);}
	else if(dir_cmd==KEY_UP){map_set_tile(cur_map,player_x,player_y-1,edit_tile);}
	else if(dir_cmd==KEY_PPAGE){map_set_tile(cur_map,player_x+1,player_y-1,edit_tile);}
	else if(dir_cmd==KEY_LEFT){map_set_tile(cur_map,player_x-1,player_y,edit_tile);}
	else if(dir_cmd==KEY_RIGHT){map_set_tile(cur_map,player_x+1,player_y,edit_tile);}
	else if(dir_cmd==KEY_END){map_set_tile(cur_map,player_x-1,player_y+1,edit_tile);}
	else if(dir_cmd==KEY_DOWN){map_set_tile(cur_map,player_x,player_y+1,edit_tile);}
	else if(dir_cmd==KEY_NPAGE){map_set_tile(cur_map,player_x+1,player_y+1,edit_tile);}
      }
    }
    
    //Ensure that the player is light enough to pass through corners, that they are not behind a closed door
    if(plr_mv_to_x >=0 && plr_mv_to_x<cur_map->width && plr_mv_to_y>=0 && plr_mv_to_y<cur_map->height){
      tile_t move_tile = tile_data[cur_map->tiles[plr_mv_to_y*cur_map->width+plr_mv_to_x]];
      if(move_tile.passable){
	tile_t u = tile_data[cur_map->tiles[(player_y-1)*cur_map->width+player_x]];
	tile_t d = tile_data[cur_map->tiles[(player_y+1)*cur_map->width+player_x]];
	tile_t r = tile_data[cur_map->tiles[player_y*cur_map->width+player_x+1]];
	tile_t l = tile_data[cur_map->tiles[player_y*cur_map->width+player_x-1]];
	if(get_weight()>PASS_WEIGHT && ((cmd==KEY_HOME && !u.passable && !l.passable) || (cmd==KEY_PPAGE && !u.passable && !r.passable) || (cmd==KEY_END && !d.passable && !l.passable) || (cmd==KEY_NPAGE && !d.passable && !r.passable))){ msg_add("You are too heavy to pass through.");}
	else if(!(cmd==KEY_HOME && (tile_id(u)==TILE_DOOR_CLOSE || tile_id(l)==TILE_DOOR_CLOSE)) && !(cmd==KEY_PPAGE && (tile_id(u)==TILE_DOOR_CLOSE || tile_id(r)==TILE_DOOR_CLOSE)) && !(cmd==KEY_END && (tile_id(d)==TILE_DOOR_CLOSE || tile_id(l)==TILE_DOOR_CLOSE)) && !(cmd==KEY_NPAGE && (tile_id(d)==TILE_DOOR_CLOSE || tile_id(r)==TILE_DOOR_CLOSE))){
	  player_x=plr_mv_to_x;
	  player_y=plr_mv_to_y;
	} 
        else{msg_add("You cannot pass through a closed door.");}
      }else{qckmv=false;}
    }
    if(qckmv){qckmv=qckmv_continue(cur_map, plr_mv_to_x, plr_mv_to_y, qckmv_cmd);}
  }

  endwin();
  return 0;
}
