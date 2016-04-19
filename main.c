#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <time.h>
#include "helpers.h"
#include "creature.h"
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
  //1427689757
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
	  if(creature->turn_tokens > 0){
	    creature->takeTurn(creature, cur_map);
	  }
	  //Otherwise, skip the turn to reset them
	  else{
	    creature->turn_tokens = creature->turn_tokens_reset_amount;
	  }
	}
      }
  }

  endwin();
  return 0;
}
