#include "player.h"
#include "colors.h"
#include "creature.h"
#include "map.h"
#include "status.h"
#include <stdlib.h>
#include <string.h>

void playerTakeTurnCallback(struct creature_t* creature,
			    struct map_t* map){
  map_reveal(map, player->x, player->y, creature_see_distance(player));
  draw_map(map);
  draw_status(map);

  int plr_mv_to_x = player->x,
    plr_mv_to_y = player->y;
  
  /* If the player is quickmoving, keep doing that. 
   * Else, look for keyboard input.
   */
  if(qckmv){
    cmd = qckmv_cmd;
  }
  else{cmd = getch();}
  
  analyze_cmd(cmd, &plr_mv_to_x, &plr_mv_to_y);
  
  
  /* Ensure that the player is light enough to pass through corners,
       that they are not behind a closed door.
  */
  if(creature_can_move_to(player, plr_mv_to_x, plr_mv_to_y, cmd)){
    player->x=plr_mv_to_x;
    player->y=plr_mv_to_y;
  }
  else{qckmv = false;}
  
  if(qckmv){qckmv=qckmv_continue(cur_map, plr_mv_to_x, plr_mv_to_y, qckmv_cmd);}
}

void player_init(char* name){
  player = (struct creature_t*)Calloc(1,sizeof(struct creature_t));
  player->display = '@' | COLOR_PAIR(CP_YELLOW_BLACK);
  player->exam_text = "This is you!";
  player->name = (char*)calloc(PLAYER_NAME_SIZE+1, sizeof(char));
  char* input_name = msg_prompt("What is your name? ");
  if(strcmp(input_name, "") == 0){
    player->name = "Player";
  }
  else{
    strncpy(player->name, input_name, PLAYER_NAME_SIZE);}
  
  set_vision(player, true);
  set_conscious(player, true);
  set_strength(player, 1);
  set_perception(player, 1);
  set_endurance(player, 1);
  set_charisma(player, 1);
  set_intelligence(player, 1);
  set_luck(player, 1);
  set_health(player, 10);
  set_max_hunger(player, 200);
  set_hunger(player, player->max_hunger);
  set_gold(player, 5);
  set_level(player, 1);
  set_name(player, name);

  player->breathables = (breathe_list_t*)Calloc(1,sizeof(breathe_list_t));
  add_breathable(player, BREATHE_AIR);
  player->consumables = (consume_list_t*)Calloc(1,sizeof(consume_list_t));
  add_consumable(player, CONSUME_ANIMAL);
  add_consumable(player, CONSUME_INSECT);
  add_consumable(player, CONSUME_FOOD);
  add_consumable(player, CONSUME_DRUG);
  add_consumable(player, CONSUME_WATER);
  add_consumable(player, CONSUME_ALCOHOL);
  add_consumable(player, CONSUME_POTION);
  player->intrinsics = (intrinsics_list_t*)Calloc(1,sizeof(intrinsics_list_t));
  player->resistances = (resistances_list_t*)Calloc(1,sizeof(resistances_list_t));
  player->takeTurn = &playerTakeTurnCallback;

  cmd = 0;
}
