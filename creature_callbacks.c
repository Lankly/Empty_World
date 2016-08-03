#include "colors.h"
#include "classes.h"
#include "creature_callbacks.h"
#include "creature.h"
#include "map.h"
#include "items.h"
#include "status.h"
#include "tiles.h"
#include <limits.h>
#include <string.h>

/* Move to the player, if visible.
 * Wander around randomly otherwise.
 */
void defaultPathfindCallback(struct creature_t* creature, 
			     map_t* map){
  //Make sure everything is right before starting
  if(creature == NULL){
    quit("ERROR: Cannot move NULL Creature");
  }
  if(map == NULL){
    quit("Error: Cannot move Creature on NULL Map");
  }
  int x, y, px, py;
  creature_get_coord(creature, &x, &y, NULL);
  creature_get_coord(player, &px, &py, NULL);
  if(map_get_tile(map, x, y) == TILE_UNKNOWN){
    return;
  }

  //If it can see the player
  if(creature_is_visible(player, creature)){
    int move_x = 0, move_y = 0;
    //If the creature is to the right of the player
    if(x > px && tile_is_passable(map_get_tile(map, x - 1, y))){
      move_x--;
    }
    //If the creature is to the left of the player
    else if(x < px && tile_is_passable(map_get_tile(map, x + 1, y))){
      move_x++;
    }
    //If the creature is below the player
    if(y > py && tile_is_passable(map_get_tile(map, x + move_x, y - 1))){
      move_y--;
    }
    //If the creature is above the player
    else if(y < py && tile_is_passable(map_get_tile(map, x + move_x, y + 1))){
      move_y++;
    }
    
    x += move_x;
    y += move_y;
  }
  //Else wander randomly
  else{
    //It is perfectly okay for it to not move at all for its turn
    int move_x = 0, move_y = 0, tries = 0;
    do{
      move_x = (rand()%2) * ((rand() % 2) ? -1 : 1);
      move_y = (rand()%2) * ((rand() % 2) ? -1 : 1);
    }while(!tile_is_passable(map_get_tile(map, x + move_x, y + move_y))
	   && tries < 30);
    if(tries < 30){
      creature_place_at_coord(creature, map, x + move_x, y + move_y);
    }
  }
}

void defaultTakeTurnCallback(creature_t* c, map_t* map){
  //Make sure everything is right before starting
  if(c==NULL){quit("ERROR: Cannot move NULL Creature");}
  if(map==NULL){quit("Error: Cannot move Creature on NULL Map");}

  int x, y, px, py;
  creature_get_coord(c, &x, &y, NULL);
  creature_get_coord(player, &px, &py, NULL);
  //If player is adjacent, attack
  int x_diff = px - x;
  int y_diff = py - y;
  if(x_diff >= -1 && x_diff <= 1 && y_diff >= -1 && y_diff <= 1){
    int temp = 0;
    damage_body_part(&temp, c, player, creature_get_body(player),
		     creature_get_damage(c), DMG_SLASHING);
  }
  //Otherwise, pathfind
  else if(!creature_is_immobile(c)){
    creature_pathfind(c, map);
  }
}

/* Callback used for killing a creature by default
 */
void defaultKillCallback(creature_t *c, map_t *map){
  if(c == NULL){quit("Error: Cannot kill NULL Creature.");}
  if(map == NULL){quit("Error: Cannot kill Creature on NULL Map.");}

  int x, y;
  creature_get_coord(c, &x, &y, NULL);
  item_t *corpse = create_corpse(c);

  if(corpse != NULL){
    map_add_item(map, x, y, corpse, false);
  }
    
  map_remove_creature(map, c);
  creature_free(c);
}

/* Pathfinding for rats.
 */
void ratPathfindCallback(creature_t *creature, struct map_t *map){
  if(creature == NULL){
    quit("Error: ratPathfindCallback() passed NULL Creature.");}
  if(map == NULL){
    quit("Error: ratPathfindCallback() passed NULL Map.");}

  /* Rats run along walls looking for food. If they have been attacked, and 
   * their health is above 20%, they will attempt to fight back. If they have
   * not been attacked, or their health is below 20%, they will flee from 
   * non-rats.
   */

  int x, y, move_x = INT_MIN, move_y = INT_MIN;
  creature_get_coord(creature, &x, &y, NULL);
  
  /* Check to see if we're running away from something first */
  int closest_distance = INT_MAX;
  creature_t *closest_creature = NULL;
  for(clist_t *cur = map_get_creatures(map);
      cur != NULL;
      cur = clist_next(cur)){
    creature_t *cur_creature = clist_get_creature(cur);
    //Can't handle a NULL creature here, so move on
    if(cur_creature == NULL){
      break;
    }
    int cur_x, cur_y;
    creature_get_coord(cur_creature, &cur_x, &cur_y, NULL);
    
    int dist = get_distance(x, y,
			    cur_x, cur_y);
    //Compare distances, make sure we're not comparing with the same type
    if(dist < closest_distance &&
       creature_get_type(cur_creature) != creature_get_type(creature)){
      closest_distance = dist;
      closest_creature = cur_creature;
    }
  }
  //If we found something to run from
  if(closest_creature != NULL &&
     creature_is_visible(closest_creature, creature)){
    int closest_x, closest_y;
    creature_get_coord(closest_creature, &closest_x, &closest_y, NULL);
    
    //Move away on the x-axis
    if(x < closest_x && tile_is_passable(map_get_tile(map,x - 1, y))){
      move_x = -1;
    }
    else if(x > closest_x && tile_is_passable(map_get_tile(map, x + 1, y))){
      move_x = 1;
    }
    else{
      move_x = 0;
    }

    //Move away on the y-axis
    if(y < closest_y){
      //check if it can move diagonally, or if it's the only way to go
      if(tile_is_passable(map_get_tile(map, x + move_x, y-1))){
	move_y = -1;
      }
      //if it can't go diagonally, but it can go on both the x and the y-axis,
      //randomly pick x or y
      else if(move_x != 0
	      && tile_is_passable(map_get_tile(map, x, y-1))
	      && rand()%2){
	move_x = 0; move_y = -1;
      }
      else{
	move_y = 0;
      }
    }
    else if(y > closest_y){
      if(tile_is_passable(map_get_tile(map, x + move_x, y+1))){
	move_y = 1;
      }
      //if it can't go diagonally, but it can go on both the x and the y-axis,
      //randomly pick x or y
      else if(move_x != 0 &&
	      tile_is_passable(map_get_tile(map, x, y+1))
	      && rand()%2){
	move_x = 0; move_y = -1;
      }
      else{
	move_y = 0;
      }
    }
    else{
      move_y = 0;
    }
  }
  //If the creature didn't move, do something else
  if(move_x == 0 && move_y == 0){
    move_x = INT_MIN; move_y = INT_MIN;
  }
     
  int dist = 1000;
  //If we didn't move previously, and we're hungry, check for food
  if(move_x == INT_MIN &&
     ((double)get_hunger(creature)) / ((double)get_max_hunger(creature)) < .8){
    item_map_t *cur = map_get_items(map);
    for(int j = -creature_see_distance(creature); 
	j < creature_see_distance(creature);
	j++){
      int cur_y = y + j;
      for(int i = -creature_see_distance(creature);
	  i < creature_see_distance(creature);
	  i++){
	//If there are no more items to look at
	if(cur != NULL){
	  int cur_x = x + i;
	  /* Check that this isn't out of range, that there's no wall in the way,
	   * and that there are items here.
	   */
	  if(cur->y == cur_y && cur->x == cur_x 
	     && coord_in_range(cur_x, cur_y, creature)
	     && map_tile_is_visible(map, cur_x, cur_y, creature)){
	    for(struct itemlist_t *item = cur->first;
		item != NULL;
		item = item->next){
	      /* If any items here are edible, move towards it if it's closer
		 than anything else found so far. 
	      */
	      if(item->item->edible
		 && get_distance(i, j, x, y) < dist){
		dist = get_distance(i, j, x, y);
		move_x = 0;
		if(j < 0){move_x = -1;}
		else if(j > 0){move_x = 1;}
		
		move_y = 0;
		if(i < 0){move_y = -1;}
		else if(i > 0){move_y = 1;}
	      }
	    }
	    cur = cur->next;
	  }
	}
      }
    }
  }

  //If not heading to food, move along a wall
  if(move_x == INT_MIN){
    int r = map_get_tile(map, x+1, y);
    int l = map_get_tile(map, x-1, y);
    int u = map_get_tile(map, x, y-1);
    int d = map_get_tile(map, x, y+1);
    int ur = map_get_tile(map, x+1, y-1);
    int ul = map_get_tile(map, x-1, y-1);
    int dr = map_get_tile(map, x+1, y+1);
    int dl = map_get_tile(map, x-1, y+1);

    //Figure out if already moving along the wall
    if(u == TILE_WALL || d == TILE_WALL || r == TILE_WALL || l == TILE_WALL
       || dr == TILE_WALL || dl == TILE_WALL || ur==TILE_WALL || ul==TILE_WALL
       || map_get_tile(map, x, y) == TILE_CORRIDOR){
      bool can_go_up = u == TILE_CORRIDOR || r == TILE_WALL || l == TILE_WALL,
	can_go_down = d == TILE_CORRIDOR || r == TILE_WALL || l == TILE_WALL, 
	can_go_left = l == TILE_CORRIDOR || u == TILE_WALL || d == TILE_WALL,
	can_go_right = r == TILE_CORRIDOR || u == TILE_WALL || d == TILE_WALL, 
	can_go_ur = ur == TILE_CORRIDOR, can_go_dr = dr == TILE_CORRIDOR,
	can_go_dl = dl == TILE_CORRIDOR, can_go_ul = ul == TILE_CORRIDOR;
      if(ur == TILE_WALL || map_tile_is_door(ur)){
	can_go_up = tile_is_passable(u);
	can_go_right = tile_is_passable(r);
      }
      if(dr == TILE_WALL || map_tile_is_door(dr)){
	can_go_down = tile_is_passable(d);
	can_go_right = tile_is_passable(r);
      }
      if(dl == TILE_WALL || map_tile_is_door(dl)){
	can_go_down = tile_is_passable(d);
	can_go_left = tile_is_passable(l);
      }
      if(ul == TILE_WALL || map_tile_is_door(ul)){
	can_go_left = tile_is_passable(l);
	can_go_up = tile_is_passable(u);
      }

      can_go_ur = tile_is_passable(ur) 
	&& (u == TILE_WALL || map_tile_is_door(u) || r == TILE_WALL
	    || map_tile_is_door(r));
      can_go_ul = tile_is_passable(ul) 
	&& (u == TILE_WALL || map_tile_is_door(u) || l == TILE_WALL
	    || map_tile_is_door(l));
      can_go_dr = tile_is_passable(dr) 
	&& (d == TILE_WALL || map_tile_is_door(d) || r == TILE_WALL
	    || map_tile_is_door(r));
      can_go_dl = tile_is_passable(dl) 
	&& (d == TILE_WALL || map_tile_is_door(d) || l == TILE_WALL
	    || map_tile_is_door(l));

      //If can continue moving, do so. If not, pick a new direction
      //up
      int last_pos = creature_get_last_position(creature);
      if(last_pos == 2){
	if(can_go_up && !can_go_ur && !can_go_ul){
	  move_x = 0; move_y = -1;}
	else if(can_go_ur || can_go_ul){
	  move_x = can_go_ur ? (rand() % (can_go_ul + 1)) ? -1 : 1 : -1;
	  move_y = -1;
	}
	else if(can_go_left || can_go_right){
	  move_x = can_go_left ? (rand() % (can_go_right + 1)) ? 1 : -1 : 1;
	  move_y = 0;
	}
	else{move_x = 0; move_y = 1;}
      }
      //right
      else if(last_pos == 4){
	if(can_go_right && !can_go_ur && !can_go_dr){
	  move_x = 1; move_y = 0;}
	else if(can_go_ur || can_go_dr){
	  move_x = 1;
	  move_y = can_go_ur ? (rand() % (can_go_dr + 1)) ? 1 : -1 : 1;
	}
	else if(can_go_up || can_go_down){
	  move_x = 0;
	  move_y = can_go_up ? (rand() % (can_go_down + 1)) ? 1 : -1 : 1;
	}
	else{move_x = -1; move_y = 0;}
      }
      //down
      else if(last_pos == 8){
	if(can_go_down && !can_go_dl && !can_go_dr){
	  move_x = 0; move_y = 1;}
	else if(can_go_dl || can_go_dr){
	  move_x = can_go_dl ? (rand() % (can_go_dr + 1)) ? 1 : -1 : 1;
	  move_y = 1;
	}
	else if(can_go_left || can_go_right){
	  move_x = can_go_left ? (rand() % (can_go_right + 1)) ? 1 : -1 : 1;
	  move_y = 0;
	}
	else{move_x = 0; move_y = -1;}
      }
      //left
      else if(last_pos == 6){
	if(can_go_left && !can_go_ul && !can_go_dl){
	  move_x = -1; move_y = 0;}
	else if(can_go_ul || can_go_dl){
	  move_x = -1;
	  move_y = can_go_ul ? (rand() % (can_go_dr + 1)) ? 1 : -1 : 1;
	}
	else if(can_go_up || can_go_down){
	  move_x = 0;
	  move_y = can_go_up ? (rand() % (can_go_down + 1)) ? 1 : -1 : 1;
	}
	else{move_x = 1; move_y = 0;}
      }

      //Was moving diagonally
      else{
	/* We will gather every possible direction we can move in, randomly 
	 * choose among them.
	 */
	switch(can_go_up + can_go_down + can_go_left + can_go_right){
	case 0: break;
	case 1:
	  if(can_go_up){move_x = 0; move_y = -1;}
	  else if(can_go_down){move_x = 0; move_y = 1;}
	  else if(can_go_right){move_x = 1; move_y = 0;}
	  else{move_x = -1; move_y = 0;}
	  break;
	case 2:
	  if(can_go_up){
	    if(rand() % 2){
	      move_x = 0; move_y = -1;
	    }
	    else if(can_go_down){move_x = 0; move_y = 1;}
	    else if(can_go_right){move_x = 1; move_y = 0;}
	    else if(can_go_left){move_x = -1; move_y = 0;}
	  }
	  else if(can_go_right){
	    if(rand() % 2){
	      move_x = 1; move_y = 0;
	    }
	    else if(can_go_left){move_x = -1; move_y = 0;}
	    else if(can_go_down){move_x = 0; move_y = 1;}
	  }
	  else if(can_go_down && (rand() % 2)){
	    move_x = 0; move_y = 1;
	  }
	  else{
	    move_x = -1; move_y = 0;}
	  break;
	case 3:
	  if(!can_go_up){
	    switch(rand() % 3){
	    case 0:
	      move_x = 1; move_y = 0; break;
	    case 1:
	      move_x = 0; move_y = 1; break;
	    case 2:
	      move_x = -1; move_y = 0; break;
	    }
	  }
	  else if(!can_go_right){
	    switch(rand() % 3){
	    case 0:
	      move_x = 0; move_y = 1; break;
	    case 1:
	      move_x = -1; move_y = 0; break;
	    case 2:
	      move_x = 0; move_y = -1; break;
	    }
	  }
	  else if(!can_go_down){
	    switch(rand() % 3){
	    case 0:
	      move_x = -1; move_y = 0; break;
	    case 1:
	      move_x = 0; move_y = -1; break;
	    case 2:
	      move_x = 1; move_y = 0; break;
	    }
	  }
	  else{
	    switch(rand() % 3){
	    case 0:
	      move_x = 0; move_y = -1; break;
	    case 1:
	      move_x = 1; move_y = 0; break;
	    case 2:
	      move_x = 0; move_y = 1; break;
	    }
	  }
	  break;
	case 4:
	  move_x = (rand() % 2) * ((rand() % 2) ? -1 : 1);
	  move_y = !move_x ? (rand() % 2) ? 1 : -1 : 0;
	  break;
	}

	if(move_x == INT_MIN){
	  switch(can_go_ul + can_go_ur + can_go_dl + can_go_dr){
	  case 0: break;
	  case 1:
	    move_x = can_go_ul || can_go_dl ? -1 : 1;
	    move_y = can_go_ul || can_go_ur ? -1 : 1;
	    break;
	  case 2:
	    if(can_go_ul){
	      if(last_pos != 7){
		move_x = -1; move_y = -1;}
	      else{
		if(can_go_ur){
		  move_x = 1; move_y = -1;
		}
		else if(can_go_dl){
		  move_x = -1; move_y = 1;}
		else{
		  move_x = 1; move_y = 1;}
	      }
	    }
	    else if(can_go_ur){
	      if(last_pos != 9){
		move_x = 1; move_y = -1;}
	      else if(can_go_dl){
		move_x = -1; move_y = 1;}
	      else{
		move_x = 1; move_y = 1;}
	    }
	    else if(can_go_dl){
	      if(last_pos != 1){
		move_x = -1; move_y = 1;}
	      else{
		move_x = 1; move_y = 1;}
	    }
	    break;
	  }
	}
      }
    }
  }

  //If not at a wall, find a wall
  if(move_x == INT_MIN){
    for(int j = -creature_see_distance(creature); 
	j < creature_see_distance(creature);
	j++){
      for(int i = -creature_see_distance(creature);
	  i < creature_see_distance(creature);
	  i++){
	
	/* If there's a wall or door here, and it's shorter than something
	 * already found, pick this spot.
	 */
	if((map_get_tile(map, i, j) == TILE_WALL
	   || map_tile_is_door(map_get_tile(map, i, j)))
	   && get_distance(i, j, x, y) < dist){
	  dist = get_distance(i, j, x, y);
	  move_x = 0;
	  if(j < 0){move_x = -1;}
	  else if(j > 0){move_x = 1;}	  
	  move_y = 0;
	  if(i < 0){move_y = -1;}
	  else if(i > 0){move_y = 1;}
	}
      }
    }
  }

  //If wall not found
  if(move_x == INT_MIN){
    int last_pos = creature_get_last_position(creature);
    //Continue moving in a straight line
    if(last_pos != 0 && last_pos != 5){
      //x
      move_x = (last_pos % 3);
      if(move_x == 2 ){move_x = 0;}
      else if(move_x == 0){move_x = -1;}
      
      //y
      if(last_pos > 6){
	move_y = 1;}
      else{
	move_y = last_pos > 3 ? 0 : -1;}      
    }
    //Choose a direction
    else{
      move_x = (rand() % 2) ? -1 : 1;
      move_y = (rand() % 2) ? -1 : 1;
    }
  }

  //Just one final check to make sure the move is valid
  if(tile_is_passable(map_get_tile(map, x + move_x, y + move_y))){
    creature_place_at_coord(creature, map, move_x + x, move_y + y);
    
    //Keep track of last position
    creature_record_movement(creature, move_x, move_y);
  }
  else{
    creature_record_movement(creature, 0, 0);
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
  switch(map_get_dlevel(map)){
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
	creature_set_name(c, "Giant Rat");
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
      break;
    case 4:
      c = creature_spawn(CREATURE_TYPE_PLANT, map);
      creature_set_name(c, "Venus Human Trap");
      set_exam_text(c, "This plant will eat you alive!");
      set_strength(c, 3);
      set_perception(c, 1);
      set_intelligence(c, 0);
      set_immobile(c, true);
      creature_set_display(c, 'v');
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
      creature_set_name(c, "Human");
      set_exam_text(c, "This is a human warrior.");
      set_level(c, 3);
      break;
    case 9:
      c = creature_spawn(CREATURE_TYPE_HUMAN, map);
      set_class(c, CLASS_PIKEMAN);
      creature_set_name(c, "Human");
      set_exam_text(c, "This is a human pikeman.");
      break;
    case 10:
      c = creature_spawn(CREATURE_TYPE_SKELETON, map);
      break;
    }
    break;

  case 4 ://DLEVEL 4 ---- SOME WEAK_ANIMALS + ANIMALS + HUMANS
    if(rand() % 20 == 0){
      switch(rand() % 3){
      case 0:
	c = creature_spawn(CREATURE_TYPE_AVIAN, map);
	set_perception(c, 3 + !(rand() % 3));
	set_intelligence(c, 2 + (rand() % 2));
	set_agility(c, 3);
	set_luck(c, 2 + (rand() % 2));
	set_level(c, 3 + (rand() % 2));
	break;
      case 1:
	c = creature_spawn(CREATURE_TYPE_INSECT, map);
	set_strength(c, get_strength(c) + 1);
	break;
      case 2:
	c = creature_spawn(CREATURE_TYPE_PLANT, map);
	creature_set_name(c, "Venus Human Trap");
	set_exam_text(c, "This plant will eat you alive!");
	set_strength(c, 3 + (rand() % 2));
	set_perception(c, 1);
	set_intelligence(c, 0);
	set_immobile(c, true);
	creature_set_display(c, 'v');
	break;
      }
    }
    else{
      switch(rand() % 8){
      case 0:
	c = creature_spawn(CREATURE_TYPE_RODENT, map);
	set_perception(c, 3);
	set_intelligence(c, 2 + !(rand() % 3));
	set_agility(c, 2);
	set_luck(c, 2 + !(rand() % 3));
	set_level(c, 2);
	if(rand() % 2){
	  creature_set_name(c, "Giant Rat");
	  set_exam_text(c, "This is a very large rat.");
	  set_strength(c, 3);
	  set_level(c, 3 + (rand() % 2));
	}
	break;
      case 1:
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
	creature_set_name(c, "Human");
	set_exam_text(c, "This is a human warrior.");
	set_level(c, 3);
	break;
      case 9:
	c = creature_spawn(CREATURE_TYPE_HUMAN, map);
	set_class(c, CLASS_PIKEMAN);
	creature_set_name(c, "Human");
	set_exam_text(c, "This is a human pikeman.");
	break;
      case 10:
	c = creature_spawn(CREATURE_TYPE_SKELETON, map);
	break;
      }
    }
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
}
