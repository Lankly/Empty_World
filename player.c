#include "player.h"
#include "classes.h"
#include "colors.h"
#include "creature.h"
#include "map.h"
#include "status.h"
#include <stdlib.h>
#include <string.h>

void playerTakeTurnCallback(struct creature_t* creature,
			    struct map_t* map){
  map_reveal(map, creature_see_distance(player));
  draw_map(map);
  draw_status(map);

  int plr_mv_to_x = player->x,
    plr_mv_to_y = player->y;
  
  do{
    /* If the player is quickmoving, keep doing that. 
     * Else, look for keyboard input.
     */
    if(qckmv){
      cmd = qckmv_cmd;
    }
    else{cmd = getch();}
    
  }while(!analyze_cmd(cmd, &plr_mv_to_x, &plr_mv_to_y));
    
  /* Ensure that the player is light enough to pass through corners,
       that they are not behind a closed door.
  */
  if(creature_can_move_to(player, plr_mv_to_x, plr_mv_to_y, cmd)){
    //Now we check to see if they're moving into a creature
    struct creature_t *creature_in_way = NULL;
    for(struct creature_list_node_t *cur = cur_map->creatures->first; 
	cur != NULL && creature_in_way == NULL;
	cur = cur->next){
      if(cur->creature->x == plr_mv_to_x && cur->creature->y == plr_mv_to_y
	 && cur->creature != player){
	creature_in_way = cur->creature;}
    }

    //And attacking if so
    if(creature_in_way){
      damage_creature(creature_in_way, 
		      player->name, 
		      creature_get_damage(player));
    }
    else{
      //If no creature in way, just move there.
      player->x=plr_mv_to_x;
      player->y=plr_mv_to_y;
    }
  }
  else{qckmv = false;}
  
  if(qckmv){
    qckmv = qckmv_continue(cur_map, plr_mv_to_x, plr_mv_to_y, qckmv_cmd);}
}

void player_init(char* name){
  player = (struct creature_t*)Calloc(1,sizeof(struct creature_t));
  player->display = '@' | COLOR_PAIR(CP_YELLOW_BLACK);
  player->exam_text = "This is you!";
  
  //Get player's name
  player->name = (char*)calloc(PLAYER_NAME_SIZE+1, sizeof(char));
  strncpy(player->name, msg_prompt("What is your name? "), PLAYER_NAME_SIZE);
  if(!strcmp(player->name, "")){
    player->name = "Lan";}

  player->corpse_type = CORPSE_HUMAN;
  player->class = CLASS_NEWB;
  set_strength(player, 1);
  set_perception(player, 1);
  set_endurance(player, 1);
  set_charisma(player, 1);
  set_intelligence(player, 1);
  set_luck(player, 1);
  set_health(player, 10);
  set_max_health(player, get_health(player));
  set_max_hunger(player, 200);
  set_hunger(player, player->max_hunger);
  set_gold(player, 5);
  set_level(player, 1);
  set_dlevel(player, 1);
  
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
