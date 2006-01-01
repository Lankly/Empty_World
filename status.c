#include "status.h"
#include <curses.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "classes.h"
#include "colors.h"
#include "helpers.h"
#include "macros.h"
#include "map.h"
#include "creature.h"
#include "player.h"

static bool newmsg;
static int num_msgs;
static msg_t *cur_msg, *first_msg, *last_msg;

void draw_status(map_t *map, struct creature_t *c){
  char* output = (char*)Calloc(81,sizeof(char));

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
  }  //now color the output while addch-ing
  move(MSG_ROW,0);
  for(int i=0; i<80; i++){
    addch(output[i] | COLOR_PAIR(CP_GREEN_BLACK));
  }

  //Line one of info
  sprintf(output,"  %s, %-*s STR:%-2d PER:%-2d END:%-2d CHA:%-2d INT:%-2d AGL:%-2d LCK:%-2d  ",
	  get_name(c),
	  (int)(80-2-strlen(get_name(c))-2-5-2-5-2-5-2-5-2-5-2-5-2-5-2-2),
	  class_data[c->class].name,
	  get_strength(c),
	  get_perception(c),
	  get_endurance(c),
	  get_charisma(c),
	  get_intelligence(c),
	  get_agility(c),
	  get_luck(c));
  move(22,0);
  for(int i=0; i<80; i++){
    addch(output[i] | COLOR_PAIR(CP_BLACK_WHITE));
    /*    int tempx; int tempy;
	  getyx(stdscr, &tempy, &tempx);*/
  }

  //Line two of info
  move(23,0);
  addch(' ' | COLOR_PAIR(CP_BLACK_WHITE));
  addch(' ' | COLOR_PAIR(CP_BLACK_WHITE));
  sprintf(output,"HP:%-3d", get_health(c));
  for(int i = 0; i < 6; i++){
    addch(output[i] | COLOR_PAIR(get_health(c) <(get_max_health(c)/2)?
				 get_health(c) <(get_max_health(c)/5)?
				 CP_BLACK_RED : CP_BLACK_YELLOW : CP_BLACK_WHITE
				 ));}
    
  sprintf(output,"  ENC:%-3d  Lvl:%-3d  Dlvl:%-2d Trn:%-6d%*s",
	  c->inventory != NULL ? c->inventory->cur_weight : 0,
	  c->level,
	  c->dlevel,
	  num_turns,
	  80-2-3-3-2-4-3-2-4-3-2-5-2-4-6,"");
  for(int i=0; i<80-2-3-3; i++){
    addch(output[i] | COLOR_PAIR(CP_BLACK_WHITE));
  }
  free(output);
}

void msg_add(char* new_msg){
  if(new_msg == NULL){new_msg = "Bad message";}
  
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
  newmsg = true;
  qckmv = false;
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

/* Uses the message bar to ask the user a question and return their response.
 * Will not leave the message there once the function completes.
 */
char* msg_prompt(char* prompt){
  if(prompt==NULL || strlen(prompt)>MAX_MSG_LEN-3){
    quit("Error: NULL prompt or prompt too large");
  }
  curs_set(1);

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
    
    //special case for backspace and delete
    if((ch==KEY_BACKSPACE || ch==127) && ret_pos>=0){
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

/* Like msg_prompt, but only allows the user to input one character before 
 * returning.
 */
int msg_promptchar(char* prompt){
  if(prompt==NULL || strlen(prompt)>MAX_MSG_LEN-3){
    quit("Error: NULL prompt or prompt too large");
  }
  curs_set(1);
  move(MSG_ROW,0);

  //Print out the prompt
  for(int i=0 ; i < MAX_MSG_LEN; i++){
    if(i < strlen(prompt)){
      addch(prompt[i] | COLOR_PAIR(CP_GREEN_BLACK));
    }
    else{
      addch(' ' | COLOR_PAIR(CP_GREEN_BLACK));
    }
  }
  curs_set(0);

  //Handle ESC
  ESCDELAY = 25;
  timeout(1);
  int to_ret = ERR;
  while(1){
    //Handle macro being played back
    if(playing_macro){
      to_ret = get_next_cmd();
    }
    else{
      to_ret = getch();}
    if(to_ret == 27 && getch() == ERR){
      return ERR;
    }
    else if(to_ret != ERR){
      break;
    }
  }
  timeout(-1);
  return to_ret;
}

void display_stats(struct creature_t *creature){
  int num_stats = 14;
  int possible_nonnumbered_stats = 5;
  int max_stat_length = 24;
  int **output = (int **)Calloc(num_stats, sizeof(int*));
  for(int i = 0; i < num_stats; i++){
    output[i] = (int *)Calloc(max_stat_length + 1, sizeof(int));
  }
  char *format = Calloc(max_stat_length, sizeof(char));
  //start formatting the stats
  sprintf(format,"Remaining HP - %d", get_health(creature));
  output[0] = str_to_ints(format, max_stat_length);
  sprintf(format,"Strength     - %d", get_strength(creature));
  output[1] = str_to_ints(format, max_stat_length);
  sprintf(format,"Perception   - %d", get_perception(creature));
  output[2] = str_to_ints(format, max_stat_length);
  sprintf(format,"Endurance    - %d", get_endurance(creature));
  output[3] = str_to_ints(format, max_stat_length);
  sprintf(format,"Charisma     - %d", get_charisma(creature));
  output[4] = str_to_ints(format, max_stat_length);
  sprintf(format,"Intelligence - %d", get_intelligence(creature));
  output[5] = str_to_ints(format, max_stat_length);
  sprintf(format,"Agility      - %d", get_agility(creature));
  output[6] = str_to_ints(format, max_stat_length);
  sprintf(format,"Level        - %d", get_level(creature));
  output[7] = str_to_ints(format, max_stat_length);
  sprintf(format,"Max HP       - %d", get_max_health(creature));
  output[8] = str_to_ints(format, max_stat_length);

  //Non-numbered stats
  int cur = num_stats - possible_nonnumbered_stats;
  if(creature->is_asleep){
    sprintf(format, "Is asleep");
    output[cur] = str_to_ints(format, max_stat_length);
    cur++;
  }
  if(creature->is_immobile){
    sprintf(format, "Is immobile");
    output[cur] = str_to_ints(format, max_stat_length);
    cur++;
  }
  if(creature->can_fly){
    sprintf(format,"Is flying");
    output[cur] = str_to_ints(format, max_stat_length);
    cur++;
  }
  if(creature->is_blind){
    sprintf(format, "Is blind");
    output[cur] = str_to_ints(format, max_stat_length);
    cur++;
  }
  if(creature->is_telepathic){
    sprintf(format, "Sees with their mind");
    output[cur] = str_to_ints(format, max_stat_length);
    cur++;
  }

  //Actually display them
  char *instr = Calloc(TERMINAL_WIDTH+1, sizeof(char));
  snprintf(instr, TERMINAL_WIDTH,
	   "Viewing stats for %s    ESC to quit",
	   creature->name);
  display_list(instr, output, num_stats, max_stat_length);

  free(instr);
  free(format);
  for(int i = 0; i < num_stats; i++){
    free(output[i]);
  }
  free(output);
}

void status_init(){
  char welcome_msg[81];
  sprintf(welcome_msg, "Welcome to the game, %s!", get_name(player));
  msg_add(welcome_msg);
}

char* get_cur_msg(){return cur_msg->msg;}
