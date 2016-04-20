#include "classes.h"

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

void classes_data_init(){
  class_data[CLASS_UNKNOWN] = (struct class_t){
    .id = CLASS_UNKNOWN
  };
  class_data[CLASS_NEWB] = (struct class_t){
    .id = CLASS_NEWB,
    .name = "Newb",
    .luck_bonus = 2, //Beginner's Luck ;)
    .turn_tokens_starting_amount = 5
  };
  class_data[CLASS_WARRIOR] = (struct class_t){
    .id = CLASS_WARRIOR,
    .name = "Warrior",
    .strength_bonus = 1,
    .endurance_bonus = 1,
    .turn_tokens_starting_amount = 4,
    .dex_bonus = 1
  };
  class_data[CLASS_PIKEMAN] = (struct class_t){
    .id = CLASS_PIKEMAN,
    .name = "Pikeman",
    .strength_bonus = 1,
    .agility_bonus = 1,
    .turn_tokens_starting_amount = 4
  };
}

/* Returns the class name of the specified class. It is a reference to the 
 * original, not a copy.
 */
char *class_name(int class_id){
  if(class_id < 0 || class_id > CLASS_MAX){
    class_id = CLASS_UNKNOWN;
  }
  
  return class_data[class_id].name;
}

/* Returns how long members of this class can go before getting exhausted.
 */
int class_stamina(int class_id){
  if(class_id < 0 || class_id > CLASS_MAX){
    class_id = CLASS_UNKNOWN;
  }
  return class_data[class_id].turn_tokens_starting_amount;
}

int class_str_bonus(int class_id){
  if(class_id < 0 || class_id > CLASS_MAX){
    class_id = CLASS_UNKNOWN;
  }
  return class_data[class_id].strength_bonus;
}
  
int class_per_bonus(int class_id){
  if(class_id < 0 || class_id > CLASS_MAX){
    class_id = CLASS_UNKNOWN;
  }
  return class_data[class_id].perception_bonus;
}

int class_end_bonus(int class_id){
  if(class_id < 0 || class_id > CLASS_MAX){
    class_id = CLASS_UNKNOWN;
  }
  return class_data[class_id].endurance_bonus;
}

int class_cha_bonus(int class_id){
  if(class_id < 0 || class_id > CLASS_MAX){
    class_id = CLASS_UNKNOWN;
  }
  return class_data[class_id].charisma_bonus;
}
int class_int_bonus(int class_id){
  if(class_id < 0 || class_id > CLASS_MAX){
    class_id = CLASS_UNKNOWN;
  }
  return class_data[class_id].intelligence_bonus;
}
int class_agi_bonus(int class_id){
  if(class_id < 0 || class_id > CLASS_MAX){
    class_id = CLASS_UNKNOWN;
  }
  return class_data[class_id].agility_bonus;
}
int class_luc_bonus(int class_id){
  if(class_id < 0 || class_id > CLASS_MAX){
    class_id = CLASS_UNKNOWN;
  }
  return class_data[class_id].luck_bonus;
}
int class_dex_bonus(int class_id){
  if(class_id < 0 || class_id > CLASS_MAX){
    class_id = CLASS_UNKNOWN;
  }
  return class_data[class_id].dex_bonus;
}
