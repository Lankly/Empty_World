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

void set_health(int h);
void set_encumberment(int e);
void set_level(int l);
void set_dlevel(int d);
void set_name(char* n);
void set_weight(int w);

void set_strength(int s);
void set_perception(int p);
void set_endurance(int e);
void set_charisma(int c);
void set_intelligence(int i);
void set_agility(int a);
void set_luck(int l);

char* get_name();
char* get_cur_msg();
int get_health();
int get_encumberment();
int get_level();
int get_dlevel();
int get_weight();

int get_strength();
int get_perception();
int get_endurance();
int get_charisma();
int get_intelligence();
int get_agility();
int get_luck();

void add_health(int h);
void add_weight(int w);

void status_init();

#endif
