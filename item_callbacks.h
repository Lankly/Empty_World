#ifndef ITEM_CALLBACKS_H
#define ITEM_CALLBACKS_H

#include "items.h"
#include "helpers.h"

void defaultUseCallback(item_use_t* data);
void defaultConsumeCallback(item_consume_t* data);
void defaultZapCallback(item_zap_t* data);

void upStairUseCallback(item_use_t* data);
void downStairUseCallback(item_use_t* data);
void ironSwordUseCallback(item_use_t* data);

#define CALLBACK_TYPE_PLAYER    0
#define CALLBACK_TYPE_CREATURE  1
#define CALLBACK_TYPE_ITEM      2
#define CALLBACK_TYPE_PLAYER_CREATURE 3
#define CALLBACK_TYPE_MAX       3
#endif
