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

//Main
int main(int argc, char** argv){
  //Setup
  srand(time(NULL));
  initscr();
  color_init();
  cbreak();
  noecho();
  keypad(stdscr, true);
  curs_set(0);
  tile_data_init();

  //Variables
  int player_x=0, player_y=0;
  int cmd;
  map_t* cur_map;
  cur_map = (map_t*)Calloc(1,sizeof(map_t));
  map_init(cur_map, TERMINAL_WIDTH, TERMINAL_HEIGHT-3);
  //map_draw_rect(cur_map,1,1,cur_map->width-2, cur_map->height-2,TILE_FLOOR);
  //map_draw_borders(cur_map);
  
  //Setup player stats
  set_health(100);
  set_encumberment(0);
  set_level(1);
  set_dlevel(0);
  set_name("Lan");
  char welcome_msg[81];
  sprintf(welcome_msg, "Welcome to the game, %s!", get_name());
  msg_add(welcome_msg);

  //Place character randomly
  map_draw_random_rooms(cur_map);
  map_draw_borders(cur_map);
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

    cmd = getch();
    if(cmd == KEY_UP){
      plr_mv_to_y--;
    }
    else if(cmd == KEY_DOWN){
      plr_mv_to_y++;
    }
    else if(cmd == KEY_LEFT){
      plr_mv_to_x--;
    }
    else if(cmd == KEY_RIGHT){
      plr_mv_to_x++;
    }
    else if(cmd == KEY_HOME){ //Upper Left of keypad
      plr_mv_to_x--;
      plr_mv_to_y--;
    }
    else if(cmd == KEY_PPAGE){ //Upper Right of keypad
      plr_mv_to_x++;
      plr_mv_to_y--;
    }
    else if(cmd == KEY_END){ //Lower Left of keypad
      plr_mv_to_x--;
      plr_mv_to_y++;
    }
    else if(cmd == KEY_NPAGE){ //Lower Right of keypad
      plr_mv_to_x++;
      plr_mv_to_y++;
    }
    if(plr_mv_to_x >=0 && plr_mv_to_x<cur_map->width && plr_mv_to_y>=0 && plr_mv_to_y<cur_map->height){
      if(tile_data[cur_map->tiles[plr_mv_to_y*cur_map->width+plr_mv_to_x]].passable){
	player_x=plr_mv_to_x;
	player_y=plr_mv_to_y;
      }
    } 
  }

  endwin();
  return 0;
}
