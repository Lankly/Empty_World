#include "bodies.h"
#include <math.h>
#include <ncurses.h>
#include <stdbool.h>
#include <string.h>
#include "colors.h"
#include "creature.h"
#include "inventory.h"
#include "items.h"
#include "macros.h"
#include "map.h"
#include "status.h"
#include "player.h"

body_part_t *generate_part(char *name, int health, int blood, int size, bool v){
  body_part_t *to_ret = Calloc(1, sizeof(body_part_t));

  //Add the info
  if(name != NULL){
    to_ret->name = Calloc(strlen(name), sizeof(char));
    strcpy(to_ret->name, name);
  }
  to_ret->health = health;
  to_ret->health_max = health;
  to_ret->blood_remaining = blood;
  to_ret->size = size;
  to_ret->vital = v;

  return to_ret;
}

body_part_t *gen_arm(bool left){
  body_part_t *arm = generate_part(left ? "Left upper arm" : "Right upper arm",
				   10,
				   BLOOD_FULL,
				   SIZE_AVG,
				   false);
  arm->attached = bodylist_new();
  body_part_t *forearm = generate_part(left ? "Left forearm" : "Right forearm",
				       10, BLOOD_FULL, SIZE_AVG, false);
  forearm->attached = bodylist_new();
  bodylist_add(forearm->attached, gen_hand(left));
  bodylist_add(arm->attached, forearm);
  
  return arm;
}

body_part_t *gen_foot(bool left){
  body_part_t *foot = generate_part(left ? "Left foot" : "Right foot",
				    5,
				    BLOOD_FULL,
				    SIZE_SMALL,
				    false);
  foot->attached = bodylist_new();
  bodylist_add(foot->attached, generate_part("Big toe", 1,
					     BLOOD_INSIGNIFICANT,
					     SIZE_TINY, false));
  bodylist_add(foot->attached, generate_part("Pointer toe", 1,
					     BLOOD_INSIGNIFICANT,
					     SIZE_TINY, false));
  bodylist_add(foot->attached, generate_part("Middle toe", 1,
					     BLOOD_INSIGNIFICANT,
					     SIZE_TINY, false));
  bodylist_add(foot->attached, generate_part("Ring toe", 1,
					     BLOOD_INSIGNIFICANT,
					     SIZE_TINY, false));
  bodylist_add(foot->attached, generate_part("Little toe", 1,
					     BLOOD_INSIGNIFICANT,
					     SIZE_TINY, false));

  return foot;
}

body_part_t *gen_hand(bool left){
  body_part_t *hand = generate_part(left ? "Left hand" : "Right hand",
				    5,
				    BLOOD_FULL,
				    SIZE_SMALL,
				    false);
  hand->attached = bodylist_new();
  bodylist_add(hand->attached, generate_part("Thumb", 1,
					     BLOOD_INSIGNIFICANT,
					     SIZE_TINY, false));
  bodylist_add(hand->attached, generate_part("Pointer finger", 1,
					     BLOOD_INSIGNIFICANT,
					     SIZE_TINY, false));
  bodylist_add(hand->attached, generate_part("Middle finger", 1,
					     BLOOD_INSIGNIFICANT,
					     SIZE_TINY, false));
  bodylist_add(hand->attached, generate_part("Ring finger", 1,
					     BLOOD_INSIGNIFICANT,
					     SIZE_TINY, false));
  bodylist_add(hand->attached, generate_part("Little finger", 1,
					     BLOOD_INSIGNIFICANT,
					     SIZE_TINY, false));

  return hand;
}

body_part_t *gen_leg(bool left){
  body_part_t *leg = generate_part(left ? "Left thigh" : "Right thigh",
				    15,
				    BLOOD_FULL,
				    SIZE_LARGE,
				    false);
  leg->attached = bodylist_new();
  body_part_t *shin = generate_part(left ? "Left shin" : "Right shin",
				    10, BLOOD_FULL, SIZE_AVG, false);
  shin->attached = bodylist_new();
  bodylist_add(shin->attached, gen_foot(left));
  bodylist_add(leg->attached, shin);

  return leg;
}

body_part_t *gen_head(){
  body_part_t *head = generate_part("Head", 20, BLOOD_FULL, SIZE_AVG, true);

  head->attached = bodylist_new();
  head->organs = bodylist_new();

  bodylist_add(head->attached,
	       generate_part("Left eye", 1, 0, SIZE_TINY, false));
  bodylist_add(head->attached,
	       generate_part("Right eye", 1, 0, SIZE_TINY, false));
  bodylist_add(head->organs,
	       generate_part("Tongue", 2, 0, SIZE_TINY, false));
  bodylist_add(head->organs,
	       generate_part("Brain", 5, 0, SIZE_SMALL, true));

  return head;
}
body_part_t *gen_cat(bool giant){
  return gen_cat_torso(giant);
}
body_part_t *gen_cat_torso(bool giant){
  body_part_t *torso = generate_part("Body", 5, BLOOD_FULL,
				     SIZE_AVG+giant, true);

  torso->attached = bodylist_new();
  torso->organs = bodylist_new();
  bodylist_add(torso->attached,
	       generate_part("Left foreleg", 2, BLOOD_FULL,
			     SIZE_SMALL+giant, false));
  bodylist_add(torso->attached,
	       generate_part("Right foreleg", 2, BLOOD_FULL,
			     SIZE_SMALL+giant, false));
  bodylist_add(torso->attached,
	       generate_part("Left hindleg", 2, BLOOD_FULL,
			     SIZE_SMALL+giant, false));
  bodylist_add(torso->attached,
	       generate_part("Right hindleg", 2, BLOOD_FULL,
			     SIZE_SMALL+giant, false));
  bodylist_add(torso->attached,
	       generate_part("Tail", 1, 0, SIZE_SMALL+giant, false));
  bodylist_add(torso->organs,
	       generate_part("Heart", 3, BLOOD_EMPTY, SIZE_TINY+giant, false));
  bodylist_add(torso->organs,
	       generate_part("Lungs", 3, BLOOD_EMPTY, SIZE_TINY+giant, false));
  bodylist_add(torso->organs,
	       generate_part("Guts", 3, BLOOD_EMPTY, SIZE_TINY+giant, false));
  body_part_t *head = generate_part("Head", 4, BLOOD_FULL, SIZE_SMALL+giant, false);
  head->organs = bodylist_new();
  bodylist_add(head->organs,
	       generate_part("Brain", 3, BLOOD_FULL, SIZE_TINY, false));
  bodylist_add(torso->attached, head);

  return torso;
}

body_part_t *gen_human(){
  return gen_human_torso();
}

body_part_t *gen_human_head(){
  body_part_t *head = gen_head();

  bodylist_add(head->attached,
	       generate_part("Left ear", 3, 0, SIZE_TINY, false));
  bodylist_add(head->attached,
	       generate_part("Right ear", 3, 0, SIZE_TINY, false));
  bodylist_add(head->attached,
	       generate_part("Hair", 0, BLOOD_NONE, SIZE_TINY, false));
  bodylist_add(head->attached,
	       generate_part("Nose", 3, 0, SIZE_TINY, false));

  return head;
}

body_part_t *gen_human_torso(){
  body_part_t *torso = generate_part("Thorax", 50, BLOOD_FULL, SIZE_LARGE, true);

  torso->attached = bodylist_new();
  torso->organs = bodylist_new();
  bodylist_add(torso->attached, gen_arm(true));
  bodylist_add(torso->attached, gen_arm(false));
  bodylist_add(torso->organs,
	       generate_part("Heart", 20, BLOOD_EMPTY, SIZE_SMALL, true));
  bodylist_add(torso->organs,
	       generate_part("Lungs", 20, BLOOD_EMPTY, SIZE_AVG, true));
  
  body_part_t *abdomen = generate_part("Abdomen", 30,
				       BLOOD_FULL, SIZE_AVG, true);
  abdomen->attached = bodylist_new();
  abdomen->organs = bodylist_new();
  bodylist_add(abdomen->attached, gen_leg(true));
  bodylist_add(abdomen->attached, gen_arm(false));
  bodylist_add(abdomen->organs,
	       generate_part("Stomach", 6, BLOOD_FULL, SIZE_SMALL, true));
  bodylist_add(abdomen->organs,
	       generate_part("Small intestine", 4,
			     BLOOD_EMPTY, SIZE_SMALL, true));
  bodylist_add(abdomen->organs,
	       generate_part("Large intestine", 4, BLOOD_EMPTY, SIZE_SMALL, true));
  bodylist_add(torso->attached, abdomen);
  bodylist_add(abdomen->organs,
	       generate_part("Liver", 3, BLOOD_EMPTY, SIZE_TINY, true));
  bodylist_add(abdomen->organs,
	       generate_part("Kidneys", 4, BLOOD_EMPTY, SIZE_TINY, true));

  body_part_t *neck = generate_part("Neck", 20, BLOOD_FULL, SIZE_SMALL, true);
  bodylist_add(torso->attached, neck);
  neck->attached = bodylist_new();
  bodylist_add(neck->attached, gen_human_head());
  
  return torso;
}

body_part_t *gen_owl(bool giant){
  body_part_t *owl = generate_part("Cave Owl", -1,
				   BLOOD_FULL,
				   SIZE_SMALL + giant,
				   false);
  return owl;
}

body_part_t *gen_rat(bool giant){
  body_part_t *rat = generate_part("Rat", 5+9+4,
				   BLOOD_FULL,
				   SIZE_SMALL + giant,
				   false);
  rat->attached = bodylist_new();

  body_part_t *limbs =
    generate_part("Limbs", 9, BLOOD_FULL, SIZE_SMALL+giant, false);
  limbs->attached = bodylist_new();
  bodylist_add(limbs->attached,
	       generate_part("Left foreleg", 2, BLOOD_FULL,
			     SIZE_TINY + giant, false));
  bodylist_add(limbs->attached,
	       generate_part("Right foreleg", 2, BLOOD_FULL,
			     SIZE_TINY + giant, false));
  bodylist_add(limbs->attached,
	       generate_part("Left hindleg", 2, BLOOD_FULL,
			     SIZE_TINY + giant, false));
  bodylist_add(limbs->attached,
	       generate_part("Right hindleg", 2, BLOOD_FULL,
			     SIZE_TINY + giant, false));
  bodylist_add(limbs->attached,
	       generate_part("Tail", 1, 0, SIZE_TINY + giant, false));
  bodylist_add(rat->attached, limbs);
  
  bodylist_add(rat->attached, gen_rat_torso(giant));
  
  body_part_t *head =
    generate_part("Head", 4, BLOOD_FULL, SIZE_TINY + giant, true);
  head->organs = bodylist_new();
  bodylist_add(head->organs,
	       generate_part("Brain", 3, BLOOD_FULL, SIZE_TINY, false));
  bodylist_add(rat->attached, head);

  int rv = rand()%3;
  switch(rv){
  case 0 :
    rat->image =
      " (\\,/)\n"
      "  oo   '''//,        _\n"
      ",/_;~,        \\,    /\n"
      "\"'   \\    (    \\    !\n"
      "      |,|  \\    |__.\\'\n"
      "      '~  '~----''";
    rat->image_width = 23;
    break;
  case 1 :
    rat->image =
      "  (\\;/)\n"
      "  oo   \\//,        _\n"
      ",/_;~      \\,     / '\n"
      "\"'    (  (   \\    !\n"
      "      //  \\   |__.'\n"
      "     '~  '~----''";
    rat->image_width = 21;
    break;
  default :
    rat->image =
      "(\\,;,/)\n"
      " (o o)\\//,\n"
      "  \\ /     \\,\n"
      "  `+'(  (   \\    )\n"
      "     //  \\   |_./\n"
      "   '~' '~----'";
    rat->image_width = 18;
  }
  
  return rat;
}
body_part_t *gen_rat_torso(bool giant){
  body_part_t *torso = generate_part("Body", 5,
				     BLOOD_FULL,
				     SIZE_SMALL + giant,
				     true);

  torso->attached = bodylist_new();
  torso->organs = bodylist_new();
  bodylist_add(torso->organs,
	       generate_part("Heart", 3, BLOOD_EMPTY,
			     SIZE_TINY + giant, false));
  bodylist_add(torso->organs,
	       generate_part("Lungs", 3, BLOOD_EMPTY,
			     SIZE_TINY + giant, false));
  bodylist_add(torso->organs,
	       generate_part("Guts", 3, BLOOD_EMPTY, SIZE_TINY + giant, false));

  return torso;
}

bodylist_t *bodylist_new(){
  return Calloc(1, sizeof(bodylist_t));
}

/* Adds a given body part to the start of a bodylist.
 */
void bodylist_add(bodylist_t *list, body_part_t *part){
  bodylist_t *new = bodylist_new();
  
  new->part = list->part;
  new->next = list->next;
  list->next = new;
  list->part = part;
}

/* Removes a given body part from a given bodylist. If the part was not in the
 * given bodylist, this returns the same bodylist.
 */
bodylist_t *bodylist_remove_helper(bodylist_t *list, body_part_t* part){
  if(list == NULL || part == NULL){
    return list;}
  if(list->part == part){
    return list->next;
  }
  
  list->next = bodylist_remove_helper(list->next, part);
  return list;
}
void bodylist_remove(bodylist_t *list, body_part_t *part){
  list = bodylist_remove_helper(list, part);
}


/* Removes the specified body part from a given bodylist. If the list was 
 * unchanged, returns the same bodylist.
 */
bodylist_t *bodylist_remove_by_name_helper(bodylist_t *list, char *name){
  if(list == NULL || name == NULL){
    return list;
  }
  if(strcmp(list->part->name, name) == 0){
    return list->next;
  }

  list->next = bodylist_remove_by_name_helper(list->next, name);
  return list;
}
void bodylist_remove_by_name(bodylist_t *list, char *name){
  list = bodylist_remove_by_name_helper(list, name);
}

/* Should a body part be severed, this converts that part to an item, to be
 * dropped onto the floor.
 */
item_t *body_to_item(body_part_t *part){
  if(part == NULL){
    return NULL;
  }
  item_t *to_ret = Calloc(1, sizeof(item_t));

  if(part->name != NULL){
    to_ret->name = Calloc(strlen(part->name), sizeof(char));
    strcpy(to_ret->name, part->name);
  }
  else{
    to_ret->name = "Unknown";
  }

  to_ret->weight = part->size;
  to_ret->edible = true;

  if(rand()%100 == 0){
    to_ret->curse_lvl = 2;
  }
  
  return to_ret;
}

/* Free everything in and attached to a body part, as well as the boyd part
 *  itself.
 */
void body_part_free(body_part_t *part){
  if(part == NULL){
    return;}
  free(part->name);
  for(itemlist_t *cur = part->armor; cur != NULL;){
    itemlist_t *next = cur->next;
    free(cur);
    cur = next;
  }
  for(itemlist_t *cur = part->held; cur != NULL;){
    itemlist_t *next = cur->next;
    free(cur);
    cur = next;
  }
  for(itemlist_t *cur = part->stuck; cur != NULL;){
    itemlist_t *next = cur->next;
    free(cur);
    cur = next;
  }
  
  bodylist_free(part->attached);
  bodylist_free(part->organs);

  free(part);
}

/* Free everything in a bodylist and the bodylist itself
 */
void bodylist_free(bodylist_t *list){
  if(list != NULL){
    body_part_free(list->part);
    bodylist_free(list->next);
    free(list);
  }
}

void limb_list_helper(int depth, int *lines_used, body_part_t* body, int select,
		      int largest_size){
  if(body == NULL){
    return;
  }
  
  if(*lines_used == select){
    int odds = body_part_chance_to_hit(player, body, largest_size);
    
    //These colors show how relatively easy each part is to hit
    if(odds > 75){
      attron(COLOR_PAIR(CP_BLACK_GREEN));
    }
    else if(odds > 45){
      attron(COLOR_PAIR(CP_BLACK_YELLOW));
    }
    else{
      attron(COLOR_PAIR(CP_BLACK_RED));
    }
    char *odds_str = Calloc(TERMINAL_WIDTH/2, sizeof(char));
    sprintf(odds_str, "%d%% chance to hit", odds);
    mvaddstr(TERMINAL_HEIGHT - 2,
	     (TERMINAL_WIDTH/2 - strlen(odds_str))/2,
	     odds_str);
  }
  mvaddstr(2 + *lines_used, TERMINAL_WIDTH/2 + depth, body->name);
  (*lines_used)++;
  //Turn off the colors
  attroff(COLOR_PAIR(CP_BLACK_GREEN));
  attroff(COLOR_PAIR(CP_BLACK_YELLOW));
  attroff(COLOR_PAIR(CP_BLACK_RED));
  
  for(bodylist_t *cur = body->attached; cur != NULL; cur = cur->next){
    limb_list_helper(2+depth, lines_used, cur->part, select, largest_size);
  }
}
bool target_damage_helper(int *choice, struct creature_t *c,
			  body_part_t *part, int dmg, int dmg_type){
  if(choice == NULL || part == NULL){
    //No damage done in this case
    return false;
  }
  //Check if we're already there
  if(*choice == 0){
    //Start off by dealing the damage
    part->health -= dmg;
    //Assess the kind of damage done to the body part
    if(part->health <= 0){
      part->damage = DAMAGE_BROKEN;
      //Chance of severed body part, depending on size
      if(dmg_type == DMG_SLASHING && !(rand()%part->size)){
	//TODO: Sever the limb
      }
    }
    else if(part->health < part->health_max/4){
      part->damage = DAMAGE_CRIPPLED;
      //Chance of breaking, depending on size
      if(dmg_type == DMG_BLUNT && !(rand()%part->size)){
	part->damage = DAMAGE_BROKEN;
      }
    }
    else if(part->health < part->health_max/1.5){
      part->damage = DAMAGE_BRUISED;
    }

    if(part->health < 0 && part->vital){
      c->kill(c, cur_map);
    }

    return true;
  }
  (*choice)--;

  for(bodylist_t *cur = part->attached; cur != NULL; cur = cur->next){
    if(target_damage_helper(choice, c, cur->part, dmg, dmg_type)){
      //If damage was dealt to a part of this, subtract that damage from this
      part->health -= dmg;
      return true;
    }
  }

  //We never dealt damage along this path, so return false
  return false;
}
bool target_attack(){
  int dir = msg_promptchar("Target which direction?");
  int x = player->x, y = player->y;
  if(dir == cmd_data[CMD_UP] || dir == KEY_UP){
    y -= 1;}
  else if(dir == cmd_data[CMD_DOWN] || dir == KEY_DOWN){
    y += 1;}
  else if(dir == cmd_data[CMD_LEFT] || dir == KEY_LEFT){
    x -= 1;}
  else if(dir == cmd_data[CMD_RIGHT] || dir == KEY_RIGHT){
    x += 1;}
  else if(dir == cmd_data[CMD_UP_RIGHT]){
    x += 1; y -= 1;}
  else if(dir == cmd_data[CMD_UP_LEFT]){
    x -= 1; y -= 1;}
  else if(dir == cmd_data[CMD_DOWN_RIGHT]){
    x += 1; y += 1;
  }
  else if(cmd_data[CMD_DOWN_LEFT]){
    x -= 1; y += 1;}
  else{
    return false;
  }

  struct creature_t *target = get_creature_at_position(x,y, cur_map->creatures);
  if(target != NULL && target->body != NULL){
    int choice = -1;
    int selector = 0;

    clear();
    draw_borders();

    //Draw the top of the bounding box for the image
    mvaddch(1,2,'+');
    for(int i = 0; i < target->body->image_width+2; i++){
      addch(ACS_HLINE);
    }
    addch('+');
    mvaddch(2,2, ACS_VLINE); addch(' ');
    
    int num_lines = 0;
    int len = strlen(target->body->image);
    
    //Now draw the image
    for(int i = 0; i < len; i++){
      if(target->body->image[i] == '\n'){
	mvaddch(2+num_lines, 3 + target->body->image_width + 2, ACS_VLINE);
	num_lines++;
	mvaddch(2+num_lines, 2, ACS_VLINE); addch(' ');
	i++;
      }
      addch(target->body->image[i]);
    }
    
    //Finish the bounding box
    mvaddch(2+num_lines, 3 + target->body->image_width + 2, ACS_VLINE);
    mvaddch(3+num_lines, 2, '+');
    for(int i = 0; i < target->body->image_width+2; i++){
      addch(ACS_HLINE);
    }
    addch('+');
    
    //Info
    mvaddstr(4 + num_lines,
	     2 + (4 + target->body->image_width - strlen(target->name))/2,
	     target->name);
    mvaddstr(6 + num_lines,
	     2+ (4+ target->body->image_width - strlen("Pick your target"))/2,
	     "Pick your target.");
    
    //Input
    while(choice == -1){
      //List of limbs
      int lines = 0;
      limb_list_helper(0, &lines, target->body, selector, target->body->size);
      
      //Get user input
      int ch = Getch();
      if(ch == KEY_UP || ch == cmd_data[CMD_UP]){
	selector = selector == 0 ? 0 : selector - 1;
      }
      else if(ch == KEY_DOWN || ch == cmd_data[CMD_DOWN]){
	selector = selector == lines-1 ? selector : selector + 1;
      }
      else if(ch == KEY_ENTER || ch == '\n'){
	choice = selector;
      }
    }
    //TODO: Implement damage type
    target_damage_helper(&choice, target, target->body,
			 creature_get_damage(player),0);

    return true;
  }

  return false;
}

int body_part_chance_to_hit(struct creature_t *attacker,
			    body_part_t *part, int largest_size){
  if(attacker == NULL || part == NULL){
    return 0;}
  int result = 0;
  int cur_wep_skill = get_cur_weapon_skill();
  result += cur_wep_skill > 10 ? 30 : cur_wep_skill * 3;

  int diff = largest_size - part->size;
  if(part->size == SIZE_HUGE){
    result += 90;
  }
  else if(part->size == SIZE_LARGE){
    result += 75 + (diff * 10);
  }
  else if(part->size == SIZE_AVG){
    result += 50 + (diff * 7);
  }
  else if(part->size == SIZE_SMALL){
    result += 25 + ((3-diff) * 5);
  }
  else if(part->size == SIZE_TINY){
    result += 10 + ((4-diff) * 3);
  }

  int underneath = 0, count = 3;
  for(bodylist_t *cur = part->attached; cur != NULL; cur = cur->next){
    underneath += body_part_chance_to_hit(attacker, cur->part, largest_size);
    count++;
  }
  underneath = underneath / count;
  result += underneath;
  
  return result > 99 ? (get_luck(attacker) > 5 ? 100 : 99) : result;
}
