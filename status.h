#ifndef STATUS_H
#define STATUS_H

#define MAX_MSGS 10
#define MAX_MSG_LEN 66
#define MAX_NAME_LEN 12
#define MSG_ROW 21

typedef struct msg_t {
  struct msg_t* prev;
  struct msg_t* next;
  char* msg;
} msg_t;

void draw_status();

void msg_add(char* new_msg);
void msg_addf(char* msg, ...);
char* msg_prompt();
char* get_cur_msg();

void status_init();

#endif
