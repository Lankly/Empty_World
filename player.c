#include "player.h"
#include "creature.h"
#include <stdlib.h>

int player_add_breathable(int type){
  //Make sure everything is right
  if(player==NULL){quit("ERROR: NULL Player");}
  if(player->breathables==NULL){quit("ERROR: NULL Player_Breathables");}
  if(type<0 || type>BREATHE_MAX){
    return 1;
}
  
  struct breathe_node_t* next = player->breathables->first;
  //Case where adding first breathable
  if(next==NULL){
    next=(struct breathe_node_t*)Calloc(1,sizeof(struct breathe_node_t));
    next->breathe_type=type;
    return 0;
  }
  /* Otherwise navigate to the end and add it, making sure 
   * the type isn't already in the list
   */
  while(next->next!=NULL){
    if(next->breathe_type==type){return 1;}
    next=next->next;
  }
  next->next =
    (struct breathe_node_t*)Calloc(1,sizeof(struct breathe_node_t));
  next->next->breathe_type=type;
  return 0;
}

int player_add_consumable(int type){
  //Make sure everything is right
  if(player==NULL){
    quit("ERROR: NULL Player");}
  if(player->consumables==NULL){
    quit("ERROR: NULL Player_Consumables");}
  if(type<0 || type>CONSUME_MAX){return 1;}

  struct consume_node_t* next = player->consumables->first;
  //Case where adding first consumable
  if(next==NULL){
    next = (consume_node_t*)Calloc(1,sizeof(consume_node_t));
    next->consume_type = type;
    return 0;
  }
  /* Otherwise navigate to the end and add it, making sure 
   * the type isn't already in the list
   */
  while(next->next!=NULL){
    if(next->consume_type==type){return 1;}
    next=next->next;
  }
  next->next = (consume_node_t*)Calloc(1,sizeof(consume_node_t));
  next->next->consume_type = type;
  return 0;
}

int player_add_intrinsic(int type){
  //Make sure everything is right
  if(player==NULL){quit("ERROR: NULL Player");}
  if(player->intrinsics==NULL){quit("ERROR: NULL Player_Intrinsics");}
  if(type<0 || type>TRINSIC_MAX){return 1;}

  struct intrinsics_node_t* next = player->intrinsics->first;
  //Case where adding first intrinsic
  if(next==NULL){
    next=(struct intrinsics_node_t*)
      Calloc(1,sizeof(struct intrinsics_node_t));
    next->intrinsic_type=type;
    return 0;
  }
  /* Otherwise navigate to the end and add it, making sure 
   * the type isn't already in the list
   */
  while(next->next!=NULL){
    if(next->intrinsic_type==type){return 1;}
    next=next->next;
  }
  next->next = 
    (struct intrinsics_node_t*)Calloc(1,sizeof(struct intrinsics_node_t));
  next->next->intrinsic_type=type;
  return 0;
}

int player_add_resistance(int type){
  //Make sure everything is right
  if(player==NULL){quit("ERROR: NULL Player");}
  if(player->resistances==NULL){quit("ERROR: NULL Player_Resistance");}
  if(type<0 || type>DMG_MAX){return 1;}

  struct resistances_node_t* next = player->resistances->first;
  //Case where adding first resistance
  if(next==NULL){
    next = (struct resistances_node_t*)
      Calloc(1,sizeof(resistances_node_t));
    next->damage_type=type;
    return 0;
  }
  /*Otherwise navigate to the end and add it, making sure 
   * the type isn't already in the list
   */
  while(next->next!=NULL){
    if(next->damage_type==type){return 1;}
    next=next->next;
  }
  next->next = (resistances_node_t*)Calloc(1,sizeof(resistances_node_t));
  next->next->damage_type=type;
  return 0;
}

int player_remove_breathable(int type){
  //Make sure everything is right
  if(player==NULL){
    quit("ERROR: NULL Player");}
  /* In this function, if there are no items to remove, we react as if we have
   * successfully removed the item
   */
  if(player->breathables==NULL || player->breathables->first==NULL){
    return 0;}
  if(type<0 || type>BREATHE_MAX){return 1;}

  breathe_node_t* next = player->breathables->first;
  //If it's the first node
  if(next->breathe_type==type){
    player->breathables->first = next->next;
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
  if(player==NULL){
    quit("ERROR: NULL Player");}
  /* In this function, if there are no items to remove, we react as if we have
   * successfully removed the item
   */
  if(player->consumables==NULL || player->consumables->first==NULL){
    return 0;}
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
  if(player==NULL){
    quit("ERROR: NULL Player");}
  /* In this function, if there are no items to remove, we react as if we have
   * successfully removed the item
   */
  if(player->intrinsics==NULL || player->intrinsics->first==NULL){
    return 0;}
  if(type<0 || type>TRINSIC_MAX){return 1;}

  intrinsics_node_t* next = player->intrinsics->first;
  //If it's the first node
  if(next->intrinsic_type==type){
    player->intrinsics->first=next->next;
    free(next);
    return 0;
  }
  //Loop through to find it
  while(next->next!=NULL){
    if(next->next->intrinsic_type==type){
      intrinsics_node_t* temp = next->next;
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
  if(player==NULL){
    quit("ERROR: NULL Player");}
  /* In this function, if there are no items to remove, we react as if we have
   * successfully removed the item
   */
  if(player->resistances==NULL || player->resistances->first==NULL){
    return 0;}
  if(type<0 || type>DMG_MAX){return 1;}

  resistances_node_t* next = player->resistances->first;
  //If it's the first node
  if(next->damage_type==type){
    player->resistances->first=next->next;
    free(next);
    return 0;
  }
  //Loop through to find it
  while(next->next!=NULL){
    if(next->next->damage_type==type){
      resistances_node_t* temp = next->next;
      next->next = next->next->next;
      free(temp);
      return 0;
    }
  }
  //Not found
  return 1;
}

void player_init(char* name){
  player = (creature_t*)Calloc(1,sizeof(creature_t));
  set_strength(player, 1);
  set_perception(player, 1);
  set_endurance(player, 1);
  set_charisma(player, 1);
  set_intelligence(player, 1);
  set_luck(player, 1);
  set_health(player, 10);
  set_max_hunger(player, 200);
  set_hunger(player, player->max_hunger);
  set_gold(player, 5);
  set_level(player, 1);
  set_name(player, name);

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
  player->intrinsics = (intrinsics_list_t*)Calloc(1,sizeof(intrinsics_list_t));
  player->resistances = (resistances_list_t*)Calloc(1,sizeof(resistances_list_t));
}
