#include "bodies.h"
#include <stdbool.h>
#include <string.h>
#include "inventory.h"

body_part_t *generate_part(char *name, int health, int blood, int size, bool v){
  body_part_t *to_ret = Calloc(1, sizeof(body_part_t));

  //Add the info
  if(name != NULL){
    to_ret->name = Calloc(strlen(name), sizeof(char));
    strcpy(to_ret->name, name);
  }
  to_ret->health = health;
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

body_part_t *gen_rat(bool giant){
  return gen_rat_torso(giant);
}
body_part_t *gen_rat_torso(bool giant){
  body_part_t *torso = generate_part("Body", 5,
				     BLOOD_FULL,
				     SIZE_SMALL + giant,
				     true);

  torso->attached = bodylist_new();
  torso->organs = bodylist_new();
  bodylist_add(torso->attached,
	       generate_part("Left foreleg", 2, BLOOD_FULL,
			     SIZE_TINY + giant, false));
  bodylist_add(torso->attached,
	       generate_part("Right foreleg", 2, BLOOD_FULL,
			     SIZE_TINY + giant, false));
  bodylist_add(torso->attached,
	       generate_part("Left hindleg", 2, BLOOD_FULL,
			     SIZE_TINY + giant, false));
  bodylist_add(torso->attached,
	       generate_part("Right hindleg", 2, BLOOD_FULL,
			     SIZE_TINY + giant, false));
  bodylist_add(torso->attached,
	       generate_part("Tail", 1, 0, SIZE_TINY + giant, false));
  bodylist_add(torso->organs,
	       generate_part("Heart", 3, BLOOD_EMPTY,
			     SIZE_TINY + giant, false));
  bodylist_add(torso->organs,
	       generate_part("Lungs", 3, BLOOD_EMPTY,
			     SIZE_TINY + giant, false));
  bodylist_add(torso->organs,
	       generate_part("Guts", 3, BLOOD_EMPTY, SIZE_TINY + giant, false));
  body_part_t *head = generate_part("Head", 4, BLOOD_FULL,
				    SIZE_TINY + giant, false);
  head->organs = bodylist_new();
  bodylist_add(head->organs,
	       generate_part("Brain", 3, BLOOD_FULL, SIZE_TINY, false));
  bodylist_add(torso->attached, head);

  return torso;
}

bodylist_t *bodylist_new(){
  return Calloc(1, sizeof(bodylist_t));
}

/* Adds a given body part to the start of a bodylist.
 */
void bodylist_add(bodylist_t *list, body_part_t *part){
  bodylist_t *new = bodylist_new();
  
  new->part = part;
  new->next = list;
  list = new;
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
