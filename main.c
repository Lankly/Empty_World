#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <time.h>
#include "helpers.h"
#include "map.h"
#include "overworld.h"
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

    if(player_is_on_overworld()){
      overworld_step();
    }
    else{
      apply_to_all_maps(&map_step);
    }
  }

  endwin();
  return 0;
}
