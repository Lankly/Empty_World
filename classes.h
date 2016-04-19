#ifndef CLASSES_H
#define CLASSES_H

#define CLASS_UNKNOWN 0
#define CLASS_NEWB 1
#define CLASS_WARRIOR 2
#define CLASS_PIKEMAN 3
#define CLASS_MAX 3

typedef struct class_t class_t;

#include "creature.h"

struct class_t{
  int id;
  int display_color;
  char *name;
  
  int strength_bonus;
  int perception_bonus;
  int endurance_bonus;
  int charisma_bonus;
  int intelligence_bonus;
  int agility_bonus;
  int luck_bonus;
  int dex_bonus;
  
  int turn_tokens_starting_amount;
};

class_t class_data[CLASS_MAX+1];

void classes_data_init();

#endif
