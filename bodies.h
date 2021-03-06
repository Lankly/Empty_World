#ifndef BODIES_H
#define BODIES_H

typedef struct body_part_t body_part_t;
typedef struct bodylist_t bodylist_t;

#include <stdbool.h>
#include "inventory.h"

#define SIZE_TINY 0
#define SIZE_SMALL 1
#define SIZE_AVG 2
#define SIZE_LARGE 3
#define SIZE_HUGE 4

#define BLOOD_FULL 5
#define BLOOD_THREE_QUARTERS 4
#define BLOOD_HALF 3
#define BLOOD_QUARTER 2
#define BLOOD_EMPTY 1
#define BLOOD_INSIGNIFICANT 0
#define BLOOD_NONE -1 //Means that the creature doesn't have blood

#define DAMAGE_UNHURT 0
#define DAMAGE_BRUISED 1
#define DAMAGE_CRIPPLED 2
#define DAMAGE_BROKEN 3
#define DAMAGE_SEVERED 4

body_part_t *generate_part(char *name, int h, int b, int s, bool v);

body_part_t *gen_arm(bool left);
body_part_t *gen_hand(bool left);
body_part_t *gen_leg(bool left);
body_part_t *gen_head();

body_part_t *gen_cat(bool giant);
body_part_t *gen_cat_torso(bool giant);
body_part_t *gen_fungus(bool giant);
body_part_t *gen_hornet(bool giant);
body_part_t *gen_human();
body_part_t *gen_human_head();
body_part_t *gen_human_torso();
body_part_t *gen_owl(bool giant);
body_part_t *gen_rat(bool giant);
body_part_t *gen_rat_torso(bool giant);

bodylist_t *bodylist_new();
void bodylist_add(bodylist_t *list, body_part_t *part);
void bodylist_remove(bodylist_t *list, body_part_t *part);
void bodylist_remove_by_name(bodylist_t *list, char *name);

int draw_body_image(struct creature_t *c, bool left);

bool damage_body_part(int *choice, creature_t *attacker, creature_t *target,
		      body_part_t *part, int dmg, int dmg_type);
body_part_t *get_body_part_by_name(body_part_t *part, char *name);
item_t *body_part_get_held_item(body_part_t *part);
int body_part_chance_to_hit(creature_t *attacker, creature_t *target,
			    body_part_t *part);
int body_part_health(body_part_t *part);

void body_part_free(body_part_t *part);
void bodylist_free(bodylist_t *list);

item_t *body_to_item(body_part_t *part);
item_t *create_corpse(struct creature_t *c);
item_t *get_weapon(body_part_t *part);

bool target_attack();

#endif
