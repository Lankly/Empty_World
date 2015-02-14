#include "status.h"
#include <curses.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "colors.h"
#include "helpers.h"

static bool newmsg;
static int dlevel, encumberment, health, level, num_msgs, weight;
static msg_t *cur_msg, *first_msg, *last_msg;

void draw_status(player_t* player,map_t* map){
  char* output = (char*) Calloc(81,sizeof(char));

  //Status bar
  if(newmsg){
    if(cur_msg->next != NULL){
      sprintf(output,"    %-*s[MORE]    ",MAX_MSG_LEN,cur_msg->msg);
    }
    else{
      sprintf(output,"    %-*s          ",MAX_MSG_LEN,cur_msg->msg);      
    }
  }
  else{
    sprintf(output,"%80s", "");
  }  //now color the output white printf-ing
  move(MSG_ROW,0);
  for(int i=0; i<80; i++){
    addch(output[i] | COLOR_PAIR(CP_GREEN_BLACK));
  }

  //Line one of info
  sprintf(output,"  %-*s      STR:%-2d  PER:%-2d  END:%-2d  CHA:%-2d  INT:%-2d  AGL:%-2d  LCK:%-2d      ",
	  MAX_NAME_LEN,
	  player->name,
	  player->strength,
	  player->perception, 
	  player->endurance, 
	  player->charisma, 
	  player->intelligence, 
	  player->agility, 
	  player->luck);
  move(22,0);
  for(int i=0; i<80; i++){
    addch(output[i] | COLOR_PAIR(CP_BLACK_WHITE));
  }

  //Line two of info
  sprintf(output,"  HP:%-3d  ENC:%-3d  Lvl:%-3d  Dlvl:%-2d%*s",
	  player->health,
	  player->inventory!=NULL?player->inventory->encumberment:0,
	  player->level,
	  map->dlevel,
	  80-2-3-3-2-4-3-2-4-3-2-5-2,"");
  move(23,0);
  for(int i=0; i<80; i++){
    addch(output[i] | COLOR_PAIR(CP_BLACK_WHITE));
  }
  free(output);
  output=0;
}

void msg_add(char* new_msg){
  char* m = (char*) Calloc(MAX_MSG_LEN+1,sizeof(char*));
  strncpy(m,new_msg,MAX_MSG_LEN);
  //If no messages exist
  if(num_msgs==0){
    cur_msg = (msg_t*) Calloc(1,sizeof(msg_t));
    cur_msg->msg = m;
  }//If we have not reached max messages
  else if (num_msgs<MAX_MSGS){
    last_msg->next = (msg_t*) Calloc(1,sizeof(msg_t));
    last_msg->next->prev=last_msg;
    last_msg=last_msg->next;
    if(!newmsg){
      cur_msg=last_msg;
    }
  }//If there are too many messages
  else{
    //delete the first
    first_msg=first_msg->next;
    free(first_msg->prev->msg);
    free(first_msg->prev);
    first_msg->prev=0;
    //call this method again
    num_msgs--;
    msg_add(m);
  }
  newmsg=true;
}

/*Same as msg_add, but with more arguments*/
void msg_addf(char* msg, ...){
  va_list args;
  va_start(args,msg);
  char* new_msg = (char*)Calloc(MAX_MSG_LEN+1,sizeof(char*));
  vsprintf(new_msg,msg,args);
  va_end(args);
  msg_add(new_msg);
  free(new_msg);
}

char* msg_prompt(char* prompt){
  if(prompt==NULL || strlen(prompt)>MAX_MSG_LEN-3){
    quit("Error: NULL prompt or prompt too large");
  }
  curs_set(1);
  move(MSG_ROW,0);

  int ch = (int)' ';
  int ret_pos=0;
  char* ret = (char*)Calloc(81-strlen(prompt),sizeof(char));
  while(ch!='\n'){
    //clear message bar
    move(MSG_ROW,0);
    for(int i=0;i<MAX_MSG_LEN;i++){addch(' ');}
    move(MSG_ROW,0);
    //print out the prompt first
    for(int i=0; i<strlen(prompt); i++){
      addch(prompt[i] | COLOR_PAIR(CP_GREEN_BLACK));
    }
    //then print out everything the user has input
    for(int i=0; i<strlen(ret); i++){
      addch(ret[i] | COLOR_PAIR(CP_GREEN_BLACK));
    }
    //read in the next key
    ch=getch();
    //special case for backspace
    if(ch==KEY_BACKSPACE && ret_pos>=0){
	ret_pos--;
	ret[ret_pos]=0;
      }
    else if((strlen(prompt)+strlen(ret))<=MAX_MSG_LEN){
      //do not add \n
      if(ch!='\n' && (isalnum(ch) || isspace(ch))){
	ret[ret_pos]=ch;
	ret_pos++;
      }
    }
    else{msg_add("Error");}
  }
  curs_set(0);
  return ret;
}

void set_health(int h){health=h;}
void set_encumberment(int e){encumberment=e;}
void set_level(int l){level=l;}
void set_dlevel(int d){dlevel=d;}
void set_name(char* n){name=n;}
void set_weight(int w){weight=w;}

void set_strength(int s){strength=s;}
void set_perception(int p){perception=p;}
void set_endurance(int e){endurance=e;}
void set_charisma(int c){charisma=c;}
void set_intelligence(int i){intelligence=i;}
void set_agility(int a){agility=a;}
void set_luck(int l){luck=l;}

char* get_name(){return name;}
char* get_cur_msg(){return cur_msg->msg;}
int get_health(){return health;}
int get_encumberment(){return encumberment;}
int get_level(){return level;}
int get_dlevel(){return dlevel;}
int get_weight(){return weight;}

int get_strength(){return strength;}
int get_perception(){return perception;}
int get_endurance(){return endurance;}
int get_charisma(){return charisma;}
int get_intelligence(){return intelligence;}
int get_agility(){return agility;}
int get_luck(){return luck;}

void add_health(int h){health+=h;}
void add_weight(int w){weight+=w;}

void status_init(){
  set_health(100);
  set_encumberment(0);
  set_level(1);
  set_dlevel(0);
  set_name("Lan");
  char welcome_msg[81];
  sprintf(welcome_msg, "Welcome to the game, %s!", get_name());
  msg_add(welcome_msg);
}
