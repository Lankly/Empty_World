#ifndef CLASSES_H
#define CLASSES_H

#define CLASS_UNKNOWN 0
#define CLASS_NEWB 1
#define CLASS_WARRIOR 2
#define CLASS_PIKEMAN 3
#define CLASS_MAX 3

typedef struct class_t class_t;

#include "creature.h"

void classes_data_init();

char *class_name(int class_id);
int class_stamina(int class_id); // AKA turn_tokens

int class_str_bonus(int class_id);
int class_per_bonus(int class_id);
int class_end_bonus(int class_id);
int class_cha_bonus(int class_id);
int class_int_bonus(int class_id);
int class_agi_bonus(int class_id);
int class_luc_bonus(int class_id);
int class_dex_bonus(int class_id);

#endif
