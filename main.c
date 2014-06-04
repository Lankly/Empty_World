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
  //Check for change in tile type
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
  if(cmd == KEY_UP){
    *y-=1;
  }else if(cmd == KEY_DOWN){
    *y+=1;
  }else if(cmd == KEY_LEFT){
    *x-=1;
  }else if(cmd == KEY_RIGHT){
    *x+=1;
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
  //srand(1400352790);
  srand(1400358288);
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
    //clearok(stdscr, true);
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
    
    if(qckmv){
      cmd = qckmv_cmd;
    }
    else{
      cmd = getch();
    }

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
      }
      else if(open_x==player_x && open_y==player_y){
	msg_add("Invalid direction.");
      }
      else{msg_add("That cannot be opened.");}
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
      }
      else{msg_add("That cannot be closed.");}
    }
    if(plr_mv_to_x >=0 && plr_mv_to_x<cur_map->width && plr_mv_to_y>=0 && plr_mv_to_y<cur_map->height){
      if(tile_data[cur_map->tiles[plr_mv_to_y*cur_map->width+plr_mv_to_x]].passable){
	player_x=plr_mv_to_x;
	player_y=plr_mv_to_y;
      }
      else{
	qckmv=false;
      }
    }
    if(qckmv){qckmv=qckmv_continue(cur_map, plr_mv_to_x, plr_mv_to_y, qckmv_cmd);}
  }

  endwin();
  return 0;
}
