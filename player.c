#include "player.h"

int player_add_breathable(int type){
  //Make sure everything is right
  if(player==NULL){quit("ERROR: NULL Player");}
  if(player->breathables==NULL){quit("ERROR: NULL Player_Breathables");}
  if(type<0 || type>BREATHE_MAX){return 1;}

  breathe_node_t* next = player->breathables->first;
  //Case where adding first breathable
  if(next==NULL){
    next=(breathe_node_t*)Calloc(1,sizeof(breathe_node_t));
    next->breathe_type=type;
    return 0;
  }
  //Otherwise navigate to the end and add it, making sure the type isn't already in the list
  while(next->next!=NULL){
    if(next->breathable_type==type){return 1;}
    next=next->next;
  }
  next->next(breathe_node_t*)Calloc(1,sizeof(breathe_node_t));
  next->next->breathe_type=type;
  return 0;
}

int player_add_consumable(int type){
  //Make sure everything is right
  if(player==NULL){quit("ERROR: NULL Player");}
  if(player->consumables==NULL){quit("ERROR: NULL Player_Consumables");}
  if(type<0 || type>CONSUMABLE_MAX){return 1;}

  consumable_node_t* next = player->consumables->first;
  //Case where adding first consumable
  if(next==NULL){
    next=(consumable_node_t*)Calloc(1,sizeof(consumable_node_t));
    next->consumable_type=type;
    return 0;
  }
  //Otherwise navigate to the end and add it, making sure the type isn't already in the list
  while(next->next!=NULL){
    if(next->consumable_type==type){return 1;}
    next=next->next;
  }
  next->next(consumable_node_t*)Calloc(1,sizeof(consumable_node_t));
  next->next->consumable_type=type;
  return 0;
}

int player_add_intrinsic(int type){
  //Make sure everything is right
  if(player==NULL){quit("ERROR: NULL Player");}
  if(player->intrinsics==NULL){quit("ERROR: NULL Player_Intrinsics");}
  if(type<0 || type>TRINSIC_MAX){return 1;}

  intrinsic_node_t* next = player->intrinsics->first;
  //Case where adding first intrinsic
  if(next==NULL){
    next=(intrinsic_node_t*)Calloc(1,sizeof(intrinsic_node_t));
    next->intrinsic_type=type;
    return 0;
  }
  //Otherwise navigate to the end and add it, making sure the type isn't already in the list
  while(next->next!=NULL){
    if(next->intrinsic_type==type){return 1;}
    next=next->next;
  }
  next->next(intrinsic_node_t*)Calloc(1,sizeof(intrinsic_node_t));
  next->next->intrinsic_type=type;
  return 0;
}

int player_add_resistance(int type){
  //Make sure everything is right
  if(player==NULL){quit("ERROR: NULL Player");}
  if(player->resistances==NULL){quit("ERROR: NULL Player_Resistance");}
  if(type<0 || type>DMG_MAX){return 1;}

  resistance_node_t* next = player->resistances->first;
  //Case where adding first resistance
  if(next==NULL){
    next=(resistance_node_t*)Calloc(1,sizeof(resistance_node_t));
    next->resistance_type=type;
    return 0;
  }
  //Otherwise navigate to the end and add it, making sure the type isn't already in the list
  while(next->next!=NULL){
    if(next->resistance_type==type){return 1;}
    next=next->next;
  }
  next->next(resistance_node_t*)Calloc(1,sizeof(resistance_node_t));
  next->next->resistance_type=type;
  return 0;
}

int player_remove_breathable(int type){
  //Make sure everything is right
  if(player==NULL){quit("ERROR: NULL Player");}
  if(player->breathables==NULL || player->breathables->first==NULL){return 0;}
  if(type<0 || type>BREATHE_MAX){return 1;}

  breathe_node_t* next = player->breathables->first;
  //If it's the first node
  if(next->breathe_type==type){
    player->breathables->first=next->next;
    free(next);
    return 0;
  }
  //Loop through to find it
  while(next->next!=NULL){
    if(next->next->breathe_type==type){
      breathe_node_t* temp = next->next;
      next->next=next->next->next;
      free(temp);
      return 0;
    }
  }
  //Not found
  return 1;
}

int player_remove_consumable(int type){
  //Make sure everything is right
  if(player==NULL){quit("ERROR: NULL Player");}
  if(player->consumables==NULL || player->consumables->first==NULL){return 0;}
  if(type<0 || type>CONSUME_MAX){return 1;}

  consume_node_t* next = player->consumables->first;
  //If it's the first node
  if(next->consume_type==type){
    player->consumables->first=next->next;
    free(next);
    return 0;
  }
  //Loop through to find it
  while(next->next!=NULL){
    if(next->next->consume_type==type){
      consume_node_t* temp = next->next;
      next->next=next->next->next;
      free(temp);
      return 0;
    }
  }
  //Not found
  return 1;
}

int player_remove_instrinsic(int type){
  //Make sure everything is right
  if(player==NULL){quit("ERROR: NULL Player");}
  if(player->instrinsics==NULL || player->instrinsics->first==NULL){return 0;}
  if(type<0 || type>TRINSIC_MAX){return 1;}

  intrinsic_node_t* next = player->instrinsics->first;
  //If it's the first node
  if(next->intrinsic_type==type){
    player->instrinsics->first=next->next;
    free(next);
    return 0;
  }
  //Loop through to find it
  while(next->next!=NULL){
    if(next->next->intrinsic_type==type){
      intrinsic_node_t* temp = next->next;
      next->next=next->next->next;
      free(temp);
      return 0;
    }
  }
  //Not found
  return 1;
}

int player_remove_resistance(int type){
  //Make sure everything is right
  if(player==NULL){quit("ERROR: NULL Player");}
  if(player->resistances==NULL || player->resistances->first==NULL){return 0;}
  if(type<0 || type>DMG_MAX){return 1;}

  resist_node_t* next = player->resistances->first;
  //If it's the first node
  if(next->resist_type==type){
    player->resistances->first=next->next;
    free(next);
    return 0;
  }
  //Loop through to find it
  while(next->next!=NULL){
    if(next->next->resist_type==type){
      resist_node_t* temp = next->next;
      next->next=next->next->next;
      free(temp);
      return 0;
    }
  }
  //Not found
  return 1;
}

void player_init(char* name){
  player = (player_t*)Calloc(1,sizeof(player));
  player->strength = 1;
  player->perception = 1;
  player->endurance = 1;
  player->charisma = 1;
  player->intelligence = 1;
  player->luck = 1;
  player->health = 10;
  player->hunger = 1;
  player->gold = 5;
  player->level = 1;
  player->name = name;

  player->breathables = (breathe_list_t*)Calloc(1,sizeof(breathe_list_t));
  player_add_breathable(BREATHE_AIR);
  player->consumables = (consume_list_t*)Calloc(1,sizeof(consume_list_t));
  player_add_consumable(CONSUME_ANIMAL);
  player_add_consumable(CONSUME_INSECT);
  player_add_consumable(CONSUME_FOOD);
  player_add_consumable(CONSUME_DRUG);
  player_add_consumable(CONSUME_WATER);
  player_add_consumable(CONSUME_ALCOHOL);
  player_add_consumable(CONSUME_POTION);
  player->intrinsics  = (intrinsics_list_t*)Calloc(1,sizeof(intrinsics_list_t));
  player->resistances = (resistances_list_t*)Calloc(1,sizeof(resistances_list_t));
}
