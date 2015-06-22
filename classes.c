#include "classes.h"

void classes_data_init(){
  class_data[CLASS_UNKNOWN] = (struct class_t){
    .id = CLASS_UNKNOWN
  };
  class_data[CLASS_NEWB] = (struct class_t){
    .id = CLASS_NEWB,
    .name = "Newb",
    .luck_bonus = 1 //Beginner's Luck ;)
  };
  class_data[CLASS_WARRIOR] = (struct class_t){
    .id = CLASS_WARRIOR,
    .name = "Warrior",
    .strength_bonus = 1,
    .endurance_bonus = 1
  };
  class_data[CLASS_PIKEMAN] = (struct class_t){
    .id = CLASS_PIKEMAN,
    .name = "Pikeman",
    .strength_bonus = 1,
    .agility_bonus = 1
  };
}
