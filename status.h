#ifndef STATUS_H
#define STATUS_H

#define MAX_MSGS 10
#define MAX_MSG_LEN 66
#define MAX_NAME_LEN 12

typedef struct msg_t {
  struct msg_t* prev;
  struct msg_t* next;
  char* msg;
} msg_t;

void draw_status();

void msg_add(char* m);
void set_health(int h);
void set_encumberment(int e);
void set_level(int l);
void set_dlevel(int d);
void set_name(char* n);

char* get_name();
char* get_cur_msg();
int get_health();
int get_encumberment();
int get_level();
int get_dlevel();

#endif
