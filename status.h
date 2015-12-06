#include "creature.h"
#ifndef STATUS_H
#define STATUS_H

#define MAX_MSGS 10
#define MAX_MSG_LEN 66
#define MAX_NAME_LEN 32 > (TERMINAL_WIDTH-4) ? TERMINAL_WIDTH-4 : 32
#define MSG_ROW 21

typedef struct msg_t {
  struct msg_t* prev;
  struct msg_t* next;
  char* msg;
} msg_t;

void draw_status();

void msg_add(char* new_msg);
void msg_addf(char* msg, ...);
char* msg_prompt(char* prompt);
int msg_promptchar(char* prompt);
char* get_cur_msg();

void display_stats(struct creature_t *creature);

void status_init();

#endif
