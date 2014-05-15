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
    printf("Error: Index out of bounds (get)\n");
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
	}
	else if(startx==curmidx){
	  starty < curmidy ? starty++ : starty--; 
	}
	else if(starty==curmidy){
	  prevmidx < curmidx ? startx++ : startx--;
	}
	else{
	  if(rand()%2 == 0){//Work on the x-axis for a step
	    startx < curmidx ? startx++ : startx--;
	  }
	  else{//Work on the y-axis for a step
	  prevmidy < curmidy ? starty++ : starty--; 
	  }
	}

	if(!done && map->tiles[starty*map->width+startx]==TILE_UNKNOWN){
	  map->tiles[starty*map->width+startx]=TILE_CORRIDOR;
	}
      }
    }
    prevw=randw; prevh=randh; prevx=randx; prevy=randy;
  }
}
