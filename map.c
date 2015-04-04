#include <curses.h>
#include <math.h>
#include <stdlib.h>
#include "creature.h"
#include "map.h"
#include "helpers.h"
#include "creature.h"
#include "player.h"
#include "status.h"
#include "tiles.h"


/* Initializes a given map. Expects the map to have already been allocated.
 */
void map_init(struct map_t* map, int w, int h, int max_item_height){
  if(map == NULL){quit("Error: Cannot initialize NULL Map");}
  map->width = w;
  map->height= h;
  map->max_item_height = max_item_height;
  map->tiles = (int*)Calloc(w * h,sizeof(int));
  map->creatures = (struct creature_list_t*)
    Calloc(1, sizeof(struct creature_list_t));
}

/* This function adds a creature to a given map.
 */
void map_add_creature(struct map_t* map, struct creature_t* creature){
  if(map == NULL){quit("Error: Cannot add Creature to NULL Map");}
  if(creature == NULL){quit("Error: Cannot add NULL Creature to Map");}
  if(map->creatures == NULL){
    map->creatures = (struct creature_list_t*)
      Calloc(1, sizeof(struct creature_list_t));
  }

  struct creature_list_node_t* to_add = (struct creature_list_node_t*)
    Calloc(1, sizeof(creature_list_node_t));
  to_add->creature = creature;
  to_add->next = map->creatures->first;
  map->creatures->first = to_add;
}

/* This function removes a creature from a given map.
 */
void map_remove_creature(struct map_t* map, struct creature_t* creature){
  if(map == NULL || map->creatures == NULL 
     || map->creatures->first == NULL || creature == NULL){return;}
  
  struct creature_list_node_t* cur = map->creatures->first;
  //Need to check if the thing we're removing is first in the list
  if(cur->creature == creature){
    map->creatures->first = cur->next;
    free(cur); return;
  }
  
  //Now we check everything else
  for(; cur->next != NULL; cur = cur->next){
    if(cur->next->creature == creature){
      creature_list_node_t* temp = cur->next;
      cur->next = temp->next;
      free(temp);
    }
  }
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

int map_get_tile(struct map_t* map, int x, int y){
  if(map == NULL){
    endwin();
    printf("Error: Map is null\n");
    exit(1);
  }
  else if(x<0 || y<0 || x>=map->width || y>=map->height){
    endwin();
    printf("Error: Index out of bounds (get %d,%d)\n",x,y);
    exit(1);
  }
  return map->tiles[y*map->width+x];
}

void map_draw_rect(struct map_t* map, int x, int y, int w, int h, int tile){
  if(map == NULL){
    endwin();
    printf("Error: Map is null\n");
    exit(1);
  }
  else if(x<0 || y<0 || x>=map->width || y>=map->height){
    endwin();
    printf("Error: Index out of bounds (draw)\n");
    exit(1);
  }
  else if(w<0 || h<0 || (x+w)>map->width || (y+h)>map->height ){
    endwin();
    printf("Error: Rectangle out of bounds\n");
    exit(1);
  }
  else if(tile<0 || tile>TILE_MAX){
    endwin();
    printf("Error: Tile nonexistant\n");
    exit(1);
  }

  for(int i=y;i<y+h;i++){
    for(int j=x;j<x+w;j++){
      map->tiles[i*map->width+j]=tile;
    }
  }
}

void map_draw_borders(struct map_t* map){
  if(map==NULL){
    endwin();
    printf("Error: Map nonexistant\n");
    exit(1);
  }
  for(int j=0; j<map->height; j++){
    for(int i=0; i<map->width; i++){
      if(map->tiles[j*map->width+i]==TILE_UNKNOWN){
	int chk_x=i+1,chk_y=j;
	bool done=false;
	
	//Check right
	if(chk_x<map->width 
	   && map->tiles[chk_y*map->width+chk_x] == TILE_FLOOR){
	  map->tiles[j*map->width+i]=TILE_WALL;
	  done=true;
	}//Check right, down
	chk_y++;
	if(!done && chk_x<map->width 
	   && chk_y<map->height 
	   && map->tiles[chk_y*map->width+chk_x] == TILE_FLOOR){
	  map->tiles[j*map->width+i] = TILE_WALL;
	  done=true;
	}//Check down
	chk_x--;
	if(!done && chk_x<map->width 
	   && chk_y<map->height 
	   && map->tiles[chk_y*map->width+chk_x] == TILE_FLOOR){
	  map->tiles[j*map->width+i] = TILE_WALL;
	  done=true;
	}//Check down, left
	chk_x--;
	if(!done && chk_x<map->width 
	   && chk_y<map->height 
	   && map->tiles[chk_y*map->width+chk_x] == TILE_FLOOR){
	  map->tiles[j*map->width+i] = TILE_WALL;
	  done=true;
	}//Check left
	chk_y--;
	if(!done && chk_x<map->width 
	   && chk_y<map->height 
	   && map->tiles[chk_y*map->width+chk_x] == TILE_FLOOR){
	  map->tiles[j*map->width+i] = TILE_WALL;
	  done=true;
	}//Check up,left
	chk_y--;
	if(!done && chk_x<map->width 
	   && chk_y<map->height 
	   && map->tiles[chk_y*map->width+chk_x] == TILE_FLOOR){
	  map->tiles[j*map->width+i] = TILE_WALL;
	  done=true;
	}//Check up
	chk_x++;
	if(!done && chk_x<map->width 
	   && chk_y<map->height 
	   && map->tiles[chk_y*map->width+chk_x]==TILE_FLOOR){
	  map->tiles[j*map->width+i] = TILE_WALL;
	  done=true;
	}//Check up, right
	chk_x++;
	if(!done 
	   && chk_x<map->width 
	   && chk_y<map->height 
	   && map->tiles[chk_y*map->width+chk_x] == TILE_FLOOR){
	  map->tiles[j*map->width+i] = TILE_WALL;
	}
      }
    }
  }
}

void map_draw_random_rooms(struct map_t* map){
  //Assumes empty map
  int stop = rand()%7+2;
  int prevw=-1, prevh=-1, prevx=-1, prevy=-1;
  for(int i=0; i<stop; i++){
    int randw=(10-i)*(rand()%3+1)+3;
    int randh=rand()%(10-i)+3;
    int randx=rand()%(map->width-randw-3)+1;
    int randy=rand()%(map->height-randh-3)+1;
    map_draw_rect(map, randx, randy, randw, randh, TILE_FLOOR);
    //draw corridors
    if(prevw!=-1){
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
    prevw=randw; prevh=randh; prevx=randx; prevy=randy;
  }
}

bool map_coord_is_door(struct map_t* map, int x, int y){
  int tile = map_get_tile(map,x,y);
  return (tile==TILE_DOOR_OPEN || tile==TILE_DOOR_CLOSE || tile==TILE_DOOR_BROKEN);
}

bool map_tile_is_door(int tile){
  return (tile==TILE_DOOR_OPEN || tile==TILE_DOOR_CLOSE || tile==TILE_DOOR_BROKEN);
}

/* This function allows the player to get the description of a given tile. It 
 * first gives them a cursor to move around with and select what they want with
 * enter. Then, it checks that coordinate for a creature, then an item stack, then
 * the tile.
 */
void map_examine_tile(struct map_t* map){
  if(map == NULL){quit("Error: Cannot examine NULL Map");}
  int x = player->x;
  int y = player->y;
  get_coord_via_cursor(&y, &x);
  
  //First loop through creatures
  if(map->creatures != NULL){
    for(struct creature_list_node_t* cur = map->creatures->first;
	cur != NULL;
	cur = cur->next){
      if(cur->creature->x == x && cur->creature->y == y){
	msg_addf("%s", cur->creature->exam_text);
	return;
      }
    }
  }
  //Now check items
  if(map->items != NULL){
    for(struct item_map_t* cur = map->items;
	cur != NULL && cur->y <= y;
	cur = cur->next){
      if(cur->x == x && cur->y == y){
	msg_addf("%s", cur->first->item->exam_text);
	return;
      }
    }
  }
  //Now the tile itself
  msg_addf("%s", tile_data[map_get_tile(map, x, y)].exam_text);
}

/* Line of sight code         *
 * this is a Boolean function *
 * that returns FALSE if the  *
 * monster cannot see the     *
 * player and TRUE if it can  *
 *                            *
 * It has the monsters x and y*
 * coords as parameters       */
bool map_tile_is_visible(struct map_t* map, int check_x, int check_y){
  int t, x, y, abs_delta_x, abs_delta_y, sign_x, sign_y, delta_x, delta_y;
  
  /* Delta x is the players x minus the monsters x    *
   * d is my dungeon structure and px is the players  *
   * x position. monster_x is the monsters x position passed *
   * to the function.                                 */
  delta_x = player->x - check_x;
  
  /* delta_y is the same as delta_x using the y coordinates */
  delta_y = player->y - check_y;
  
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
      
      /* check to see if we are at the player's position */
      if(x == player->x && y == player->y){
	/* return that the monster can see the player */
	return true;
      }
      /* keep looping until the monster's sight is blocked *
       * by an object at the updated x,y coord             */
    }while(tile_data[map_get_tile(map, x, y)].transparent);
    
    /* the loop was exited because the monster's sight was blocked *
     * return FALSE: the monster cannot see the player             */
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
	if(x == player->x && y == player->y){
	  return true;
	}
      }while(tile_data[map_get_tile(map, x, y)].transparent);
      return false;
   }
}

/* This function uses a pair of coordinates and a radius to reveal the floor of
 * a given map.
 */
void map_reveal(struct map_t* map, int x, int y, int rev_dist){
  if(map == NULL){quit("Error: Cannot reveal NULL Map");}
  if(x < 0 || y <0 || x > TERMINAL_WIDTH-1 || y > TERMINAL_HEIGHT-1){
    return;
  }

  //If no map has been revealed yet, start revealing
  if(map->known_map == NULL){
    map->known_map = (struct map_t*)Calloc(1, sizeof(struct map_t));
    map_init(map->known_map, map->width, map->height, map->max_item_height);
  }

  //For each point in the map,
  for(int j=0; j < map->height; j++){
    for(int i=0; i < TERMINAL_WIDTH; i++){
      //Check to see if it's in the possible radius to reveal a tile
      int dist = sqrt(pow(( (double)(i-x) ), 2) + pow(( (double)(j-y) ), 2));
      //If it is, also check whether or not something's in the way
      if(rev_dist - dist >= 0
	 && map_tile_is_visible(map, i, j)){
	map->known_map->tiles[get_coord(i, j, TERMINAL_WIDTH)] =
	  map->tiles[get_coord(i, j, TERMINAL_WIDTH)];
      }
    }
  }
}

void map_cleanup(struct map_t* map){
  for(int y=1; y<map->height; y++){
    for(int x=1; x<map->width; x++){
      //Delete all unnecessary corridors
      int cur_tile=map_get_tile(map,x,y);
      if(cur_tile==TILE_CORRIDOR){
	int u=map_get_tile(map,x,y-1);
	int d=map_get_tile(map,x,y+1);
	int l=map_get_tile(map,x-1,y);
	int r=map_get_tile(map,x+1,y);
	int ul=map_get_tile(map,x-1,y-1);
	int ur=map_get_tile(map,x+1,y-1);
	int dl=map_get_tile(map,x-1,y+1);
	int dr=map_get_tile(map,x+1,y+1);
	//Catches corridors that replace sides of rooms - vertical sides
	if(d==TILE_CORRIDOR 
	   && ((r==TILE_FLOOR 
		&& dr==TILE_FLOOR 
		&& ur==TILE_FLOOR 
		&& l!=TILE_CORRIDOR) 
	       || (l==TILE_FLOOR 
		   && dl==TILE_FLOOR 
		   && ul==TILE_FLOOR 
		   && r!=TILE_CORRIDOR))){
	  if(r==TILE_FLOOR && l==TILE_FLOOR){
	    map_set_tile(map,x,y,TILE_FLOOR);
	  }
	  else if(map_tile_is_door(l)){
	    map_set_tile(map,x-1,y,TILE_FLOOR);
	    map_set_tile(map,x,y,TILE_FLOOR);
	  }
	  else {
	    map_set_tile(map,x,y,TILE_WALL);
	  }
	}//Catches corridors that replace sides of rooms - horizontal side
	else if(r==TILE_CORRIDOR 
		&& ((u==TILE_FLOOR 
		     && ur==TILE_FLOOR 
		     && ul==TILE_FLOOR 
		     && d!=TILE_CORRIDOR) 
		    || (d==TILE_FLOOR 
			&& dr==TILE_FLOOR 
			&& dl==TILE_FLOOR 
			&& u!=TILE_CORRIDOR))){
	  if(u==TILE_FLOOR && d==TILE_FLOOR){
	    map_set_tile(map,x,y,TILE_FLOOR);
	  }
	  else if(map_tile_is_door(u)){
	    map_set_tile(map,x,y-1,TILE_FLOOR);
	    map_set_tile(map,x,y,TILE_FLOOR);
	  }
	  else if(d!=TILE_CORRIDOR){
	    map_set_tile(map,x,y,TILE_WALL);
	  }
	}//Catches too many corridors in empty space
	else if(u==TILE_CORRIDOR 
		&& ur==TILE_CORRIDOR 
		&& r==TILE_CORRIDOR 
		&& d==TILE_UNKNOWN){
	  map_set_tile(map,x,y,TILE_UNKNOWN);
	}
	//Place a door here if possible (Possible=Adjacent Floor+Adjacent Corridor)
	else if((u==TILE_FLOOR 
		 || d==TILE_FLOOR 
		 || l==TILE_FLOOR 
		 || r==TILE_FLOOR) 
		&& (u==TILE_CORRIDOR 
		    || d==TILE_CORRIDOR 
		    || r==TILE_CORRIDOR 
		    || l==TILE_CORRIDOR)){
	  //If two adjacent doors, replace with Floor
	  if(map_tile_is_door(l)){
	    if(u!=TILE_CORRIDOR){
	      map_set_tile(map,x-1,y,TILE_FLOOR);
	      map_set_tile(map,x,y,TILE_FLOOR);
	    }
	    else{
	      map_set_tile(map,x-1,y,TILE_WALL);
	      map_set_tile(map,x,y,TILE_DOOR_CLOSE);
	    }
	  }
	  else if(map_tile_is_door(u)){
	    if(l!=TILE_CORRIDOR){
	      map_set_tile(map,x,y-1,TILE_FLOOR);
	      map_set_tile(map,x,y,TILE_FLOOR);
	    }
	    else{
	      map_set_tile(map,x,y-1,TILE_WALL);
	      map_set_tile(map,x,y,TILE_DOOR_CLOSE);
	    }
	  }
	  else if((l==TILE_FLOOR 
		   && u==TILE_FLOOR) 
		  || (u==TILE_FLOOR 
		      && r==TILE_FLOOR) 
		  || (r==TILE_FLOOR 
		      && d==TILE_FLOOR) 
		  || (d==TILE_FLOOR 
		      && l==TILE_FLOOR)){
	    map_set_tile(map,x,y,TILE_FLOOR);
	  }
	  else if(rand()%3==0){
	    map_set_tile(map,x,y,TILE_DOOR_OPEN);
	  }
	  else if(rand()%50==0){
	    map_set_tile(map,x,y,TILE_DOOR_BROKEN);
	  }
	  else{map_set_tile(map,x,y,TILE_DOOR_CLOSE);}
	}
	//Replace double doors (Distinct from Two Adjacent Doors)
	else if(map_tile_is_door(u) && d==TILE_FLOOR){
	  if(ur!=TILE_CORRIDOR && ul!=TILE_CORRIDOR){
	    map_set_tile(map,x,y-1,TILE_FLOOR);//Changed from WALL to FLOOR
	  }
	  //if((ur==TILE_FLOOR && ul==TILE_CORRIDOR) || (ur==TILE_CORRIDOR && ul==TILE_FLOOR)){
	  map_set_tile(map,x,y,TILE_FLOOR);//Changed from WALL to FLOOR
	    //}
	}
	else if(map_tile_is_door(l) && r==TILE_FLOOR){
	  if(ul!=TILE_CORRIDOR && dl!=TILE_CORRIDOR){
	    map_set_tile(map,x-1,y,TILE_FLOOR);
	  }
	  //if((ul==TILE_FLOOR && dl==TILE_CORRIDOR) || (ul==TILE_CORRIDOR && dl==TILE_FLOOR) || (ul==TILE_WALL && dl==TILE_WALL)){
	    map_set_tile(map,x,y,TILE_FLOOR);
	    //}
	}
	//Fix corridor blobs
	else if(r==TILE_CORRIDOR && d==TILE_CORRIDOR && dr==TILE_CORRIDOR){
	  if(u==TILE_FLOOR || ur==TILE_FLOOR){
	    map_set_tile(map,x,y,TILE_FLOOR);
	    map_set_tile(map,x+1,y,TILE_FLOOR);
	  }
	  else if(u==TILE_UNKNOWN && ur==TILE_UNKNOWN){
	    if(l!=TILE_CORRIDOR){map_set_tile(map,x,y,TILE_UNKNOWN);}
	    if(map_get_tile(map,x+2,y)!=TILE_CORRIDOR){map_set_tile(map,x+1,y,TILE_UNKNOWN);}
	  }
	}
	else if(d==TILE_CORRIDOR && l==TILE_CORRIDOR && dl==TILE_CORRIDOR){
	   if(r==TILE_FLOOR){map_set_tile(map,x,y,TILE_WALL);}
	}
	//Fix abandoned corridors
	else if(u!=TILE_CORRIDOR && d!=TILE_CORRIDOR && r!=TILE_CORRIDOR && l!=TILE_CORRIDOR){
	  if((l==TILE_FLOOR && r==TILE_FLOOR) || (u==TILE_FLOOR && d==TILE_FLOOR)){
	    map_set_tile(map,x,y,TILE_FLOOR);
	  }
	  else if(map_tile_is_door(u)){
	    if(ur!=TILE_CORRIDOR && ul!=TILE_CORRIDOR){
	      map_set_tile(map,x,y-1,TILE_WALL);
	    }
	    if((ur==TILE_FLOOR && ul==TILE_CORRIDOR) || (ur==TILE_CORRIDOR && ul==TILE_FLOOR)){
	      map_set_tile(map,x,y,TILE_WALL);
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
      cur_tile=map_get_tile(map,x,y);
      if(map_tile_is_door(cur_tile)){
	int u=map_get_tile(map,x,y-1);
	int d=map_get_tile(map,x,y+1);
	int l=map_get_tile(map,x-1,y);
	int r=map_get_tile(map,x+1,y);
	int ul=map_get_tile(map,x-1,y-1);
	int ur=map_get_tile(map,x+1,y-1);
	int dl=map_get_tile(map,x-1,y+1);
	int dr=map_get_tile(map,x+1,y+1);
	if(u!=TILE_CORRIDOR && d!=TILE_CORRIDOR && r!=TILE_CORRIDOR && l!=TILE_CORRIDOR){
	  if((u==TILE_FLOOR && d==TILE_FLOOR) || (l==TILE_FLOOR && r==TILE_FLOOR)){
	    map_set_tile(map,x,y,TILE_FLOOR);
	  }
	  else{
	    map_set_tile(map,x,y,TILE_WALL);
	  }
	}
	else if(u==TILE_CORRIDOR && r==TILE_CORRIDOR && ur==TILE_CORRIDOR){
	  map_set_tile(map,x,y-1,TILE_WALL);
	}
	else if(r==TILE_CORRIDOR && d==TILE_CORRIDOR && dr==TILE_CORRIDOR){
	  map_set_tile(map,x+1,y,TILE_WALL);
	}
	else if(d==TILE_CORRIDOR && l==TILE_CORRIDOR && dl==TILE_CORRIDOR){
	  map_set_tile(map,x,y+1,TILE_WALL);
	}
	else if(l==TILE_CORRIDOR && u==TILE_CORRIDOR && ul==TILE_CORRIDOR){
	  map_set_tile(map,x-1,y,TILE_WALL);
	}
	else if(map_tile_is_door(ul) && (l==TILE_CORRIDOR || u==TILE_CORRIDOR)){
	  map_set_tile(map,x,y,TILE_WALL);
	}
	else if(map_tile_is_door(ur) && (r==TILE_CORRIDOR || u==TILE_CORRIDOR)){
	  map_set_tile(map,x,y,TILE_WALL);
	}
	else if(map_tile_is_door(dl) && (l==TILE_CORRIDOR || d==TILE_CORRIDOR)){
	  map_set_tile(map,x,y,TILE_WALL);
	}
	else if(map_tile_is_door(dr) && (r==TILE_CORRIDOR || d==TILE_CORRIDOR)){
	  map_set_tile(map,x,y,TILE_WALL);
	}
	else if(u==TILE_CORRIDOR && ur==TILE_FLOOR){
	  map_set_tile(map,x,y-1,cur_tile);
	  map_set_tile(map,x,y,TILE_WALL);
	}
	
      }

    }
  }
}

void draw_map(struct map_t* map){
  if(map == NULL){quit("Error: Cannot draw NULL Map");}
  struct map_t* m = map->known_map ? map->known_map : map;
  item_map_t* items = map->items;

  for(int j=0; j<m->height; j++){
    move(j,0);
    for(int i=0; i<m->width; i++){
      //Draw top item on each item stack if there is one
      if(items != NULL 
	 && items->size != 0 
	 && items->x == i 
	 && items->y == j){
	addch(get_top_item_sym_from_stack(items));
	items=items->next;
      }
      else{addch(tile_data[map_get_tile(m,i,j)].display);}
    }
  }
  for(struct creature_list_node_t* cur = map->creatures->first;
      cur != NULL;
      cur = cur->next){
    mvaddch(cur->creature->y, cur->creature->x, cur->creature->display);
  }
}
