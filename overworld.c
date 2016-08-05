
#include <curses.h>
#include <math.h>
#include <stdlib.h>
#include "colors.h"
#include "creature.h"
#include "helpers.h"
#include "items.h"
#include "overworld.h"
#include "tiles.h"

/********************
 * CHUNKLIST STRUCT *
 ********************/
typedef struct chunklist_t{
  //coordinates
  int x, y, z;
  int *map;
  
  //Next chunk in list
  struct chunklist_t *next;
}chunklist_t;

/**********************
 * USEFUL DEFINITIONS *
 **********************/
#define OVERWORLD_CHUNK_WIDTH 1024
#define SOIL_DEPTH 5

/*********************
 * Private Variables *
 *********************/
clist_t *creatures;
int wastes_width, wastes_height;
int *wastes_map;
chunklist_t *overworld;

/******************************
 * HELPER FUNCTION PROTOTYPES *
 ******************************/
void draw_overworld_wastes();
void generate_overworld_chunk_at_coord(int x, int y, int z);
int get_3d_coord(int x, int y, int z, int width, int height);
chunklist_t *get_chunk_at_coord(int x, int y, int z);
bool set_chunklist_at_coord(int x, int y, int z, chunklist_t *chunk);
  
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

  //Generate the first chunk of the Overworld
  generate_overworld_chunk_at_coord(0,0,0);
}

void draw_overworld(){
  if(on_overworld_wastes){
    draw_overworld_wastes();
    return;
  }
}

void draw_overworld_coord(int x, int y, int z){
  
}

/* GETTERS AND SETTERS */

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


/*****************************
 * WASTES-SPECIFIC FUNCTIONS *
 *****************************/

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


/********************************
 * OVERWORLD-SPECIFIC FUNCTIONS *
 ********************************/

//Returns the average height -1, used for determining water level
int water_level(int *heightmap, int length){
  int x = 0;
  int y = 0;
  
  for(int i = 0; i < length; i++){
    x += heightmap[i] / length;
    int b = heightmap[i] % length;

    if(y >= length - b){
      x++;
      y -= length - b;
    }
    else{
      y += b;
    }
  }

  return y - 1;
}

int *gen_heightmap(int dimension){
  int *hm = Calloc(dimension * dimension, sizeof(int));

  for(int j = 0; j < dimension; j++){
    for(int i = 0; i < dimension, i++){
      int coord = get_coord(i, j, dimension);
      hm[coord] = 100;
    }
  }
}

/* Generates a chunk at the specified coordinate of the overworld. Coordinates
 * are not the kind that items, creatures, etc use, but rather where the chunk
 * being generated fits into everything.
 */
void generate_overworld_chunk_at_coord(int x, int y, int z){
  int side = OVERWORLD_CHUNK_WIDTH;
  int square = side * side;
  int cube = square * side;
  int *heightmap = gen_heightmap(side);
  int water_lvl = water_lvl(heightmap, square);

  chunklist_t *new_chunk = Calloc(1, sizeof(chunklist_t));
  int *new_map = Calloc(cube, sizeof(int));
  new_chunk->map = new_map;
  new_chunk->x = x;
  new_chunk->y = y;
  new_chunk->z = z;

  //Fill in overworld from heightmap
  for(int j = 0; j < side; j++){
    for(int i = 0; i < side; i++){
      int coord = get_coord(i, j, side);
      int height = heightmap[coord];

      for(int k = 0; k < side; k++){
        int coord3d = get_3d_coord(i, j, k, side, side);
        int tile_to_place = TILE_AIR;
        
        if(k <= height){
          //Place dirt for the inital SOIL_DEPTH amount of land
          if((height - k) < SOIL_DEPTH){
            //Or sand if underwater
            if(k <= water_lvl){
              tile_to_place = get_random_sand_tile();
            }
            else{
              tile_to_place = TILE_DIRT;
            }
          }
          //Stone below the soil/sand
          else{
            tile_to_place = TILE_STONE;
          }
        }
        //Above the height on the heightmap
        else if(k <= water_lvl){
          tile_to_place = TILE_WATER;
        }

        new_map[coord3d] = tile_to_place;
      }
    }
  }

  set_chunklist_at_coord(new_chunk, x, y, z);
  free(heightmap);
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

int get_3d_coord(int x, int y, int z, int width, int height){
  return x + (y * width) + (z * width * height);
}

/* CHUNKLIST HELPER FUNCTIONS */

/* Returns the chunk at the given coordinate. If no such chunk exists, returns
 * NULL. If it returned a chunk, that chunk gets pushed to the top of the list,
 * making subsequent requests for it faster.
 */
chunklist_t *get_chunk_at_coord(int x, int y, int z){
  chunklist_t *failed_to_find = NULL;
  if(overworld == NULL){
    return failed_to_find;
  }
  
  //If it's at the start, simply return it
  if(overworld->x == x && overworld->y == y && overworld->z == z){
    return overworld;
  }
  
  for(chunklist_t *cur = overworld; cur->next != NULL; cur = cur->next){
    if(cur->next->x == x && cur->next->y == y && cur->next->z == z){
      //Put at front of list, for easy access next time, if it's soon
      chunklist_t *found = cur->next;
      cur->next = cur->next->next;
      found->next = overworld;
      overworld = found;
      return found;
    }
  }

  return failed_to_find;
}

/* Updates or sets the chunk at the given coordinate. Returns true if successful
 */
bool set_chunklist_at_coord(chunklist_t *chunk, int x, int y, int z){
  for(chunklist_t *cur = overworld; cur != NULL; cur = cur->next){
    //If chunk already exists, update it
    if(cur->x == chunk->x && cur->y == chunk->y && cur->z == chunk->z){
      cur->map = map;
      return true;
    }
    //If we've reached the end, place it at the start
    if(cur->next == NULL){
      chunk->next = overworld;
      overworld = chunk;
      return true;
    }
  }
  
  return false;
}
