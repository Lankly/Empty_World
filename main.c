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

    //Let each creature take its turn
    for(creature_list_node_t* cur = cur_map->creatures->first; 
	cur != NULL; cur = cur->next){
      cur->creature->takeTurn(cur->creature, cur_map);
    }
    num_turns++;
  }

  endwin();
  return 0;
}
