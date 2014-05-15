#include "status.h"
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colors.h"
#include "helpers.h"

static bool newmsg;
static char* name;
static int dlevel, encumberment, health, level, num_msgs;
static msg_t *cur_msg, *first_msg, *last_msg;

void draw_status(){
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
  move(21,0);
  for(int i=0; i<80; i++){
    addch(output[i] | COLOR_PAIR(CP_GREEN_BLACK));
  }

  //Line one of info
  sprintf(output,"  %-*s%*s",MAX_NAME_LEN,name,80-MAX_NAME_LEN-2,"");
  move(22,0);
  for(int i=0; i<80; i++){
    addch(output[i] | COLOR_PAIR(CP_BLACK_WHITE));
  }

  //Line two of info
  sprintf(output,"  HP:%-3d  ENC:%-3d  Lvl:%-3d  Dlvl:%-2d%*s",health,encumberment,level,dlevel,80-2-3-3-2-4-3-2-4-3-2-5-2,"");
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

void set_health(int h){health=h;}
void set_encumberment(int e){encumberment=e;}
void set_level(int l){level=l;}
void set_dlevel(int d){dlevel=d;}
void set_name(char* n){name=n;}

char* get_name(){return name;}
char* get_cur_msg(){return cur_msg->msg;}
int get_health(){return health;}
int get_encumberment(){return encumberment;}
int get_level(){return level;}
int get_dlevel(){return dlevel;}
  
