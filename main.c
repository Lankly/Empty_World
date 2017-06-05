#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <time.h>
#include "helpers.h"
#include "creatures.h"
#include "maps.h"
#include "tiles.h"
#include "colors.h"
#include "items.h"

//Main
int main(){
  //Setup
  int seed = time(NULL);
  printf("Seed: %d\n", seed);
  srand(seed);
  game_init(seed);

  creature_t *player = new_player();
  map_t *maps = map_new(&new_desert);
  map_add_creature(maps, player);
  
  //Main Game Loop
  while(true){

    refresh();
    
    /* Let each creature take its turn.
     */
    for(clist_t* cur = get_creatures(maps);
	cur != NULL;
	cur = ll_next(cur))
      {
	creature_t *creature = ll_elem(cur);
	if(creature != NULL){
          creature_take_turn(creature, maps);
	}
      }
  }

  endwin();
  return 0;
}
