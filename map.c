#include "map.h"
#include "helpers.h"
#include <curses.h>
#include <stdlib.h>
#include "tiles.h"

void map_init(map_t* map, int w, int h){
  map->width = w;
  map->height= h;
  map->tiles = (int*)Calloc(w * h,sizeof(int));
}

void map_set_tile(map_t* map, int x, int y, int tile){
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

int map_get_tile(map_t* map, int x, int y){
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

void map_draw_rect(map_t* map, int x, int y, int w, int h, int tile){
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

void map_draw_borders(map_t* map){
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
	if(chk_x<map->width && map->tiles[chk_y*map->width+chk_x]==TILE_FLOOR){
	  map->tiles[j*map->width+i]=TILE_WALL;
	  done=true;
	}//Check right, down
	chk_y++;
	if(!done && chk_x<map->width && chk_y<map->height && map->tiles[chk_y*map->width+chk_x]==TILE_FLOOR){
	  map->tiles[j*map->width+i]=TILE_WALL;
	  done=true;
	}//Check down
	chk_x--;
	if(!done && chk_x<map->width && chk_y<map->height && map->tiles[chk_y*map->width+chk_x]==TILE_FLOOR){
	  map->tiles[j*map->width+i]=TILE_WALL;
	  done=true;
	}//Check down, left
	chk_x--;
	if(!done && chk_x<map->width && chk_y<map->height && map->tiles[chk_y*map->width+chk_x]==TILE_FLOOR){
	  map->tiles[j*map->width+i]=TILE_WALL;
	  done=true;
	}//Check left
	chk_y--;
	if(!done && chk_x<map->width && chk_y<map->height && map->tiles[chk_y*map->width+chk_x]==TILE_FLOOR){
	  map->tiles[j*map->width+i]=TILE_WALL;
	  done=true;
	}//Check up,left
	chk_y--;
	if(!done && chk_x<map->width && chk_y<map->height && map->tiles[chk_y*map->width+chk_x]==TILE_FLOOR){
	  map->tiles[j*map->width+i]=TILE_WALL;
	  done=true;
	}//Check up
	chk_x++;
	if(!done && chk_x<map->width && chk_y<map->height && map->tiles[chk_y*map->width+chk_x]==TILE_FLOOR){
	  map->tiles[j*map->width+i]=TILE_WALL;
	  done=true;
	}//Check up, right
	chk_x++;
	if(!done && chk_x<map->width && chk_y<map->height && map->tiles[chk_y*map->width+chk_x]==TILE_FLOOR){
	  map->tiles[j*map->width+i]=TILE_WALL;
	}
      }
    }
  }
}

void map_draw_random_rooms(map_t* map){
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
	  if(rand()%2 == 0 || map_get_tile(map,startx+1,starty)==TILE_CORRIDOR){//Work on the x-axis for a step
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

bool map_coord_is_door(map_t* map, int x, int y){
  int tile = map_get_tile(map,x,y);
  return (tile==TILE_DOOR_OPEN || tile==TILE_DOOR_CLOSE || tile==TILE_DOOR_BROKEN);
}

bool map_tile_is_door(int tile){
  return (tile==TILE_DOOR_OPEN || tile==TILE_DOOR_CLOSE || tile==TILE_DOOR_BROKEN);
}

void map_cleanup(map_t* map){
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
	if(d==TILE_CORRIDOR && ((r==TILE_FLOOR && dr==TILE_FLOOR && ur==TILE_FLOOR && l!=TILE_CORRIDOR) || (l==TILE_FLOOR && dl==TILE_FLOOR && ul==TILE_FLOOR && r!=TILE_CORRIDOR))){
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
	else if(r==TILE_CORRIDOR && ((u==TILE_FLOOR && ur==TILE_FLOOR && ul==TILE_FLOOR && d!=TILE_CORRIDOR) || (d==TILE_FLOOR && dr==TILE_FLOOR && dl==TILE_FLOOR && u!=TILE_CORRIDOR))){
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
	else if(u==TILE_CORRIDOR && ur==TILE_CORRIDOR && r==TILE_CORRIDOR && d==TILE_UNKNOWN){
	  map_set_tile(map,x,y,TILE_UNKNOWN);
	}
	//Place a door here if possible (Possible=Adjacent Floor+Adjacent Corridor)
	else if((u==TILE_FLOOR || d==TILE_FLOOR || l==TILE_FLOOR || r==TILE_FLOOR) && (u==TILE_CORRIDOR || d==TILE_CORRIDOR || r==TILE_CORRIDOR || l==TILE_CORRIDOR)){
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
	  else if((l==TILE_FLOOR && u==TILE_FLOOR) || (u==TILE_FLOOR && r==TILE_FLOOR) || (r==TILE_FLOOR && d==TILE_FLOOR) || (d==TILE_FLOOR && l==TILE_FLOOR)){map_set_tile(map,x,y,TILE_FLOOR);}
	  else if(rand()%3==0){map_set_tile(map,x,y,TILE_DOOR_OPEN);}
	  else if(rand()%50==0){map_set_tile(map,x,y,TILE_DOOR_BROKEN);}
	  else{map_set_tile(map,x,y,TILE_DOOR_CLOSE);}
	}
	//Replace double doors (Distinct from Two Adjacent Doors)
	else if(map_tile_is_door(u) && d==TILE_FLOOR){
	  if(ur!=TILE_CORRIDOR && ul!=TILE_CORRIDOR){
	    map_set_tile(map,x,y-1,TILE_WALL);
	  }
	  //if((ur==TILE_FLOOR && ul==TILE_CORRIDOR) || (ur==TILE_CORRIDOR && ul==TILE_FLOOR)){
	    map_set_tile(map,x,y,TILE_WALL);
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
