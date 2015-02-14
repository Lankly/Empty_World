#include "player.h"
#include "status.h"
#include "items.h"

#ifndef ITEM_CALLBACKS_H
#define ITEM_CALLBACKS_H

typedef struct creature_list_node_t{
  creature_t* creature;
  creature_list_node_t* next;
}creature_list_node_t;
typedef struct{
  creature_list_node_t* first;
}creature_list_t;

typedef struct{
  int type;
  item_t* caller;
  item_t* item;
  creatue_t* creature;
}item_use_t;

typedef struct{
  int type;
  creature_t* creature;
}item_consume_t;

typedef struct{
  int type;
  creature_list_t* creatures;
}item_zap_t;

typedef void (*itemUseCallback)(item_use_t* data);
typedef void (*itemConsumeCallback)(item_consume_t* data);
typedef void (*itemZapCallback)(item_zap_t* data);

void defaultUseCallback(item_use_t* data);
void defaultConsumeCallback(item_consume_t* data);
void defaultZapCallback(item_zap_t* data);
void ironSwordUseCallback(item_use_t* data);

#define CALLBACK_TYPE_PLAYER    0
#define CALLBACK_TYPE_CREATURE  1
#define CALLBACK_TYPE_ITEM      2
#define CALLBACK_TYPE_PLAYER_CREATURE 3
#define CALLBACK_TYPE_MAX       3
#endif
