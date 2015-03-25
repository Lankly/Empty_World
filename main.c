#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <time.h>
#include "helpers.h"
#include "map.h"
#include "player.h"
#include "tiles.h"
#include "colors.h"
#include "status.h"
#include "items.h"
#include "inventory.h"

//Main
int main(int argc, char** argv){
  //Setup
  int seed = time(NULL);
  printf("Seed: %d\n", seed);
  srand(seed);
  //srand(1401917882);
  //KNOWN TESTING SEEDS
  //1401917882
  //1410214243
  //1410291444
  game_init(seed);

  //Variables
  int cmd = 0;
  
  //Place character randomly
  while(cur_map->tiles[player->y*cur_map->width+player->x]!=TILE_FLOOR){
    player->x=rand() % cur_map->width;
    player->y=rand() % cur_map->height;
  }

  //Main Game Loop
  while(true){

    draw_map(cur_map);
    
    int plr_mv_to_x = player->x,
      plr_mv_to_y = player->y;
    
    /* If the player is quickmoving, keep doing that. 
     * Else, look for keyboard input.
    */
    if(qckmv){cmd = qckmv_cmd;}
    else{cmd = getch();}

    analyze_cmd(cmd, &plr_mv_to_x, &plr_mv_to_y);
    
    
    /* Ensure that the player is light enough to pass through corners,
       that they are not behind a closed door.
     */
    if(plr_mv_to_x >=0 && plr_mv_to_x<cur_map->width 
       && plr_mv_to_y>=0 && plr_mv_to_y<cur_map->height){
      tile_t move_tile = 
	tile_data[cur_map->tiles[plr_mv_to_y*cur_map->width+plr_mv_to_x]];
      if(move_tile.passable){
	tile_t u = tile_data[cur_map->tiles[(player->y-1)*cur_map->width+player->x]];
	tile_t d = tile_data[cur_map->tiles[(player->y+1)*cur_map->width+player->x]];
	tile_t r = tile_data[cur_map->tiles[player->y*cur_map->width+player->x+1]];
	tile_t l = tile_data[cur_map->tiles[player->y*cur_map->width+player->x-1]];
	if(player->inventory->cur_weight>PASS_WEIGHT 
	   && ((cmd==KEY_HOME && !u.passable && !l.passable) 
	       || (cmd==KEY_PPAGE && !u.passable && !r.passable) 
	       || (cmd==KEY_END && !d.passable && !l.passable) 
	       || (cmd==KEY_NPAGE && !d.passable && !r.passable))){
	  msg_add("You are too heavy to pass through.");
	}
	else if(!(cmd==KEY_HOME && 
		  (tile_id(u)==TILE_DOOR_CLOSE 
		   || tile_id(l)==TILE_DOOR_CLOSE))
		&& !(cmd==KEY_PPAGE 
		     && (tile_id(u)==TILE_DOOR_CLOSE 
			 || tile_id(r)==TILE_DOOR_CLOSE)) 
		&& !(cmd==KEY_END 
		     && (tile_id(d)==TILE_DOOR_CLOSE 
			 || tile_id(l)==TILE_DOOR_CLOSE))
		&& !(cmd==KEY_NPAGE 
		     && (tile_id(d)==TILE_DOOR_CLOSE 
			 || tile_id(r)==TILE_DOOR_CLOSE))){
	  player->x=plr_mv_to_x;
	  player->y=plr_mv_to_y;
	} 
        else{msg_add("You cannot pass through a closed door.");}
      }else{qckmv=false;}
    }
    if(qckmv){qckmv=qckmv_continue(cur_map, plr_mv_to_x, plr_mv_to_y, qckmv_cmd);}
  }

  endwin();
  return 0;
}
