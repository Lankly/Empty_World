#include "bodies.h"
#include <math.h>
#include <ncurses.h>
#include <stdbool.h>
#include <string.h>
#include "colors.h"
#include "creature.h"
#include "creature_callbacks.h"
#include "inventory.h"
#include "items.h"
#include "macros.h"
#include "map.h"
#include "status.h"

struct bodylist_t;
struct body_part_t{
  char *name;
  char *image;
  int image_width;
  bool bleeding;
  int blood_remaining;
  int health; //For HP
  int damage; //For wounds
  int health_max;
  int size;
  itemlist_t *armor;
  item_t *held;
  itemlist_t *stuck;

  bool vital;
  char *blunt_message;
  char *cold_message;
  char *crush_message;
  char *explode_message;
  char *fire_message;
  char *infect_message;
  char *pierce_message;
  char *psychic_message;
  char *slash_message;
  
  struct body_part_t *attached_to;
  struct bodylist_t *attached;
  struct bodylist_t *organs;
};

struct bodylist_t{
  body_part_t *part;
  struct bodylist_t *next;
  int num_parts;
};

body_part_t *generate_part(char *name, int health, int blood, int size, bool v){
  body_part_t *to_ret = Calloc(1, sizeof(body_part_t));

  //Add the info
  if(name != NULL){
    to_ret->name = Calloc(strlen(name)+1, sizeof(char));
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
  body_part_t *arm = generate_part(left ? "Left arm" : "Right arm", 55,
				   BLOOD_FULL, SIZE_LARGE, false);
  arm->attached = bodylist_new();

  body_part_t *upper =
    generate_part(left ? "Left upper arm" : "Right upper arm",
		  30,
		  BLOOD_FULL,
		  SIZE_AVG,
		  false);
  body_part_t *forearm = generate_part(left ? "Left forearm" : "Right forearm",
				       25, BLOOD_FULL, SIZE_AVG, false);
  forearm->attached = bodylist_new();
  bodylist_add(forearm->attached, gen_hand(left));

  bodylist_add(arm->attached, upper);
  bodylist_add(arm->attached, forearm);
  return arm;
}

body_part_t *gen_hand(bool left){
  body_part_t *hand = generate_part(left ? "Left hand" : "Right hand",
				    20,
				    BLOOD_FULL,
				    SIZE_SMALL,
				    false);
  hand->attached = bodylist_new();
  bodylist_add(hand->attached, generate_part("Thumb", 3,
					     BLOOD_INSIGNIFICANT,
					     SIZE_TINY, false));
  bodylist_add(hand->attached, generate_part("Pointer finger", 3,
					     BLOOD_INSIGNIFICANT,
					     SIZE_TINY, false));
  bodylist_add(hand->attached, generate_part("Middle finger", 3,
					     BLOOD_INSIGNIFICANT,
					     SIZE_TINY, false));
  bodylist_add(hand->attached, generate_part("Ring finger", 3,
					     BLOOD_INSIGNIFICANT,
					     SIZE_TINY, false));
  bodylist_add(hand->attached, generate_part("Little finger", 2,
					     BLOOD_INSIGNIFICANT,
					     SIZE_TINY, false));

  return hand;
}

body_part_t *gen_leg(bool left){
  body_part_t *leg = generate_part(left ? "Left leg" : "Right leg", 80,
				   BLOOD_FULL, SIZE_LARGE, false);
  leg->attached = bodylist_new();
  body_part_t *thigh = generate_part(left ? "Left thigh" : "Right thigh",
				     35,
				     BLOOD_FULL,
				     SIZE_LARGE,
				     false);

  bodylist_add(leg->attached, generate_part(left ? "Left foot" : "Right foot",
					    20, BLOOD_FULL, SIZE_SMALL, false));
  bodylist_add(leg->attached, generate_part(left ? "Left shin" : "Right shin",
					    25, BLOOD_FULL, SIZE_AVG, false));
  bodylist_add(leg->attached, thigh);
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

body_part_t *gen_fungus(bool giant){
  body_part_t *fungus = generate_part("Fungus", 3, BLOOD_NONE, SIZE_SMALL+giant,
				      true);
  fungus->attached = bodylist_new();

  body_part_t *cap = generate_part("Cap", 2, BLOOD_NONE, SIZE_TINY+giant,false);
  body_part_t *stem = generate_part("Stem",3, BLOOD_NONE, SIZE_TINY+giant,true);
  
  switch(rand() % 2){
  case 0:
    fungus->image =
      "         ___..._\n"
      "    _,--'       \"`-.\n"
      "  ,'.  .            \\\n"
      ",/:. .     .       .'\n"
      "|;..  .      _..--'\n"
      "`--:...-,-'""\\\n"
      "        |:.  `.\n"
      "        l;.   l\n"
      "        `|:.   |\n"
      "         |:.   `.,\n"
      "        .l;.    j, ,\n"
      "     `. \\`;:.   //,/\n"
      "      .\\\\)`;,|\\'/(\n"
      "       ` `itz `(,;";
    fungus->image_width = 21;
      break;
  default:
    fungus->image =
      "       __.....__\n"
      "    .'\" _  o    \"`.\n"
      "  .' O (_)     () o`.\n"
      " .           O       .\n"
      ". ()   o__...__    O  .\n"
      ". _.--\"\"\"       \"\"\"--._ .\n"
      ":\"                     \";\n"
      " `-.__    :   :    __.-'\n"
      "      \"\"\"-:   :-\"\"\"\n"
      "         J     L\n"
      "         :     :\n"
      "        J       L\n"
      "        :       :\n"
      "        `._____.' mh;";
    fungus->image_width = 25;
  }
  bodylist_add(fungus->attached, stem);
  bodylist_add(fungus->attached, cap);

  return fungus;
}

body_part_t *gen_hornet(bool giant){
  body_part_t *hornet = generate_part("Hornet", 8, BLOOD_FULL, SIZE_SMALL,true);
  hornet->attached = bodylist_new();
  
  body_part_t *head = generate_part("Head",4, BLOOD_FULL, SIZE_TINY+giant,true);

  body_part_t *body = generate_part("Body", 6, BLOOD_FULL, SIZE_TINY, false);
  body->attached = bodylist_new();
  bodylist_add(body->attached,
	       generate_part("Left hind leg", 1, BLOOD_FULL, SIZE_TINY, false));
  bodylist_add(body->attached,
	       generate_part("Left middle leg", 1, BLOOD_FULL,SIZE_TINY,false));
  bodylist_add(body->attached,
	       generate_part("Left foreleg", 1, BLOOD_FULL, SIZE_TINY, false));
  bodylist_add(body->attached,
	       generate_part("Right hind leg",1, BLOOD_FULL, SIZE_TINY, false));
  bodylist_add(body->attached,
	       generate_part("Right middle leg",1,BLOOD_FULL,SIZE_TINY, false));
  bodylist_add(body->attached,
	       generate_part("Right foreleg", 1, BLOOD_FULL, SIZE_TINY, false));
  bodylist_add(body->attached,
	       generate_part("Thorax", 1, BLOOD_FULL, SIZE_TINY, false));
  
  bodylist_add(hornet->attached, body);
  bodylist_add(hornet->attached,
	       generate_part("Left wing", 4, BLOOD_FULL, SIZE_TINY, false));
  bodylist_add(hornet->attached,
	       generate_part("Right wing", 4, BLOOD_FULL, SIZE_TINY, false));
  bodylist_add(hornet->attached, head);

  hornet->image =
    "                    .__.---- __.-7\n"
    "_                 /      /-\"    /\n"
    " \\          .--\" /     .^      /\n"
    "\\ \\  ___   / .-~\"_~\"  /       /\n"
    " \\.\\\" _/7-{ / /'~ ~ \\/  _._--^\n"
    " /_.-~ \\|~~Y Y      /--\"\n"
    "< _ __'_I__I |    _//  \\\\\n"
    " \\_/_.~ j I~-'   (_/    )Y\n"
    "   \">-'Y\\//\\     _.    ( j__\n"
    "   /7--l //\"\\j|_/// ._  _7  ~-.\n"
    "  //    \"/,^. | 7/--||-~__)    \\\n"
    " //     // //\"'//   l`\"~_ Y   .-Y\n"
    "L/     L/_//  //    ~7`  ||._/  |\n"
    "          L/ L/     /`   ||     !\n"
    "                   /`   //     /\n"
    "             \\.--\"`    L/   .^\n"
    "              \"-.____.----\"~";
  hornet->image_width = 34;
    
  return hornet;
}

body_part_t *gen_human(){
  body_part_t *human = generate_part("Human", 200, BLOOD_FULL, SIZE_LARGE, true);
  human->attached = bodylist_new();
  
  body_part_t *arms = generate_part("Arms", 110, BLOOD_FULL, SIZE_AVG, false);
  arms->attached = bodylist_new();
  bodylist_add(arms->attached, gen_arm(true));
  bodylist_add(arms->attached, gen_arm(false));
  
  body_part_t *legs = generate_part("Legs", 160, BLOOD_FULL, SIZE_AVG, false);
  legs->attached = bodylist_new();
  bodylist_add(legs->attached, gen_leg(true));
  bodylist_add(legs->attached, gen_arm(false));
  
  body_part_t *torso = gen_human_torso();
  body_part_t *head = gen_human_head();
  body_part_t *neck = generate_part("Neck", 20, BLOOD_FULL, SIZE_SMALL, true);

  bodylist_add(human->attached, legs); //80+80
  bodylist_add(human->attached, arms); //55+55
  bodylist_add(human->attached, torso); //80
  bodylist_add(human->attached, neck); //20
  bodylist_add(human->attached, head); //30

  return human;
}
body_part_t *gen_human_head(){
  body_part_t *head = gen_head();

  bodylist_add(head->attached,
	       generate_part("Left ear", 3, 0, SIZE_TINY, false));
  bodylist_add(head->attached,
	       generate_part("Right ear", 3, 0, SIZE_TINY, false));
  bodylist_add(head->attached,
	       generate_part("Nose", 3, 0, SIZE_TINY, false));
  bodylist_add(head->attached,
	       generate_part("Left eye", 3, 0, SIZE_TINY, false));
  bodylist_add(head->attached,
	       generate_part("Right eye", 3, 0, SIZE_TINY, false));

  head->health = 30;
  head->health_max = head->health;
  
  return head;
}
body_part_t *gen_human_torso(){
  body_part_t *torso = generate_part("Torso", 80, BLOOD_FULL, SIZE_LARGE, false);
  torso->attached = bodylist_new();
  
  body_part_t *thorax = generate_part("Chest", 50, BLOOD_FULL, SIZE_LARGE, true);
  thorax->attached = bodylist_new();
  
  thorax->organs = bodylist_new();
  bodylist_add(thorax->organs,
	       generate_part("Heart", 20, BLOOD_EMPTY, SIZE_SMALL, true));
  bodylist_add(thorax->organs,
	       generate_part("Lungs", 20, BLOOD_EMPTY, SIZE_AVG, true));
  
  body_part_t *abdomen = generate_part("Abdomen", 30,
				       BLOOD_FULL, SIZE_AVG, true);
  abdomen->organs = bodylist_new();
  
  bodylist_add(abdomen->organs,
	       generate_part("Stomach", 6, BLOOD_FULL, SIZE_SMALL, true));
  bodylist_add(abdomen->organs,
	       generate_part("Small intestine", 4,
			     BLOOD_EMPTY, SIZE_SMALL, true));
  bodylist_add(abdomen->organs,
	       generate_part("Large intestine", 4, BLOOD_EMPTY, SIZE_SMALL, true));
  bodylist_add(abdomen->organs,
	       generate_part("Liver", 3, BLOOD_EMPTY, SIZE_TINY, true));
  bodylist_add(abdomen->organs,
	       generate_part("Kidneys", 4, BLOOD_EMPTY, SIZE_TINY, true));
  
  bodylist_add(torso->attached, thorax);
  bodylist_add(torso->attached, abdomen);
  return torso;
}

body_part_t *gen_owl(bool giant){
  body_part_t *owl = generate_part("Cave Owl", 10,
				   BLOOD_FULL,
				   SIZE_SMALL + giant,
				   true);
  owl->attached = bodylist_new();
  
  body_part_t *head =
    generate_part("Head", 4, BLOOD_FULL, SIZE_TINY + giant, true);
  head->blunt_message = "bash its skull in";
  head->cold_message = "freeze its brain";
  head->crush_message = "crush its skull in";
  head->explode_message = "cause its head to explode";
  head->fire_message = "cook its brain through the skull";
  head->pierce_message = "pierce its brain through the skull";
  head->psychic_message = "destroy its tiny mind";
  head->slash_message = "cave its skull in";
  head->organs = bodylist_new();
  bodylist_add(head->organs,
	       generate_part("Brain", 3, BLOOD_FULL, SIZE_TINY, false));

  body_part_t *limbs = generate_part("Limbs", 12,
				     BLOOD_FULL, SIZE_SMALL + giant, false);
  limbs->attached = bodylist_new();
  bodylist_add(limbs->attached,
	       generate_part("Left wing",3,BLOOD_FULL, SIZE_TINY+giant, false));
  bodylist_add(limbs->attached,
	       generate_part("Right wing",3,BLOOD_FULL,SIZE_TINY+giant, false));
  bodylist_add(limbs->attached,
	       generate_part("Left leg", 3, BLOOD_FULL,SIZE_TINY+giant, false));
  bodylist_add(limbs->attached,
	       generate_part("Right leg",3, BLOOD_FULL,SIZE_TINY+giant, false));

  bodylist_add(owl->attached, limbs);
  bodylist_add(owl->attached, gen_rat_torso(giant));
  bodylist_add(owl->attached, head);

  owl->image =
    "                ____\n"
    "             ,''    ''.\n"
    "            / `-.  .-' \\\n"
    "           /( (O))((O) )\n"
    "          /'-..-'/\\`-..|\n"
    "        ,'\\   `-.\\/.--'|\n"
    "      ,' ( \\           |\n"
    "    ,'( (   `._        |\n"
    "   /( (  ( ( | `-._ _,-;\n"
    "  /( (  ( ( (|     '  ;\n"
    " / ((  (    /        /\n"
    "//         /        /\n"
    "//  / /  ,'        /\n"
    "// /    ,'         /\n"
    "//  / ,'          ;\n"
    "//_,-'          ;\n"
    "// /,,,,..-))-))\\";
  owl->image_width = 24;
  
  return owl;
}

body_part_t *gen_rat(bool giant){
  body_part_t *rat = generate_part("Rat", 10,
				   BLOOD_FULL,
				   SIZE_SMALL + giant,
				   true);
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
  
  
  body_part_t *head =
    generate_part("Head", 4, BLOOD_FULL, SIZE_TINY + giant, true);
  head->blunt_message = "bash its skull in";
  head->cold_message = "freeze its brain";
  head->crush_message = "crush its skull in";
  head->explode_message = "cause its head to explode";
  head->fire_message = "cook its brain through the skull";
  head->pierce_message = "pierce its brain through the skull";
  head->psychic_message = "destroy its tiny mind";
  head->slash_message = "cave its skull in";
  head->organs = bodylist_new();
  bodylist_add(head->organs,
	       generate_part("Brain", 3, BLOOD_FULL, SIZE_TINY, false));

  item_t *teeth = Calloc(1, sizeof(item_t));
  teeth->display = '"';
  teeth->exam_text = "This is the front teeth of a rat.";
  teeth->dmg_verb = "bite";
  teeth->size = 1;
  teeth->material = MAT_BONE;
  teeth->health = 10;
  teeth->damage = 4;
  teeth->damage_type = DMG_SLASHING;
  head->held = teeth;
  
  bodylist_add(rat->attached, limbs);
  bodylist_add(rat->attached, gen_rat_torso(giant));
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

  torso->blunt_message = "destroy its rib cage";
  torso->cold_message = "freeze its heart";
  torso->crush_message = "crush its rib cage";
  torso->explode_message = "explode it from the inside out";
  torso->fire_message = "burn it to death";
  torso->infect_message = "give it high cholesterol";
  torso->pierce_message = "stab into its chest and pierce its heart";
  torso->psychic_message = "will its heart to stop";
  torso->slash_message = "slash its torso open";
  
  return torso;
}

bodylist_t *bodylist_new(){
  return Calloc(1, sizeof(bodylist_t));
}

/* Adds a given body part to the start of a bodylist.
 */
void bodylist_add(bodylist_t *list, body_part_t *part){
  if(list == NULL){
    quit("Can't add body part to NULL bodylist.");
  }
  bodylist_t *new = bodylist_new();
  
  new->part = list->part;
  new->next = list->next;
  list->next = new;
  list->part = part;
  list->num_parts++;
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

/* Returns an item that is associated with the given body. If the corpse
 * cannot be created, returns NULL.
 */
item_t *create_corpse(creature_t *c){
  body_part_t *part;
  if(c == NULL || (part = creature_get_body(c)) == NULL){
    return NULL;
  }
  
  item_t *corpse = Calloc(1, sizeof(item_t));
  corpse->corpse = part;
  switch(part->size){
  case (SIZE_TINY):
    corpse->weight = 1;
    break;
  case (SIZE_SMALL):
    corpse->weight = 5;
    break;
  case (SIZE_AVG):
    corpse->weight = 10;
    break;
  case (SIZE_LARGE):
    corpse->weight = 20;
    break;
  case (SIZE_HUGE):
    corpse->weight = 50;
    break;
  }

  corpse->health = part->health + 1;
  corpse->damage = part->size;
  corpse->damage_type = DMG_BLUNT;
  corpse->edible = true;
  corpse->curse_lvl = !(rand()%100) ? BUC_CURSED : BUC_UNCURSED;
  corpse->display = COLOR_PAIR(CP_CORPSE) | (char)creature_get_display(c);
  corpse->exam_text = Calloc(MAX_MSG_LEN, sizeof(char));
  sprintf(corpse->exam_text, "This is the corpse of a %s.",
	  str_lowercase(part->name));
  
  char *corpse_name = Calloc(MAX_NAME_LEN, sizeof(char));
  sprintf(corpse_name, "%s corpse",part->name != NULL ? part->name : "Unknown");
  item_set_name(corpse, corpse_name);
  free(corpse_name);
  
  return corpse;
}

/* Returns the held item that deals the most damage. If no such weapon exists,
 * returns NULL.
 */
item_t *get_weapon(body_part_t *part){
  if(part == NULL){
    return NULL;
  }

  item_t *weapon = part->held;
  for(bodylist_t *cur = part->attached; cur != NULL; cur = cur->next){
    item_t *other_wep = get_weapon(cur->part);
    if(other_wep != NULL &&
       (weapon == NULL || weapon->damage < other_wep->damage)){
      weapon = other_wep;
    }
  }

  return weapon;
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
  if(part->held != NULL){
    free(part->held);
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

/* Draws the image associated with a creature onto either the left or right
 * half of the screen. Assumes that the image will fit in half the screen.
 * Returns the height of the image.
 */
int draw_body_image(creature_t *creature, bool left){
  body_part_t *body;
  if(creature == NULL || (body = creature_get_body(creature)) == NULL){
    return 0;
  }
  
  int padding = left ? 0 : TERMINAL_WIDTH/2;
  padding += (TERMINAL_WIDTH/2 - body->image_width - 4)/2;

  int y0, x0;
  get_centered_box_ul_coord(&y0, &x0, TERMINAL_HEIGHT, TERMINAL_WIDTH);

  //Draw the top of the bounding box for the image
  mvaddch(y0 + 1, x0 + padding,'+');
  for(int i = 0; i < body->image_width+2; i++){
    addch(ACS_HLINE);
  }
  addch('+');
  mvaddch(y0 + 2, x0 + padding, ACS_VLINE); addch(' ');
  
  int num_lines = 0;
  int len = strlen(body->image);
  
  //Now draw the image
  for(int i = 0; i < len; i++){
    if(body->image[i] == '\n'){
      mvaddch(y0 + 2 + num_lines,
	      x0 + 1 + padding + body->image_width + 2,
	      ACS_VLINE);
      num_lines++;
      mvaddch(y0 + 2 + num_lines, x0 + padding, ACS_VLINE); addch(' ');
      i++;
    }
    addch(body->image[i]);
  }
  
  //Finish the bounding box
  mvaddch(y0 + 2 + num_lines,
	  x0 + 1 + padding + body->image_width + 2,
	  ACS_VLINE);
  mvaddch(y0 + 3 + num_lines, x0 + padding, '+');
  for(int i = 0; i < body->image_width+2; i++){
    addch(ACS_HLINE);
  }
  addch('+');

  return num_lines;
}

/* Deals damage to a creature's body part. Also handles killing them.
 */
bool damage_body_part(int *choice, creature_t *attacker,
		      creature_t *target, body_part_t *part,
		      int dmg, int dmg_type){
  if(choice == NULL || part == NULL){
    //No damage done yet
    return false;
  }
  
  //Check if we're already there
  if(*choice <= 0){
    
    /* Start off by attempting to deal the damage (if a super-part was hit, this
     * is a guaranteed hit because choice will be -1)
     */
    if(*choice == -1
       || (rand()%100) <= body_part_chance_to_hit(attacker, target, part)){
      
      //If the part has sub-parts, hit a sub-part
      if(part->attached != NULL && part->attached->num_parts != 0){
	*choice = -1;
	bool done = false;
	while(!done){
	  int rv = rand() % part->attached->num_parts;
	  bodylist_t *cur = part->attached;
	  while(rv > 0){
	    cur = cur->next;
	    rv--;
	  }
	  /* Don't want all sub-parts to have an equal chance of getting hit,
	   * so we just keep trying to hit them until it happens.
	   */
	  int c2h = body_part_chance_to_hit(attacker, target, cur->part);
	  if(c2h > 0 && rand() % c2h == 0){
	    //Extra damage to parts shouldn't affect upwards
	    if(part->health > 0){
	      part->health -= dmg > part->health ? part->health : dmg;
	    }
      
	    damage_body_part(choice, attacker, target,
			     cur->part, dmg, dmg_type);
	    done = true;
	  }
	}
      }
      //If it didn't, then show a damage message.
      else{
	part->health -= dmg;
	char *dmg_verb = get_dmg_verb(get_weapon(creature_get_body(attacker)));
      
	if(dmg == 0 && attacker == player){
	  msg_add("You deal no damage.");
	}
	else if(dmg == 0){
	  if(attacker == NULL){
	    msg_add("It deals no damage.");
	  }
	  else{
	    msg_addf("The %s deals no damage.", creature_get_name(attacker));
	  }
	}
	else if(attacker == player){
	  msg_addf("You %s the %s's %s.", dmg_verb, creature_get_name(target),
		   str_lowercase(part->name));
	}
	else if(target == player){
	  msg_addf("The %s %ss your %s.", creature_get_name(attacker), dmg_verb,
		   str_lowercase(part->name));
	}
	else if(attacker == NULL){
	  msg_addf("It %ss the %s.", dmg_verb, creature_get_name(target));
	}
	else{
	  msg_addf("The %s %ss the %s.", creature_get_name(attacker), dmg_verb,
		   creature_get_name(target));
	}
      }

      //Check for death
      if(part->health < 0 && part->vital){
	//Say how they died
	char *message = Calloc(MAX_MSG_LEN+1, sizeof(char));
	if(attacker == player){
	  strcat(message, "You ");
	}
	else if(attacker == NULL){
	  strcat(message, "It ");
	}
	else{
	  strcat(message, "The ");
	  strncat(message, creature_get_name(target),
		  MAX_MSG_LEN - strlen(message));
	  strncat(message, " ", MAX_MSG_LEN - strlen(message));
	}
	
	if(dmg_type == DMG_BLUNT && part->blunt_message != NULL){
	  strncat(message, part->blunt_message, MAX_MSG_LEN - strlen(message));
	}
	else if (dmg_type == DMG_COLD && part->cold_message != NULL){
	  strncat(message, part->cold_message, MAX_MSG_LEN - strlen(message));
	}
	else if (dmg_type == DMG_CRUSHING && part->crush_message != NULL){
	  strncat(message, part->crush_message, MAX_MSG_LEN - strlen(message));
	}
	else if (dmg_type == DMG_EXPLOSIVE && part->explode_message != NULL){
	  strncat(message, part->explode_message,
		  MAX_MSG_LEN - strlen(message));
	}
	else if (dmg_type == DMG_FIRE && part->fire_message != NULL){
	  strncat(message, part->fire_message, MAX_MSG_LEN - strlen(message));
	}
	else if (dmg_type == DMG_INFECTION && part->infect_message != NULL){
	  strncat(message, part->infect_message, MAX_MSG_LEN - strlen(message));
	}
	else if (dmg_type == DMG_PIERCING && part->pierce_message != NULL){
	  strncat(message, part->pierce_message, MAX_MSG_LEN - strlen(message));
	}
	else if (dmg_type == DMG_PSYCHIC && part->psychic_message != NULL){
	  strncat(message, part->psychic_message,
		  MAX_MSG_LEN - strlen(message));
	}
	else if (dmg_type == DMG_SLASHING && part->slash_message != NULL){
	  strncat(message, part->slash_message, MAX_MSG_LEN - strlen(message));
	}
	else if(target == player){
	  strncpy(message, "You succumb to your injuries",
		  MAX_MSG_LEN - strlen("You succumb to your injuries"));
	}
	else{
	  strncpy(message, "It succumbs to its injuries",
		  MAX_MSG_LEN - strlen("It succumbs to its injuries"));
	}
	strncat(message, ".", MAX_MSG_LEN - strlen(message));

	msg_add(message);
	free(message);
	
	//Kill callback
	creature_kill(target, cur_map);
      }
    }
    else{
      if(attacker == player){
	if(!creature_is_visible(target, attacker)){
	  msg_add("You miss it.");
	}
	else{
	  msg_addf("You miss the %s.", creature_get_name(target));
	}
      }
      else if(target == player){
	if(!creature_is_visible(attacker, target)){
	  msg_add("It misses you.");
	} 
	else{
	  msg_addf("The %s misses you.", creature_get_name(attacker));
	}
      }
      else{
	if(!creature_is_visible(target, attacker)){
	  msg_addf("It misses the %s.", creature_get_name);
	}
	msg_addf("The %s misses the %s.", creature_get_name(attacker),
		 creature_get_name(target));
      }
    }
    return true; //Return that the attack is done
    
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

    //Damage messages
    if(part->damage == DAMAGE_BROKEN && target == player){
      msg_addf("Your %s is broken!", str_lowercase(part->name));
    }
    else if(part->damage == DAMAGE_BROKEN){
      msg_addf("The %s's %s is broken!",
	       creature_get_name(target),
	       str_lowercase(part->name));
    }

    return true;
  }
  (*choice)--;

  for(bodylist_t *cur = part->attached; cur != NULL; cur = cur->next){
    if(damage_body_part(choice, attacker, target, cur->part, dmg, dmg_type)){
      //If damage was dealt to a part of this, subtract that damage from this
      part->health -= dmg;
      return true;
    }
  }

  //We never dealt damage along this path, so return false
  return false;
}

/* Finds an asked-for body part in the hierarchy of body parts. If the part is
 * not found, returns NULL.
 */
body_part_t *get_body_part_by_name(body_part_t *part, char *name){
  if(part == NULL || name == NULL){
    return NULL;
  }

  //If this is the part, we're done
  if(strcmp(part->name, name) == 0){
    return part;
  }

  //Otherwise, check all sub-parts
  for(bodylist_t *cur = part->attached; cur != NULL; cur = cur->next){
    body_part_t *res = get_body_part_by_name(cur->part, name);
    if(res != NULL){
      return res;
    }
  }
  for(bodylist_t *cur = part->organs; cur != NULL; cur = cur->next){
    body_part_t *res = get_body_part_by_name(cur->part, name);
    if(res != NULL){
      return res;
    }
  }

  //If it wasn't found, return NULL
  return NULL;
}

/* Returns the item that's being held by the given body part. If the given
 * part does not hold an item, even if there is an item being held by a
 * sub-part, returns NULL.
 */
item_t *body_part_get_held_item(body_part_t *part){
  return part == NULL ? NULL : part->held;
}

void limb_list_helper(struct creature_t *target, int depth, int *lines_used,
		      body_part_t* body, int select, int largest_size){
  if(body == NULL){
    return;
  }

  int y0, x0;
  get_centered_box_ul_coord(&y0, &x0, TERMINAL_HEIGHT, TERMINAL_WIDTH);
  
  if(*lines_used == select){
    int odds = body_part_chance_to_hit(player, target, body);
    
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
    mvaddstr(y0 + TERMINAL_HEIGHT - 2,
	     x0 + (TERMINAL_WIDTH/2 - strlen(odds_str) + 2)/2,
	     odds_str);
    free(odds_str);
  }
  mvaddstr(y0 + 2 + *lines_used, x0 + TERMINAL_WIDTH/2 + depth, body->name);
  (*lines_used)++;
  //Turn off the colors
  attroff(COLOR_PAIR(CP_BLACK_GREEN));
  attroff(COLOR_PAIR(CP_BLACK_YELLOW));
  attroff(COLOR_PAIR(CP_BLACK_RED));
  
  for(bodylist_t *cur = body->attached; cur != NULL; cur = cur->next){
    limb_list_helper(target, 2+depth, lines_used, cur->part, select, largest_size);
  }
}
bool target_attack(){
  int dir = msg_promptchar("Target which direction?");
  int x, y;
  creature_get_coord(player, &x, &y);
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

  clist_t *creatures = map_get_creatures(cur_map);
  creature_t *target = get_creature_at_position(x,y, creatures);
  body_part_t *body = creature_get_body(target);
  if(target != NULL && body != NULL){
    int choice = -1;
    int selector = 0;

    clear();
    draw_borders();
    int num_lines = draw_body_image(target, true);
    int padding = (TERMINAL_WIDTH/2 - body->image_width - 4)/2;

    int y0, x0;
    get_centered_box_ul_coord(&y0, &x0, TERMINAL_HEIGHT, TERMINAL_WIDTH);
    
    //Info
    mvaddstr(y0 + 4 + num_lines,
	     x0 + padding + (4 + body->image_width - strlen(body->name))/2,
	     body->name);
    mvaddstr(y0 + 5 + num_lines,
	     x0 + padding + (4+ body->image_width
			- strlen("Pick your target"))/2,
	     "Pick your target.");
    
    //Input
    while(choice == -1){
      //List of limbs
      int lines = 0;
      limb_list_helper(target, 0, &lines, body,
		       selector, body->size);
      
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
    damage_body_part(&choice, player, target, body,
		     creature_get_damage(player),0);

    return true;
  }

  return false;
}

/* Returns an integer amount representing how healthy a given body part is.
 * If this part is the topmost part, you'll get an indication of how well
 * the creature is doing overall, though that could change quickly with
 * damage to a vital part.
 * If the given part is NULL, returns -1.
 */
int body_part_health(body_part_t *part){
  return part == NULL ? -1 : part->health;
}

int body_part_chance_to_hit(creature_t *attacker,
			    creature_t *target,
			    body_part_t *part){
  body_part_t *tbody;
  if(attacker == NULL || target == NULL || part == NULL
     || (tbody = creature_get_body(target)) == NULL){
    return 0;
  }

  int largest_size = tbody->size;
  int result = 0;
  int cur_wep_skill = creature_get_skill_with_weapon(attacker);
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

  if(creature_is_immobile(target) && result < 80){
    result = 80;
  }
  
  int underneath = 0, count = 3;
  for(bodylist_t *cur = part->attached; cur != NULL; cur = cur->next){
    underneath += body_part_chance_to_hit(attacker, target, cur->part);
    count++;
  }
  underneath = underneath / count;
  result += underneath;

  
  return result > 99 ? (get_luck(attacker) > 5 ? 100 : 99) : result;
}
