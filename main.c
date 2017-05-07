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
int main(int argc, char** argv){
  //Setup
  int seed = time(NULL);
  printf("Seed: %d\n", seed);
  srand(seed);
  game_init(seed);
  
  //Main Game Loop
  while(true){

    refresh();
    
    /* Let each creature take its turn.
     */
    for(clist_t* cur = map_get_creatures(cur_map);
	cur != NULL;
	cur = clist_next(cur))
      {
	creature_t *creature = clist_get_creature(cur);
	if(creature != NULL){
	  //If this creature is not out of move tokens, take turn.
	  if(!creature_is_out_of_turns(creature)){
	    creature_take_turn(creature, cur_map);
	  }
	  //Otherwise, skip the turn to reset them
	  else{
	    creature_take_break(creature);
	  }
	}
      }
  }

  endwin();
  return 0;
}
