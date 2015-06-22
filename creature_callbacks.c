#include "classes.h"
#include "creature_callbacks.h"
#include "creature.h"
#include "player.h"
#include "map.h"
#include "items.h"
#include "status.h"
#include "tiles.h"

/* Move to the player, if visible.
 * Wander around randomly otherwise.
 */
void defaultPathfindCallback(struct creature_t* creature, 
			     map_t* map){
  //Make sure everything is right before starting
  if(creature==NULL){quit("ERROR: Cannot move NULL Creature");}
  if(map==NULL){quit("Error: Cannot move Creature on NULL Map");}
  if(map_get_tile(map, creature->x, creature->y)==TILE_UNKNOWN){
    return;
  }

  //If it can see the player
  if(creature_is_visible(player, creature)){
    int x_diff = player->x - creature->x;
    int y_diff = player->y - creature->y;
    //Next, check if the player is adjacent and attack if so
    if((x_diff < -1 || x_diff > 1) && (y_diff < -1 || y_diff > 1)){
      int move_x=0, move_y=0;
      //If the creature is to the right of the player
      if(creature->x > player->x){
	move_x--;}
      //If the creature is to the left of the player
      else if(creature->x < player->x){
	move_x++;}
      //If the creature is below the player
      if(creature->y > player->x){
	move_y--;}
      //If the creature is above the player
      else if(creature->y < player->x){
	move_y++;}
      
      creature->x += move_x;
      creature->y += move_y;
    }
  }
  //Else wander randomly
  else{
    //It is perfectly okay for it to not move at all for its turn
    int move_x = rand() % 2 * rand() % 2 ? -1 : 1;
    int move_y = rand() % 2 * rand() % 2 ? -1 : 1;
    int tries = 0;
    while(!tile_data[map_get_tile(map,
				  creature->x+move_x,
				  creature->y+move_y)].passable
	  && tries < 30){
      move_x = rand()%2 * rand() % 2 ? -1 : 1;
      move_y = rand()%2 * rand() % 2 ? -1 : 1;
      tries ++;
    }
    if(tries != 30){
      creature->x = creature->x + move_x;
      creature->y = creature->y + move_y;}
  }
}

void defaultTakeTurnCallback(struct creature_t* creature,
			     struct map_t* map){
  //Make sure everything is right before starting
  if(creature==NULL){quit("ERROR: Cannot move NULL Creature");}
  if(map==NULL){quit("Error: Cannot move Creature on NULL Map");}
  
  //If player is adjacent, attack
  int x_diff = player->x-creature->x;
  int y_diff = player->y-creature->y;
  if(x_diff >= -1 && x_diff <= 1 && y_diff >= -1 && y_diff <= 1){
    damage_creature(player, creature->name, creature_get_damage(creature));
  }
  //Otherwise, pathfind
  else{
    if(creature->pathfind == NULL){
      defaultPathfindCallback(creature, map);}
    else{
      creature->pathfind(creature, map);}
  }
}

/* All a spawner does on its turn is spawn creatures, with a small chance each
 * time. 
 */
void spawnerTakeTurnCallback(struct creature_t* creature,
			     struct map_t* map){
  if(map == NULL){
    quit("Error: Cannot spawn on NULL Map");}

  //Works one in 500 times
  if(rand() % 500 !=0){return;}

  struct creature_t *c = NULL;
  /* Decide what creature to spawn. Does not spawn that creature if not on the
   * proper floor range.
   */
  switch(map->dlevel){
  case 0 :
    break;
  case 1 :// DLEVEL 1  ---- WEAK ANIMALS
    switch(rand() % 5){
    case 0:
      c = creature_spawn(CREATURE_TYPE_RODENT, map);
      break;
    case 1:
      c = creature_spawn(CREATURE_TYPE_AVIAN, map);
      break;
    case 2:
      c = creature_spawn(CREATURE_TYPE_INSECT, map);
      break;
    case 3:
      c = creature_spawn(CREATURE_TYPE_PLANT, map);
      break;
    case 4:
      c = creature_spawn(CREATURE_TYPE_FOG, map);
    }
    break;

  case 2 :// DLEVEL 2 ---- WEAK ANIMALS + ANIMALS + LOW CHANCE FOR HUMAN
    if(!(rand() % 25)){
      c = creature_spawn(CREATURE_TYPE_HUMAN, map);
      set_strength(c, 3);
      set_perception(c, 2 + !(rand() % 2));
      set_charisma(c, 1);
      set_intelligence(c, 3);
      set_luck(c, 0);
      set_level(c, 3);
    }
    else{
      switch(rand() % 7){
      case 0:
	c = creature_spawn(CREATURE_TYPE_RODENT, map);
	set_intelligence(c, 2);
	set_agility(c, 2);
	set_luck(c, 1 + !(rand() % 5));
	break;
      case 1:
	c = creature_spawn(CREATURE_TYPE_AVIAN, map);
	set_intelligence(c, 2);
	set_perception(c, 2);
	break;
      case 2:
	c = creature_spawn(CREATURE_TYPE_INSECT, map);
	set_luck(c, !(rand() %2));
	break;
      case 3:
	c = creature_spawn(CREATURE_TYPE_PLANT, map);
	set_strength(c, 2);
	break;
      case 4:
	c = creature_spawn(CREATURE_TYPE_FOG, map);
	set_max_health(c, 10 + (rand() % 3));
	break;
      case 5:
	c = creature_spawn(CREATURE_TYPE_FELINE, map);
	break;
      case 6:
	c = creature_spawn(CREATURE_TYPE_CANINE, map);
	break;
      }
      set_level(c, 2);
    }
    break;

  case 3 ://DLEVEL 3 ---- WEAK ANIMALS + ANIMALS + SOME HUMANS
    switch(rand() % 11){
    case 0:
      c = creature_spawn(CREATURE_TYPE_RODENT, map);
      set_perception(c, 3);
      set_intelligence(c, 2 + !(rand() % 3));
      set_agility(c, 2);
      set_luck(c, 2 + !(rand() % 3));
      set_level(c, 2);
      if(!(rand() % 3)){
	set_name(c, "Giant Rat");
	set_exam_text(c, "This is a very large rat.");
	set_strength(c, 3);
	set_level(c, 3);
      }
      break;
    case 1:
      c = creature_spawn(CREATURE_TYPE_AVIAN, map);
      set_perception(c, 3);
      set_intelligence(c, 2 + !(rand() % 3));
      set_agility(c, 2);
      set_luck(c, 2);
      set_level(c, 2 + !(rand() % 2));
      break;
    case 2:
      c = creature_spawn(CREATURE_TYPE_FOG, map);
      set_max_health(c, get_max_health(c) + (rand() % 5));
      break;
    case 3:
      c = creature_spawn(CREATURE_TYPE_INSECT, map);
      set_strength(c, get_strength(c) + 1);
      if(get_perception(player) > 2){
	set_exam_text(c, "This is a slightly larger hornet.");}
      break;
    case 4:
      c = creature_spawn(CREATURE_TYPE_PLANT, map);
      set_name(c, "Venus Human Trap");
      set_exam_text(c, "This plant will eat you alive!");
      set_strength(c, 3);
      set_perception(c, 1);
      set_intelligence(c, 0);
      set_immobile(c, true);
      c->display = 'v';
      break;
    case 5:
      c = creature_spawn(CREATURE_TYPE_FELINE, map);
      set_perception(c, get_perception(c) + (rand() % 2));
      set_level(c, 3);
      break;
    case 6:
      c = creature_spawn(CREATURE_TYPE_CANINE, map);
      set_strength(c, get_strength(c) + (rand() % 2));
      set_perception(c, get_perception(c) + (rand() % 2));
      set_level(c, 3);
      break;
    case 7:
      c = creature_spawn(CREATURE_TYPE_HUMAN, map);
      set_level(c, 3);
      break;
    case 8:
      c = creature_spawn(CREATURE_TYPE_HUMAN, map);
      set_class(c, CLASS_WARRIOR);
      set_name(c, "Human Warrior");
      set_exam_text(c, "This is a human warrior.");
      set_level(c, 3);
      break;
    case 9:
      c = creature_spawn(CREATURE_TYPE_HUMAN, map);
      set_class(c, CLASS_PIKEMAN);
      set_name(c, "Human Pikeman");
      set_exam_text(c, "This is a human pikeman.");
      break;
    case 10:
      c = creature_spawn(CREATURE_TYPE_SKELETON, map);
      break;
    }
    break;

  case 4 ://DLEVEL 4 ---- SOME WEAK_ANIMALS + ANIMALS + HUMANS
    break;
  case 5 ://DLEVEL 5 ---- ANIMALS + HUMANS + SOME GOBLINS
    break;
  case 6 ://DLEVEL 6 ---- ANIMALS + HUMANS + GOBLINS + SOME GOLEMS
    break;
  case 7 ://DLEVEL 7 ---- ANIMALS + HUMANS + GOBLINS + GOLEMS + SOME ORCS
    break;
  case 8 ://DLEVEL 8 ---- ANIMALS + HUMANS + GOBLINS + SOME GOLEMS + ORCS
    break;
  case 9 ://DLEVEL 9 ---- ANIMALS + SOME HUMANS + GOBLINS + ORCS
    break;
  case 10 ://DLEVEL 10 -- ANIMALS + SOME HUMANS + GOBLINS + ORCS + CHANCE OF EYE
    break;
  case 11 ://DLEVEL 11 -- ANIMALS + S_HUMANS + S_GOBLINS + ORCS+S_MONSTERS+C_EYE
    break;
  case 12 ://DLEVEL 12 -- ANIMALS + S_HUMANS + S_GOBLINS + S_ORCS+MONSTERS+C_EYE
    break;
  case 13 ://DLEVEL 13 -- ANIMALS + S_GOBLINS + S_ORCS + MONSTERS + C_EYE
    break;
  case 14 ://DLEVEL 14 -- ANIMALS + S_ORCS + MONSTERS + S_GOLEMS + C_EYE
    break;
  case 15 ://DLEVEL 15 -- ANIMALS + S_ORCS + MONSTERS + S_GOLEMS + C_EYE
    break;
  case 16 ://DLEVEL 16 -- ANIMALS + S_ORCS + MONSTERS + S_GOLEMS
    break;
  case 17 ://DLEVEL 17 -- ANIMALS + S_ORCS + MONSTERS
    break;
  case 18 ://DLEVEL 18 -- ANIMALS + MONSTERS + S_SENTINELS
    break;
  case 19 ://DLEVEL 19 -- ANIMALS + MONSTERS + SENTINELS + S_DEMONS 
    break;
  case 20 ://DLEVEL 20 -- ANIMALS + MONSTERS + SENTINELS + DEMONS
    break;
  case 21 ://DLEVEL 21 -- ANIMALS+MONSTERS + SENTINELS + DEMONS + S_HUMANS
    break;
  case 22 ://DLEVEL 22 -- ANIMALS+MONSTERS + SENTINELS + DEMONS + HUMANS
    break;
  case 23 ://DLEVEL 23 -- ANIMALS+MONSTERS + SENTINELS + DEMONS + HUMANS +GOLEMS
    break;
  case 24 ://DLEVEL 24 -- ANIMALS+MONSTERS +SENTINELS +DEMONS +HUMANS +GRIFFONS
    break;
  case 25 ://DLEVEL 25 -- ANIMALS+MONSTERS +SENTINELS +DEMONS +HUMANS +GRIFFONS
    break;
  case 26 ://DLEVEL 26 -- ANIMALS+MONSTERS +SENTINELS +DEMONS +HUMANS +GRIFFONS
    break;
  case 27 ://DLEVEL 27 -- ANIMALS+MONSTERS+SENTINELS+DEMONS+HUMANS+GRIFFONS
    break;
  case 28 ://DLEVEL 28 -- ANIMALS+MONSTERS+SENTINELS+DEMONS +GRIFFONS
    break;
  case 29 ://DLEVEL 29 -- ANIMALS+MONSTERS+SENTINELS+DEMONS +GRIFFONS
    break;
  case 30 ://DLEVEL 30 -- RATS + DEMONS + HUMANS + GRIFFONS
    break;
  }

  //msg_addf("Spawned %s", c->name);
  set_health(c, get_max_health(c));
  c->dlevel = map->dlevel;
}
