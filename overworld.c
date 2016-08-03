#include <curses.h>
#include <math.h>
#include <stdlib.h>
#include "colors.h"
#include "creature.h"
#include "helpers.h"
#include "items.h"
#include "overworld.h"
#include "tiles.h"

/* Private Variables */
clist_t *creatures;
int wastes_width, wastes_height;

/*Helper Functions prototypes*/
void draw_overworld_wastes();


/********************
 * PUBLIC FUNCTIONS *
 ********************/

void overworld_init(){
  overworld_set_type(OVERWORLD_TYPE_WASTES);

  //The 3 is because of the status bar
  wastes_height = TERMINAL_HEIGHT - 3;
  wastes_width = TERMINAL_WIDTH;

  //Add player
  clist_add_creature(creatures, player);
  creature_set_coord(player, wastes_width/2, wastes_height/2);
}

void draw_overworld(){
  if(on_overworld_wastes){
    draw_overworld_wastes();
    return;
  }
}

void draw_overworld_coord(int x, int y, int z){
  
}

/***********************
 * GETTERS AND SETTERS *
 ***********************/

void overworld_set_type(int type){
  switch(type){
    
  case (OVERWORLD_TYPE_WASTES):
    on_overworld_wastes = true;
    on_overworld_large = false;
    break;
    
  case (OVERWORLD_TYPE_LARGE):
    on_overworld_large = true;
    on_overworld_wastes = false;
    break;
    
  default:
    on_overworld_large = false;
    on_overworld_wastes = false;
  }
}


/********************
 * HELPER FUNCTIONS *
 ********************/

int get_random_sand_tile(){
  switch(rand() % 6){
  case 0:
  case 1:
  case 2:
    return TILE_SAND;
  case 3:
  case 4:
    return TILE_SAND_ALT1;
  case 5:
  default:
    return TILE_SAND_ALT2;
  }
}

int *wastes_map;
void wastes_map_update(){
  //Still all moving sand on the leftmost column
  for(int y = 0; y < wastes_height; y++){
    int coord = get_coord(0, y, wastes_width);
    if(wastes_map[coord] == TILE_SAND_MOVING){
      wastes_map[coord] = get_random_sand_tile();
    }
  }
  //Move all TILE_SAND_MOVING to the left
  for(int j = 0; j < wastes_height; j++){
    for(int i = 1; i < wastes_width; i++){
      int coord = get_coord(i, j, wastes_width);
      
      if(wastes_map[coord] == TILE_SAND_MOVING){
	wastes_map[get_coord(i - 1, j, wastes_width)] = TILE_SAND_MOVING;
	wastes_map[coord] = get_random_sand_tile();
      }
    }
  }
  //and make the right column completely sand
  for(int y = 0; y < wastes_height; y++){
    int coord = get_coord(wastes_width - 1, y, wastes_width);
    wastes_map[coord] = get_random_sand_tile();
  }

  //Once that's done, we need to place some sand in the new column
  for(int y = 0; y < wastes_height; y++){
    int coord = get_coord(wastes_width - 2 , y, wastes_width);
    
    if(wastes_map[coord] == TILE_SAND_MOVING){
      //We want we sand to move away from the center of the wave, so check
      //around this tile to figure out which direction to move in
      int coord_u = get_coord(wastes_width - 2 , y - 1, wastes_width);
      int coord_ul = get_coord(wastes_width - 3 , y - 1, wastes_width);
      int coord_ur = get_coord(wastes_width - 1 , y - 1, wastes_width);
      int coord_d = get_coord(wastes_width - 2 , y + 1, wastes_width);
      int coord_dl = get_coord(wastes_width - 3 , y + 1, wastes_width);
      int coord_dr = get_coord(wastes_width - 1 , y + 1, wastes_width);
      
      //Will we move up?
      if(y > 0
	 && wastes_map[coord_u] != TILE_SAND_MOVING
	 && wastes_map[coord_ul] != TILE_SAND_MOVING){
	wastes_map[coord_ur] = TILE_SAND_MOVING;
	
	//Randomly determine how long this new section will be
	int len = 1;
	while((rand() % 2) > 0 && (y - len) >= 0){
	  len++;
	  int cur_coord = get_coord(wastes_width - 1, y - len, wastes_width);
	  wastes_map[cur_coord] = TILE_SAND_MOVING;
	}
      }
      //Will we move down?
      else if(y < wastes_height - 1
	      && wastes_map[coord_d] != TILE_SAND_MOVING
	      && wastes_map[coord_dl] != TILE_SAND_MOVING){
	wastes_map[coord_dr] = TILE_SAND_MOVING;
	
	//Randomly determine how long this new section will be
	int len = 1;
	while((rand() % 2) > 0 && (y - len) >= 0){
	  len++;
	  int cur_coord = get_coord(wastes_width - 1
				    , y + len >= wastes_height
				      ? wastes_height-1
				      : y  + len
				    , wastes_width);
	  wastes_map[cur_coord] = TILE_SAND_MOVING;
	}
      }
    }
  }

  //Now figure out if we need to start a new wave
  int dist_to_next_moving_sand = wastes_width;
  for(int y = 0; y < wastes_height; y++){
    for(int x = wastes_width - 1; x >=0; x--){
      int coord = get_coord(x, y, wastes_width);
      if(wastes_map[coord] == TILE_SAND_MOVING){
	int diff = wastes_width - 1 - x;
	
	if(diff < dist_to_next_moving_sand){
	  dist_to_next_moving_sand = diff;
	}
	break;
      }
    }
  }
  int dist_between_waves = 12;
  if(dist_to_next_moving_sand >= dist_between_waves){
    int rand_wave_len = 3 + (rand() % (wastes_height / 3));

    for(int y = rand() % (wastes_height - rand_wave_len) + rand_wave_len/2;
	rand_wave_len > 0;
	rand_wave_len--){
      int coord = get_coord(wastes_width - 1, y, wastes_width);
      
      wastes_map[coord] = TILE_SAND_MOVING;
      y++;
    }
  }
}

void wastes_map_init(){
  wastes_map = Calloc(wastes_width * wastes_height, sizeof(int));
  
  // Set all to sand
  for(int j = 0; j < wastes_height; j++){
    for(int i = 0; i < wastes_width; i++){
      int coord = get_coord(i, j, wastes_width);
      
      wastes_map[coord] = TILE_SAND;
    }
  }
  // Update wastes_width times, since that generates the moving sand for us
  for(int i = 0; i < wastes_width; i++){
    wastes_map_update();
  }
}
void wastes_map_pan(){
  //Assumes player is in the overworld
  int x, y;
  creature_get_coord(player, x, y, NULL);

  if((x > 0 && x < wastes_width) || (y > 0 && y < wastes_height)){
    //No need to pan, since we keep the player's coord in-bounds
    return;
  }
}
void draw_overworld_wastes(){
  //Assumes player is in the overworld
  int x, y;
  creature_get_coord(player, x, y, NULL);
  
  if(wastes_map == NULL){
    wastes_map_init();
  }
  else if(x >= wastes_width || y >= wastes_height
          || x < 0 || y < 0 ){
    wastes_map_pan();
  }
  else{
    wastes_map_update();
  }

  int y0, x0;
  // The 3 is because of the status bar
  get_centered_box_ul_coord(&y0, &x0, wastes_height + 3, wastes_width);

  for(int j = 0; j < wastes_height; j++){
    for(int i = 0; i < wastes_width; i++){
      int coord = get_coord(i, j, wastes_width);
      int disp = tile_get_display(wastes_map[coord]);
      int cur_tile = mvinch(y0 + j, x0 + i);

      if(cur_tile != disp){
	addch(disp);
      }
    }
  }
}
