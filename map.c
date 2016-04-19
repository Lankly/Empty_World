#include <curses.h>
#include <math.h>
#include <stdlib.h>
#include "colors.h"
#include "creature.h"
#include "map.h"
#include "helpers.h"
#include "creature.h"
#include "status.h"
#include "tiles.h"

struct map_t{
  int dlevel;
  int width;
  int height;
  int max_item_height;
  int *tiles;
  item_map_t *items;
  clist_t *creatures;
  item_map_t *known_items;
  struct map_t *known_map;
};

map_t *map_new(){
  return Calloc(1, sizeof(struct map_t));
}

/* Initializes a given map. Expects the map to have already been allocated.
 */
void map_init(struct map_t* map, int w, int h, int max_item_height, int dlevel){
  if(map == NULL){quit("Error: Cannot initialize NULL Map");}
  map->width = w;
  map->height= h;
  map->dlevel = dlevel;
  map->max_item_height = max_item_height;
  map->tiles = (int*)Calloc(w * h,sizeof(int));
}

clist_t *map_get_creatures(map_t *map){
  return map == NULL ? NULL : map->creatures;
}

int map_get_dlevel(map_t *map){
  return map == NULL ? -1 : map->dlevel;
}

item_map_t *map_get_items(map_t *map){
  return map == NULL ? NULL : map->items;
}

int map_get_max_item_height(map_t *map){
  return map == NULL ? -1 : map->max_item_height;
}

/* This function adds a creature to a given map.
 */
void map_add_creature(struct map_t *map, creature_t *creature){
  if(map == NULL){quit("Error: Cannot add Creature to NULL Map");}
  if(creature == NULL){quit("Error: Cannot add NULL Creature to Map");}

  //If there are no creatures yet, set them up
  if(map->creatures == NULL){
    map->creatures = clist_new(creature);
  }
  //)Otherwise, just add the creature
  else{
    clist_add(map->creatures, clist_new(creature));
  }
}

/* This function returns a char representing the item at the given index on
 * the item stack at the given coordinates in the given map.
 */
int map_get_item_symbol(map_t* map, int x, int y, int index){
  if(get_coord(x, y, map->width) > map->width*map->height){
    quit("Error: Items Index Out of Bounds");
  }

  //Navigate to the correct item-stack on the map
  item_map_t *cur = NULL;
  for(item_map_t *i = map->items; i != NULL; i = i->next){
    if(i->y > y || (i->y == y && i->x > x)){
      break;}
    if(i->y == y && i->x == x){
      cur = i;}
  }
  if(cur==NULL){return -1;}

  //Navigate to the correct item on the stack
  itemlist_t* cur_node = cur->first;
  for(int i=0;i<index;i++){
    cur_node=cur_node->next;
    if(cur==NULL){quit("Error: Stack Index out of bounds");}
  }
  //Return the symbol from here
  char to_return = cur_node->item->display;
  return to_return;  
}

int map_get_top_symbol_helper(item_map_t *items, int x, int y){
  if(items == NULL){
    return 0;
  }
  if(items->x == x && items->y == y){
    return get_top_item_sym_from_stack(items);
  }
  return map_get_top_symbol_helper(items->next, x, y);
}
/* This function returns the top item on the item stack at the given coordinates
 * If it could not be found, returns a NULL-terminator.
 */
int map_get_top_symbol(map_t *map, int x, int y){
  return map_get_top_symbol_helper(map->items, x, y);
}

int map_get_tile(struct map_t* map, int x, int y){
  if(map == NULL){
    endwin();
    printf("Error: Map is null\n");
    exit(1);
  }
  else if(x<0 || y<0 || x >= map->width || y >= map->height){
    return TILE_UNKNOWN;
  }
  return map->tiles[y*map->width+x];
}

int map_get_height(map_t *map){
  return map == NULL ? 0 : map->height;
}

int map_get_width(map_t *map){
  return map == NULL ? 0 : map->width;
}

/* This function removes a creature from a given map. Returns the removed 
 * creature.
 */
creature_t *map_remove_creature(map_t *map, creature_t *creature){
  if(map == NULL){
    return NULL;
  }
  return clist_remove_by_creature(map->creatures, creature);
}

/* Adds an item to the given map at the given coordinates. Reveal tell us
 * if we are adding this to the list of known items or not. Returns false
 * if the item could not be added.
 */
bool map_add_item(map_t* map, int x, int y, item_t* item, bool reveal){
  if(map == NULL){
    return false;
  }
  int h = map_get_height(map),
    w = map_get_width(map);
  if(item->size > map_get_max_item_height(map)
     || x < 0 || y < 0 ||
     get_coord(x, y, w) > w*h){
    return false;
  }

  //Pick which map we're operating on, based on reveal
  item_map_t *m = reveal ? map->known_items : map->items;
  
  //Case where no items exist yet
  if(m == NULL){
    m = Calloc(1, sizeof(item_map_t));
    item_map_init(m, x, y);
    if(reveal){
      map->known_items = m;
    }
    else{
      map->items = m;
    }
  }

  //Case where need to insert new item_map_t at the beginning
  if(m->y > y || (m->y == y && m->x > x)){
    item_map_t *new = Calloc(1, sizeof(item_map_t));
    item_map_init(new, x, y);
    new->next = m;
    m = new;
    if(reveal){
      map->known_items = m;
    }
    else{
      map->items = m;
    }
  }
  
  //Now we loop through to find where to insert our data
  //Data is sorted left->right, up->down
  for(item_map_t *cur = m; cur != NULL; cur = cur->next){
    //If we've found the correct location, add it
    if(cur->y == y && cur->x == x){
      item_map_add(cur, item);
      return true;
    }
    /* If it goes in the next spot, add it
     * Conditions for next spot:
     * 1 - We're at the end of the list
     * 2 - It goes in-between cur and the next node
     */
    if(cur->next == NULL
       ||
       (cur->next != NULL
	&& (cur->next->y > y
	    || (cur->next->y == y && cur->next->x > x)))){
      
      item_map_t *add = Calloc(1, sizeof(item_map_t));
      item_map_init(add, x, y);
      item_map_add(add, item);
      add->next = cur->next;
      cur->next = add;
      return true;
    }
  }

  //If we somehow get here, then we didn't add anything, so return false
  return false;
}

void map_set_tile(struct map_t* map, int x, int y, int tile){
  if(map == NULL){
    endwin();
    printf("Error: Map is null\n");
    exit(1);
  }
  else if(x>=map->width || y>=map->height || x<0 || y<0){
    endwin();
    printf("Error: Index out of bounds (set)\n");
    exit(1);
  }
  else if(tile>TILE_MAX || tile<0){
    endwin();
    printf("Error: Tile nonexistant\n");
    exit(1);
  }

  map->tiles[y*map->width+x] = tile;
}

void map_draw_rect(struct map_t* map, int x, int y, int w, int h, int tile){
  if(map == NULL){
    quit("Error: NULL map");
  }

  if(x < 0){x = 1;}
  else if(x + w >= TERMINAL_WIDTH){
    w = TERMINAL_WIDTH-x-1;
  }
  if(y < 0){y = 1;}
  else if(y + h >= TERMINAL_HEIGHT){
    h = TERMINAL_HEIGHT-y-1;
  }

  for(int i = y; i < y+h; i++){
    for(int j = x; j < x+w; j++){
      map->tiles[i*map->width+j]=tile;
    }
  }
}

/* Takes in a map and draws borders around all its rooms
 */
void map_draw_borders(struct map_t* map){
  if(map==NULL){
    quit("Error: NULL Map\n");
  }
  //Loop through the map and add walls next to floors
  for(int j = 0; j < map->height; j++){
    for(int i = 0; i < map->width; i++){
      //Don't want to replace corridors or floors, so only edit on blank tiles
      if(map_get_tile(map, i, j) == TILE_UNKNOWN){
	
	//Check each direciton for a floor
	if(map_get_tile(map, i+1, j) == TILE_FLOOR
	   || map_get_tile(map, i+1, j+1) == TILE_FLOOR
	   || map_get_tile(map, i+1, j-1) == TILE_FLOOR
	   || map_get_tile(map, i, j+1) == TILE_FLOOR
	   || map_get_tile(map, i, j-1) == TILE_FLOOR
	   || map_get_tile(map, i-1, j+1) == TILE_FLOOR
	   || map_get_tile(map, i-1, j) == TILE_FLOOR
	   || map_get_tile(map, i-1, j-1) == TILE_FLOOR){
	  //Set this to a wall tile
	  map->tiles[j*map->width+i] = TILE_WALL;
	}
      }
    }
  }
}

/* This function places a random number of randomly-sized rooms onto the map,
 * with a guaranteed position of x and y, unless x and y are negative.
 */
void map_draw_random_rooms(struct map_t* map, int x, int y){
  //Assumes empty map
  int stop = rand()%7+3; //min 3 rooms, max 10
  int prevw=-1, prevh=-1, prevx=-1, prevy=-1;
  for(int i=0; i<stop; i++){
    int randw=(10-i)*(rand()%3+1)+3;
    int randh=rand()%(10-i)+3;
    int randx=rand()%(map->width-randw-3)+1;
    int randy=rand()%(map->height-randh-3)+1;

    /* If an initial position was given, we cannot advance until the first
     * room generated contains the given position.
     */
    bool good = true;
    if(i == 0 && x != -1 && y != -1
       && ((randx > x || (randx + randw) <= x)
	   || (randy > y || (randy + randh) <= y)
	   )){
      good = false;
      i--;
    }
    else{
      map_draw_rect(map, randx, randy, randw, randh, TILE_FLOOR);
    }
    //draw corridors
    if(good && prevw!=-1){
      int prevmidx= prevx+(prevw/2);
      int prevmidy= prevy+(prevh/2);
      int curmidx = randx+(randw/2);
      int curmidy = randy+(randh/2);

      int startx=prevmidx, starty=prevmidy;
      bool done=false;
      while(!done){
	
	if(startx==curmidx && starty==curmidy){
	  done=true;
	}//If the path is aligned with the middle of the target
	else if(startx==curmidx){starty   < curmidy ? starty++ : starty--;}
	else if(starty==curmidy){prevmidx < curmidx ? startx++ : startx--;}
	else{//Otherwise
	  //Work on the x-axis for a step
	  if(rand()%2 == 0 
	     || map_get_tile(map,startx+1,starty) == TILE_CORRIDOR){
	    startx   < curmidx ? startx++ : startx--;
	  }
	  else{//Or work on the y-axis for a step
	    starty < curmidy ? starty++ : starty--; 
	  }
	}

	if(!done && map_get_tile(map,startx,starty)==TILE_UNKNOWN){
	  map_set_tile(map,startx,starty,TILE_CORRIDOR);

	  //Try to move along pre-existing corridors
	  if(startx==curmidx && starty>curmidy){//If corridor is going strictly up
	    if(map_get_tile(map,startx,starty-1)==TILE_CORRIDOR){
	      starty--;
	    }
	    else if(map_get_tile(map,startx-1,starty-1)==TILE_CORRIDOR){
	      startx--;starty--;
	    }
	    else if(map_get_tile(map,startx+1,starty-1)==TILE_CORRIDOR){
	      startx++;starty--;
	    }
	  }
	  else if(startx==curmidx && starty<curmidy){//If corridor is going strictly down
	    if(map_get_tile(map,startx,curmidy+1)==TILE_CORRIDOR){
	      starty++;
	    }
	    else if(map_get_tile(map,startx-1,starty+1)==TILE_CORRIDOR){
	      startx--;starty++;
	    }
	    else if(map_get_tile(map,startx+1,starty+1)==TILE_CORRIDOR){
	      startx++;starty++;
	    }
	  }
	  else if(starty==curmidy && startx>curmidx){//If corridor is going strictly left
	    if(map_get_tile(map,startx-1,starty)==TILE_CORRIDOR){
	      startx--;
	    }
	    else if(map_get_tile(map,startx-1,starty-1)==TILE_CORRIDOR){
	      startx--;starty--;
	    }
	    else if(map_get_tile(map,startx-1,starty+1)==TILE_CORRIDOR){
	      startx--;starty++;
	    }
	  }
	  else if(starty==curmidy && startx<curmidx){//If corridor is going strictly right
	    if(map_get_tile(map,startx+1,starty)==TILE_CORRIDOR){
	      startx++;
	    }
	    else if(map_get_tile(map,startx+1,starty-1)==TILE_CORRIDOR){
	      startx++;starty--;
	    }
	    else if(map_get_tile(map,startx+1,starty+1)==TILE_CORRIDOR){
	      startx++;starty++;
	    }
	  }
	  else if(startx<curmidx && starty<curmidy){//If corridor is going right and down
	    if(map_get_tile(map,startx+1,starty)==TILE_CORRIDOR){
	      startx++;
	    }
	    else if(map_get_tile(map,startx,starty+1)==TILE_CORRIDOR){
	      starty++;
	    }
	  }
	  
	}
	
      }
    }
    if(good){
      prevw=randw; prevh=randh; prevx=randx; prevy=randy;
    }
  }
}
/* This function checks whether a given coordinate on the map contains a
 * down or an up stair.*/
bool tile_has_stair(struct map_t* map, int x, int y){
  if(map == NULL){quit("Error: Cannot check NULL Map");}
  for(struct item_map_t* items = map->items;
      items != NULL;
      items = items->next){
    if(items->x == x && items->y == y){
      for(struct itemlist_t* i = items->first;
	  items != NULL;
	  items = items->next){
	if(i->item->id == ITEM_UP_STAIR
	   || i->item->id == ITEM_DOWN_STAIR){
	  return true;
	}
      }
      return false;
    }
  }
  return false;
}

void map_place_up_stair(struct map_t* map, 
			int x, 
			int y,
			struct map_t* go_to_map){
  if(map == NULL){
    quit("Error: Cannot place Up-Stair on NULL Map");
  }
  item_t* stair = item_create_from_data(ITEM_UP_STAIR);
  stair->go_to_map = go_to_map;
  map_add_item(map, x, y, stair, false);
}

void map_place_down_stair_randomly(struct map_t* map){
  if(map == NULL){
    quit("Error: Cannot place Down-Stair on NULL Map");
  }
  bool done = false;
  int c = 0;
  while(!done){
    c++;
    int x = rand()%map->width;
    int y = rand()%map->height;

    if(map_get_tile(map, x, y) == TILE_FLOOR){
      done = true;
      //Check for nearby stairs
      for(int i = -18; i < 15; i++){
	for(int j = -18; j < 15; j++){
	  int check_x = x + i;
	  int check_y = y + j;
	  if(c < 1000 
	     && check_x >= 0 && check_x < map->width
	     && check_y >= 0 && check_y < map->height
	     && tile_has_stair(map, check_x, check_y)){
	    done = false;
	  }
	}
      }
      //If we're done, add the down stair to the map
      if(done){
	map_add_item(map, x, y, item_create_from_data(ITEM_DOWN_STAIR), false);
      }
    }
  }
}

void map_place_spawners(map_t* map){
  if(map == NULL){
    quit("Error: Cannot place spawners on NULL Map");}
  
  //We place one spawner for every ten levels, plus one
  for(int i = 0; i < ((map->dlevel/10) + 1); i++){
    creature_t *spawner = 
      creature_create_from_data(CREATURE_TYPE_SPAWNER);

    map_add_creature(map, spawner);
    creature_place_randomly_in_walls(spawner, map);

    //Each spawner is activated one hundred times when created (avg 5 creatures)
    //for(int i = 0; i < 100; i++){
    //  (*(spawner->takeTurn))(spawner, map);}
  }
}

bool map_tile_is_door(int tile){
  return (tile==TILE_DOOR_OPEN
	  || tile==TILE_DOOR_CLOSE
	  || tile==TILE_DOOR_BROKEN);
}

bool map_coord_is_door(struct map_t* map, int x, int y){
  int tile = map_get_tile(map,x,y);
  return map_tile_is_door(tile);
}


/* This function allows the player to get the description of a given tile. It 
 * first gives them a cursor to move around with and select what they want with
 * enter. Then, it checks that coordinate for a creature, then an item stack, 
 * then the tile.
 */
void map_examine_tile(map_t *map){
  if(map == NULL){quit("Error: Cannot examine NULL Map");}
  int x, y;
  creature_get_coord(player, &x, &y);
  get_coord_via_cursor(&y, &x);
  
  //First loop through creatures
  if(map->creatures != NULL){
    for(clist_t* cur = map->creatures; cur != NULL; cur = clist_next(cur)){
      creature_t *cur_creature = clist_get_creature(cur);
      if(cur_creature != NULL){
	int cur_x, cur_y;
	creature_get_coord(cur_creature, &cur_x, &cur_y);
	if(cur_x == x && cur_y == y && (in_range(cur_creature, player)
	     || creature_is_telepathic(player))){
	  creature_examine(cur_creature);
	  return;
	}
      }
    }
  }
  //Now check items
  for(struct item_map_t* cur = map->known_items;
      cur != NULL && cur->y <= y;
      cur = cur->next){
    if(cur->x == x && cur->y == y){
      msg_addf("%s", cur->first->item->exam_text);
      return;
    }
  }
  //Now the tile itself
  msg_addf("%s", tile_data[map_get_tile(map, x, y)].exam_text);
}

/* Line of sight code              *
 * this is a Boolean function      *
 * that returns FALSE if the       *
 * creature cannot see the         *
 * given point and TRUE if it can  *
 *                                 *
 * It has the monsters x and y     *
 * coords as parameters            */
bool map_tile_is_visible(map_t *map, int check_x, int check_y, creature_t *c){
  if(c == NULL){
    return false;
  }
  int c_x, c_y;
  creature_get_coord(c, &c_x, &c_y);
  if(c_x == check_x && c_y == check_y){
    return true;
  }
  //Check to see if it's in the possible radius of the creature's sight
  if(!coord_in_range(check_x, check_y, c)){
    return false;
  }
     
  int t, x, y, abs_delta_x, abs_delta_y, sign_x, sign_y, delta_x, delta_y;
  
  /* Delta x is the creature's x minus the check_x       *
   * d is my dungeon structure and px is the creature's  *
   * x position. check_x is the target x position passed *
   * to the function.                                    */
  delta_x = c_x - check_x;
  
  /* delta_y is the same as delta_x using the y coordinates */
  delta_y = c_y - check_y;
  
  /* abs_delta_x & abs_delta_y: these are the absolute values of delta_x & delta_y */
  abs_delta_x = abs(delta_x);
  abs_delta_y = abs(delta_y);
  
  /* sign_x & sign_y: these are the signs of delta_x & delta_y */
  sign_x = delta_x < 0 ? -1 : 1;
  sign_y = delta_y < 0 ? -1 : 1;
  
  /* x & y: these are the monster's x & y coords */
  x = check_x;
  y = check_y;
  
  /* The following if statement checks to see if the line *
   * is x dominate or y dominate and loops accordingly    */
  if(abs_delta_x > abs_delta_y){
    /* X dominate loop */
    /* t = twice the absolute of y minus the absolute of x*/
    t = abs_delta_y * 2 - abs_delta_x;
    do{
      if(t >= 0){
	/* if t is greater than or equal to zero then *
	 * add the sign of delta_y to y                    *
	 * subtract twice the absolute of delta_x from t   */
	y += sign_y;
	t -= abs_delta_x*2;
      }
      
      /* add the sign of delta_x to x      *
       * add twice the adsolute of delta_y to t  */
      x += sign_x;
      t += abs_delta_y * 2;
      
      /* check to see if we are at the creature's position */
      if(x == c_x && y == c_y){
	/* return that the monster can see the creature */
	return true;
      }
      /* keep looping until the monster's sight is blocked *
       * by an object at the updated x,y coord             */
    }while(tile_data[map_get_tile(map, x, y)].transparent);
    
    /* the loop was exited because the monster's sight was blocked *
     * return FALSE: the monster cannot see the point              */
    return false;
  }
  else
    {
      /* Y dominate loop, this loop is basically the same as the x loop */
      t = abs_delta_x * 2 - abs_delta_y;
      do{
	if(t >= 0){
	  x += sign_x;
	  t -= abs_delta_y * 2;
	}
	y += sign_y;
	t += abs_delta_x * 2;
	if(x == c_x && y == c_y){
	  return true;
	}
      }while(tile_data[map_get_tile(map, x, y)].transparent);
      return false;
   }
}

/* Returns true if the tile at coordinates x and y contains an item that stops
 * the player's quick-movement. Passing out-of-bounds coordinates returns false.
 */
bool map_tile_stopme(struct map_t *map, int x, int y){
  if(map == NULL){quit("Error: Cannot examine NULL Map");}
  if(x < 0 || y < 0 || x >= TERMINAL_WIDTH || y >= TERMINAL_HEIGHT){
    return false;}
  
  //Find the item stack at the given coordinates
  item_map_t *i = map->items;
  while(i != NULL && (i->x < x || i->y < y)){
    i = i->next;}
  if(i == NULL || i->x != x || i->y != y){return false;}

  //If any item on the stack stops the player, return
  
  for(itemlist_t *cur = i->first; cur != NULL; cur = cur->next){
    if(cur->item->stopme){
      return true;}
  }
  return false;
}

/* This function uses a pair of coordinates and a radius to reveal the floor of
 * a given map.
 */
void map_reveal(map_t *map, int rev_dist){
  //Checks to make sure everything is OK
  if(map == NULL){
    quit("Error: Cannot reveal NULL Map");
  }

  int px, py;
  creature_get_coord(player, &px, &py);
  if(px < 0 || py <0
     || px > TERMINAL_WIDTH-1 || py > TERMINAL_HEIGHT-1){
    return;
  }

  //If no map has been revealed yet, start revealing
  if(map->known_map == NULL){
    map->known_map = (struct map_t*)Calloc(1, sizeof(struct map_t));
    map_init(map->known_map,
	     map->width,
	     map->height,
	     map->max_item_height,
	     map->dlevel);
  }
  struct item_map_t *items = map->items;

  //For each point in the map,
  for(int j=0; j < map->height; j++){
    for(int i=0; i < map->width; i++){
      //Check whether or not something's in the way
      if(map_tile_is_visible(map, i, j, player)){
	//Reveal the tile
	map->known_map->tiles[get_coord(i, j, TERMINAL_WIDTH)] =
	  map->tiles[get_coord(i, j, TERMINAL_WIDTH)];
	
	//Remove items already revealed
	for(item_map_t *k_items = map->known_items;
	    k_items != NULL;
	    k_items = k_items->next){
	  if(k_items->y == j && k_items->x == i){
	    struct itemlist_t *prev;
	    while(k_items->first != NULL){
	      prev = k_items->first;
	      k_items->first = k_items->first->next;
	      free(prev);
	    }
	    k_items->size = 0;
	  }
	}
	
	//Reveal the items (if they exist)
	if(items != NULL && items->y == j && items->x == i){
	  //Add the new items
	  for(itemlist_t *item = items->first;
	      item != NULL;
	      item = item->next){
	    map_add_item(map, i, j, item->item, true);
	  }
	  items = items->next;
	}
      }
      else if(items != NULL && items->y == j && items->x == i){
	items = items->next;
      }
    }
  }
}

void map_cleanup(struct map_t* map){
  for(int y = 1; y < map->height; y++){
    for(int x = 1; x < map->width; x++){
      //Delete all unnecessary corridors
      int cur_tile = map_get_tile(map, x, y);
      if(cur_tile == TILE_CORRIDOR){
	int u = map_get_tile(map, x, y-1);
	int d = map_get_tile(map, x, y+1);
	int l = map_get_tile(map, x-1, y);
	int r = map_get_tile(map, x+1, y);
	int ul = map_get_tile(map, x-1, y-1);
	int ur = map_get_tile(map, x+1, y-1);
	int dl = map_get_tile(map, x-1, y+1);
	int dr = map_get_tile(map, x+1, y+1);
	//Catches corridors that replace sides of rooms - vertical sides
	if(d == TILE_CORRIDOR 
	   && ((r == TILE_FLOOR 
		&& dr == TILE_FLOOR 
		&& ur == TILE_FLOOR 
		&& l != TILE_CORRIDOR) 
	       || (l == TILE_FLOOR 
		   && dl == TILE_FLOOR 
		   && ul == TILE_FLOOR 
		   && r != TILE_CORRIDOR))){
	  if(r == TILE_FLOOR && l == TILE_FLOOR){
	    map_set_tile(map, x, y, TILE_FLOOR);
	  }
	  else if(map_tile_is_door(l)){
	    map_set_tile(map, x-1, y, TILE_FLOOR);
	    map_set_tile(map, x, y, TILE_FLOOR);
	  }
	  else {
	    map_set_tile(map, x, y, TILE_WALL);
	  }
	}//Catches corridors that replace sides of rooms - horizontal side
	else if(r == TILE_CORRIDOR 
		&& ((u == TILE_FLOOR && ur == TILE_FLOOR 
		     && ul == TILE_FLOOR && d  != TILE_CORRIDOR) 
		    || (d == TILE_FLOOR  && dr == TILE_FLOOR 
			&& dl == TILE_FLOOR && u != TILE_CORRIDOR))){
	  if(u == TILE_FLOOR && d == TILE_FLOOR){
	    map_set_tile(map, x, y, TILE_FLOOR);
	  }
	  else if(map_tile_is_door(u)){
	    map_set_tile(map, x, y-1,TILE_FLOOR);
	    map_set_tile(map, x, y,TILE_FLOOR);
	  }
	  else if(d != TILE_CORRIDOR){
	    map_set_tile(map, x, y, TILE_WALL);
	  }
	}//Catches too many corridors in empty space
	else if(u == TILE_CORRIDOR 
		&& ur == TILE_CORRIDOR 
		&& r == TILE_CORRIDOR 
		&& d == TILE_UNKNOWN){
	  map_set_tile(map, x, y, TILE_UNKNOWN);
	}
	//Place a door here if possible
	//(Possible=Adjacent Floor+Adjacent Corridor)
	else if((u == TILE_FLOOR 
		 || d == TILE_FLOOR 
		 || l == TILE_FLOOR 
		 || r == TILE_FLOOR) 
		&& (u == TILE_CORRIDOR 
		    || d == TILE_CORRIDOR 
		    || r == TILE_CORRIDOR 
		    || l == TILE_CORRIDOR)){
	  //If two adjacent doors, replace with Floor
	  if(map_tile_is_door(l)){
	    if(u != TILE_CORRIDOR){
	      map_set_tile(map, x-1, y, TILE_FLOOR);
	      map_set_tile(map, x, y, TILE_FLOOR);
	    }
	    else{
	      map_set_tile(map, x-1, y, TILE_WALL);
	      map_set_tile(map, x, y, TILE_DOOR_CLOSE);
	    }
	  }
	  else if(map_tile_is_door(u)){
	    if(l != TILE_CORRIDOR){
	      map_set_tile(map, x, y-1, TILE_FLOOR);
	      map_set_tile(map, x, y, TILE_FLOOR);
	    }
	    else{
	      map_set_tile(map, x, y-1, TILE_WALL);
	      map_set_tile(map, x, y, TILE_DOOR_CLOSE);
	    }
	  }
	  else if((l == TILE_FLOOR 
		   && u == TILE_FLOOR) 
		  || (u == TILE_FLOOR 
		      && r == TILE_FLOOR) 
		  || (r == TILE_FLOOR 
		      && d == TILE_FLOOR) 
		  || (d == TILE_FLOOR 
		      && l == TILE_FLOOR)){
	    map_set_tile(map, x, y, TILE_FLOOR);
	  }
	  else if(rand()%3 == 0){
	    map_set_tile(map, x, y, TILE_DOOR_OPEN);
	  }
	  else if(rand()%50 == 0){
	    map_set_tile(map, x, y, TILE_DOOR_BROKEN);
	  }
	  else{map_set_tile(map, x, y, TILE_DOOR_CLOSE);}
	}
	//Replace double doors (Distinct from Two Adjacent Doors)
	else if(map_tile_is_door(u) && d == TILE_FLOOR){
	  if(ur != TILE_CORRIDOR && ul != TILE_CORRIDOR){
	    map_set_tile(map, x, y-1, TILE_FLOOR);//Changed from WALL to FLOOR
	  }
	  //if((ur==TILE_FLOOR && ul==TILE_CORRIDOR)
	  //|| (ur==TILE_CORRIDOR && ul==TILE_FLOOR)){
	  map_set_tile(map, x, y, TILE_FLOOR);//Changed from WALL to FLOOR
	    //}
	}
	else if(map_tile_is_door(l) && r == TILE_FLOOR){
	  if(ul != TILE_CORRIDOR && dl != TILE_CORRIDOR){
	    map_set_tile(map, x-1, y, TILE_FLOOR);
	  }
	  //if((ul==TILE_FLOOR && dl==TILE_CORRIDOR)
	  //|| (ul==TILE_CORRIDOR && dl==TILE_FLOOR)
	  //|| (ul==TILE_WALL && dl==TILE_WALL)){
	    map_set_tile(map, x, y, TILE_FLOOR);
	    //}
	}
	//Fix corridor blobs
	else if(r == TILE_CORRIDOR
		&& d == TILE_CORRIDOR
		&& dr == TILE_CORRIDOR){
	  if(u == TILE_FLOOR || ur == TILE_FLOOR){
	    map_set_tile(map, x, y, TILE_FLOOR);
	    map_set_tile(map, x+1, y, TILE_FLOOR);
	  }
	  else if(u == TILE_UNKNOWN && ur == TILE_UNKNOWN){
	    if(l != TILE_CORRIDOR){
	      map_set_tile(map, x, y, TILE_UNKNOWN);}
	    if(map_get_tile(map, x+2, y) != TILE_CORRIDOR){
	      map_set_tile(map, x+1, y, TILE_UNKNOWN);}
	  }
	}
	else if(d==TILE_CORRIDOR && l==TILE_CORRIDOR && dl==TILE_CORRIDOR){
	   if(r==TILE_FLOOR){map_set_tile(map,x,y,TILE_WALL);}
	}
	//Fix abandoned corridors
	else if(u != TILE_CORRIDOR
		&& d != TILE_CORRIDOR
		&& r != TILE_CORRIDOR
		&& l != TILE_CORRIDOR){
	  if((l == TILE_FLOOR && r == TILE_FLOOR)
	     || (u == TILE_FLOOR && d == TILE_FLOOR)){
	    map_set_tile(map, x, y, TILE_FLOOR);
	  }
	  else if(map_tile_is_door(u)){
	    if(ur!=TILE_CORRIDOR && ul!=TILE_CORRIDOR){
	      map_set_tile(map,x,y-1,TILE_WALL);
	    }
	    if((ur == TILE_FLOOR && ul == TILE_CORRIDOR)
	       || (ur == TILE_CORRIDOR && ul == TILE_FLOOR)){
	      map_set_tile(map, x, y, TILE_WALL);
	    }
	  }
	  else if(map_tile_is_door(r)){
	    if(ur!=TILE_CORRIDOR && dr!=TILE_CORRIDOR){
	      map_set_tile(map,x+1,y,TILE_WALL);
	    }
	    if((ur==TILE_FLOOR && dr==TILE_CORRIDOR) || (ur==TILE_CORRIDOR && dr==TILE_FLOOR)){
	      map_set_tile(map,x,y,TILE_WALL);	    
	    }
	  }
	  else if(map_tile_is_door(d)){
	    if(dl!=TILE_CORRIDOR && dr!=TILE_CORRIDOR && map_get_tile(map,x,y+2)!=TILE_CORRIDOR){
	      map_set_tile(map,x,y+1,TILE_WALL);
	    }
	    map_set_tile(map,x,y,TILE_WALL);
	  }
	  else if(map_tile_is_door(l)){
	    if(ul!=TILE_CORRIDOR && dl!=TILE_CORRIDOR && map_get_tile(map,x-2,y)!=TILE_CORRIDOR){
	      map_set_tile(map,x-1,y,TILE_WALL);
	    }
	    map_set_tile(map,x,y,TILE_WALL);
	  }
	  else{
	    map_set_tile(map,x,y,TILE_WALL);
	  }
	}
      }
      //Fix improper doors
      cur_tile = map_get_tile(map, x, y);
      if(map_tile_is_door(cur_tile)){
	int u = map_get_tile(map, x, y-1);
	int d = map_get_tile(map, x, y+1);
	int l = map_get_tile(map, x-1, y);
	int r = map_get_tile(map, x+1, y);
	int ul = map_get_tile(map, x-1, y-1);
	int ur = map_get_tile(map, x+1, y-1);
	int dl = map_get_tile(map, x-1, y+1);
	int dr = map_get_tile(map, x+1, y+1);
	if(u != TILE_CORRIDOR 
	   && d != TILE_CORRIDOR 
	   && r != TILE_CORRIDOR 
	   && l != TILE_CORRIDOR){
	  if((u == TILE_FLOOR && d == TILE_FLOOR) 
	     || (l == TILE_FLOOR && r == TILE_FLOOR)){
	    map_set_tile(map, x, y, TILE_FLOOR);
	  }
	  else{
	    map_set_tile(map, x, y, TILE_WALL);
	  }
	}
	else if(u == TILE_CORRIDOR 
		&& r == TILE_CORRIDOR 
		&& ur == TILE_CORRIDOR){
	  map_set_tile(map, x, y-1, TILE_WALL);
	}
	else if(r == TILE_CORRIDOR 
		&& d == TILE_CORRIDOR 
		&& dr == TILE_CORRIDOR){
	  map_set_tile(map, x+1, y, TILE_WALL);
	}
	else if(d == TILE_CORRIDOR 
		&& l == TILE_CORRIDOR 
		&& dl == TILE_CORRIDOR){
	  map_set_tile(map, x, y+1, TILE_WALL);
	}
	else if(l == TILE_CORRIDOR 
		&& u == TILE_CORRIDOR 
		&& ul == TILE_CORRIDOR){
	  map_set_tile(map, x-1, y, TILE_WALL);
	}
	else if(map_tile_is_door(ul) 
		&& (l == TILE_CORRIDOR || u == TILE_CORRIDOR)){
	  map_set_tile(map, x, y, TILE_WALL);
	}
	else if(map_tile_is_door(ur) 
		&& (r == TILE_CORRIDOR || u == TILE_CORRIDOR)){
	  map_set_tile(map, x, y, TILE_WALL);
	}
	else if(map_tile_is_door(dl) 
		&& (l == TILE_CORRIDOR || d == TILE_CORRIDOR)){
	  map_set_tile(map, x, y, TILE_WALL);
	}
	else if(map_tile_is_door(dr) 
		&& (r == TILE_CORRIDOR || d == TILE_CORRIDOR)){
	  map_set_tile(map, x, y, TILE_WALL);
	}
	else if(u == TILE_CORRIDOR && ur == TILE_FLOOR){
	  map_set_tile(map, x, y-1, cur_tile);
	  map_set_tile(map, x, y, TILE_WALL);
	}
	
      }

    }
  }
}

/* This function returns the proper ASCII value for a wall tile in a given
 * location.
 */
int wall_correct(struct map_t *m, int i, int j){
  int r = map_get_tile(m, i+1, j);
  int l = map_get_tile(m, i-1, j);
  int u = map_get_tile(m, i, j-1);
  int d = map_get_tile(m, i, j+1);
  int ur = map_get_tile(m, i+1, j-1);
  int ul = map_get_tile(m, i-1, j-1);
  int dr = map_get_tile(m, i+1, j+1);
  int dl = map_get_tile(m, i-1, j+1);
  
  //Three sides
  if((u == TILE_WALL || map_tile_is_door(u))
     && (d == TILE_WALL || map_tile_is_door(d))
     && (l == TILE_WALL || map_tile_is_door(l))
     && ((tile_data[ul].passable && tile_data[dl].passable)
	 || (tile_data[r].passable && tile_data[ul].passable)
	 || (tile_data[r].passable && tile_data[dl].passable))){
    return ACS_RTEE;
  }
  if((u == TILE_WALL || map_tile_is_door(u))
     && (d == TILE_WALL || map_tile_is_door(d))
     && (r == TILE_WALL || map_tile_is_door(r))
     && ((tile_data[ur].passable && tile_data[dr].passable)
	 || (tile_data[l].passable && tile_data[ur].passable)
	 || (tile_data[l].passable && tile_data[dr].passable))){
    return ACS_LTEE;
  }
  if((u == TILE_WALL || map_tile_is_door(u))
     && (r == TILE_WALL || map_tile_is_door(r))
     && (l == TILE_WALL || map_tile_is_door(l))
     && ((tile_data[ur].passable && tile_data[ul].passable)
	 || (tile_data[d].passable && tile_data[ur].passable)
	 || (tile_data[d].passable && tile_data[ul].passable))){
    return ACS_BTEE;
  }
  if((d == TILE_WALL || map_tile_is_door(d))
     && (r == TILE_WALL || map_tile_is_door(r))
     && (l == TILE_WALL || map_tile_is_door(l))
     && ((tile_data[dr].passable && tile_data[dl].passable)
	 || (tile_data[u].passable && tile_data[dr].passable)
	 || (tile_data[u].passable && tile_data[dl].passable))){
    return ACS_TTEE;
  }
  //Two sides
  if((u == TILE_WALL || map_tile_is_door(u)) 
     && (l == TILE_WALL || map_tile_is_door(l))
     && ((d != TILE_WALL && r != TILE_WALL && dr != TILE_WALL)
	 || tile_data[ul].passable)){
    return ACS_LRCORNER;
  }
  if((u == TILE_WALL || map_tile_is_door(u)) 
     && (r == TILE_WALL || map_tile_is_door(r))
     && ((d != TILE_WALL && l != TILE_WALL && dl != TILE_WALL)
	 || tile_data[ur].passable)){
    return ACS_LLCORNER;
  }
  if((d == TILE_WALL || map_tile_is_door(d))
     && (l == TILE_WALL || map_tile_is_door(l))
     && ((u != TILE_WALL && r != TILE_WALL && ur != TILE_WALL)
	 || tile_data[dl].passable)){
    return ACS_URCORNER;
  }
  if((d == TILE_WALL || map_tile_is_door(d))
     && (r == TILE_WALL || map_tile_is_door(r))
     && ((u != TILE_WALL && l != TILE_WALL && ul != TILE_WALL)
	 || tile_data[dr].passable)){
    return ACS_ULCORNER;
  }
  //One side
  if((u == TILE_WALL || map_tile_is_door(u)
      || d == TILE_WALL || map_tile_is_door(d))
     && (tile_data[r].passable || tile_data[l].passable)){
    return ACS_VLINE;
  }
  if((r == TILE_WALL || map_tile_is_door(r) 
      || l == TILE_WALL || map_tile_is_door(l))
     && (tile_data[u].passable || tile_data[d].passable)){
    return ACS_HLINE;
  }
  return ACS_PLUS;
}

void draw_map(struct map_t* map){
  if(map == NULL){quit("Error: Cannot draw NULL Map");}
  struct map_t* m = map->known_map ? map->known_map : map;
  
  for(int j = 0; j < m->height; j++){
    move(j, 0);
    for(int i = 0; i < m->width; i++){
      int tile = map_get_tile(m,i,j);
      int display = tile_data[tile].display;
      
      //Wall correct
      if(display == ACS_PLUS){
	display = wall_correct(m, i, j);
      }

      //Color correct
      int color = 0;
      if(!use_8_colors && tile_data[tile].display_color != 0){
	color = tile_data[tile].display_color;
      }
      else if(use_8_colors && tile_data[tile].display_color_alt != 0){
	color = tile_data[tile].display_color_alt;
      }
      
      addch(display | COLOR_PAIR(map_tile_is_visible(map, i, j, player) ? color
		       : use_8_colors ? CP_BLUE_BLACK 
				 : CP_DARK_GREY_BLACK));
    }
  }
  
  //Now items
  for(struct item_map_t *known = map->known_items;
      known != NULL; 
      known = known->next){
    if(known != NULL && known->first != NULL){
      mvaddch(known->y, known->x, known->first->item->display);}
  }

  //Now creatures
  for(clist_t* cur = map_get_creatures(map);
      cur != NULL; cur = clist_next(cur)){
    creature_t *cur_creature = clist_get_creature(cur);
    if(cur_creature != NULL){
      int cur_x, cur_y;
      creature_get_coord(cur_creature, &cur_x, &cur_y);
      
      //Show creature if it's visible to the player
      if(creature_is_telepathic(player)
	 || (creature_is_visible(cur_creature, player))){
	mvaddch(cur_y, cur_x, creature_get_display(cur_creature));}
    }
  }
}

void debug_reveal_map(map_t *map){
  if(map != NULL){
    map->known_map = map;
  }
}
