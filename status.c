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

/**********************
 * THE MESSAGE STRUCT *
 **********************/
typedef struct msg_t {
  struct msg_t* prev;
  struct msg_t* next;
  char* msg;
} msg_t;

/*********************
 * PRIVATE VARIABLES *
 *********************/
bool newmsg, was_prev_msg;
int num_msgs, prev_msg;
msg_t *first_msg, *last_msg;


/********************
 * PUBLIC FUNCTIONS *
 ********************/

void draw_status(map_t *map, creature_t *c){
  if(map == NULL || c == NULL){
    quit("Can't draw status bar!");
  }
  
  char* output = Calloc(TERMINAL_WIDTH + 1, sizeof(char));

  /* If the user displayed a previous message, we will want it to go away when
   * they perform another action, so we let a tiny bit of buffer time happen
   * before we stop displaying the previous message.
   */
  if(!was_prev_msg){
    prev_msg = 0;
  }
  else{
    was_prev_msg = false;
  }
  
  //Status bar
  if(newmsg){
    //If too many messages, "..." will be displayed, so +3
    sprintf(output,"%-*s", MAX_MSG_LEN + 3, last_msg->msg);
    newmsg = false;
    qckmv = false;
    prev_msg = 0;
  }
  else{
    if(!prev_msg){
      sprintf(output,"%-*s", TERMINAL_WIDTH, " ");
    }
  }

  //Get coordinates for centering
  int y0, x0;
  get_centered_box_ul_coord(&y0, &x0, TERMINAL_HEIGHT, TERMINAL_WIDTH);
  
  attron(COLOR_PAIR(CP_GREEN_BLACK));
  mvaddstr(y0 + MSG_ROW, x0 + 0, output);
  attroff(COLOR_PAIR(CP_GREEN_BLACK));

  free(output);
  output = Calloc(TERMINAL_WIDTH + 1, sizeof(char));
  
  //Line one of info (always at least player_name, class_name)
  char *name_and_class = Calloc(TERMINAL_WIDTH, sizeof(char));
  sprintf(name_and_class, "%s, %s",
	  creature_get_name(c), class_name(creature_get_class(c)));
  strncat(output, name_and_class, (int)(TERMINAL_WIDTH - strlen(output)));
  
  ////Contains information about what is in the left and right hand
  char *left_hand = Calloc(TERMINAL_WIDTH, sizeof(char));
  body_part_t *lh = get_body_part_by_name(creature_get_body(c), "Left hand");
  strncat(left_hand, "  LeftHand:", TERMINAL_WIDTH);
  item_t *lh_held = body_part_get_held_item(lh);
  strncat(left_hand, lh == NULL ? "Gone" : lh_held == NULL ? "None"
	  : item_name(lh_held), TERMINAL_WIDTH - strlen(left_hand));

  if((strlen(output) + strlen(left_hand)) < TERMINAL_WIDTH){
    strcat(output, left_hand);
  }

  char *right_hand = Calloc(TERMINAL_WIDTH, sizeof(char));
  body_part_t *rh = get_body_part_by_name(creature_get_body(c), "Right hand");
  item_t *rh_held = body_part_get_held_item(rh);
  strncat(right_hand, " RightHand:", TERMINAL_WIDTH);
  strncat(right_hand, rh == NULL ? "Gone" : rh_held == NULL ? "None"
	  : item_name(rh_held), TERMINAL_WIDTH - strlen(right_hand));

  if((strlen(output) + strlen(right_hand)) < TERMINAL_WIDTH){
    strcat(output, right_hand);
  }

  ////Pad the string
  char *padding = Calloc(TERMINAL_WIDTH+1, sizeof(char));
  sprintf(padding, "%-*s", (int)(TERMINAL_WIDTH)," ");
  strncat(output, padding, ((int)(TERMINAL_WIDTH - strlen(output))));

  ////print out the string
  move(y0 + MSG_ROW + 1, x0 + 0);
  attron(A_BOLD);
  for(int i = 0; i < TERMINAL_WIDTH; i++){
    addch(output[i] | COLOR_PAIR(CP_BLACK_WHITE));
    if(i >= strlen(name_and_class)){
      attroff(A_BOLD);
    }
  }

  //Line two of info
  free(output);
  output = Calloc(TERMINAL_WIDTH+1, sizeof(char));
  
  char *help = Calloc(7, sizeof(char));
  sprintf(help, "Help:%c", (char)cmd_data[CMD_MANUAL]);
  if(TERMINAL_WIDTH - strlen(output) - strlen(help) > 0){
    strcat(output, help);}
  free(help);
  
  ////Pad the string
  strncat(output, padding, ((int)(TERMINAL_WIDTH - strlen(output))));

  ////print out the string
  attron(COLOR_PAIR(CP_BLACK_WHITE));
  mvaddstr(y0 + MSG_ROW + 2, x0 + 0, output);
  attroff(COLOR_PAIR(CP_BLACK_WHITE));

  free(name_and_class);
  free(padding);
  free(left_hand);
  free(right_hand);
  free(output);
}


void message_nav_helper(int dir);
/* Moves back one message, for displaying on-screen.
 */
void prev_message(){
  message_nav_helper(-1);
}



/* Moves forward one message, for displaying on-screen.
 */
void next_message(){
  message_nav_helper(1);
}



void msg_add(char *new_msg){
  if(new_msg == NULL){new_msg = "Bad message";}

  //If no messages exist
  if(num_msgs == 0){
    first_msg = (msg_t *)Calloc(1, sizeof(msg_t));
    first_msg->msg = (char *)Calloc(MAX_MSG_LEN + 4, sizeof(char*));
    strncpy(first_msg->msg, new_msg, MAX_MSG_LEN);
    last_msg = first_msg;
    num_msgs++;
  }
  //If we have not reached max messages
  else if (num_msgs < MAX_MSGS){
    //Try to append message to existing unread messages
    if(newmsg && last_msg->msg != NULL
       && (strlen(last_msg->msg) + strlen(new_msg) < (MAX_MSG_LEN-1))){
      
      strcat(last_msg->msg, " ");
      strcat(last_msg->msg, new_msg);
    }
    //If too many unread messages to fit in message bar
    else if(newmsg){
      //Add "..." to existing unread message
      strcat(last_msg->msg, "...");
      //Display the message (sets newmsg to false)
      draw_map(cur_map);
      draw_status(cur_map, player);
      //Wait for a key
      Getch();
      //Call this function again
      msg_add(new_msg);
    }
    //Otherwise, just add a new message
    else{
      last_msg->next = (msg_t *)Calloc(1, sizeof(msg_t));
      last_msg->next->prev = last_msg;
      last_msg = last_msg->next;
      last_msg->msg = (char *)Calloc(MAX_MSG_LEN + 4, sizeof(char*));
      strncpy(last_msg->msg, new_msg, MAX_MSG_LEN);
      num_msgs++;
    }
  }
  //If there are too many messages
  else{
    //delete the first
    first_msg = first_msg->next;
    free(first_msg->prev->msg);
    free(first_msg->prev);
    first_msg->prev = NULL;
    num_msgs--;

    //call this function again
    msg_add(new_msg);
  }
  newmsg = true;
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

  //Get coordinates for centering
  int y0, x0;
  get_centered_box_ul_coord(&y0, &x0, TERMINAL_HEIGHT, TERMINAL_WIDTH);
  
  int ch = (int)' ';
  int ret_pos=0;
  char* ret = (char*)Calloc(81-strlen(prompt),sizeof(char));
  while(ch!='\n'){
    //clear message bar
    move(y0 + MSG_ROW, x0 + 0);
    for(int i=0;i<MAX_MSG_LEN;i++){addch(' ');}
    move(y0 + MSG_ROW, x0 + 0);
    
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

  //Get coordinates for centering
  int y0, x0;
  get_centered_box_ul_coord(&y0, &x0, TERMINAL_HEIGHT, TERMINAL_WIDTH);
  move(y0 + MSG_ROW, x0 + 0);

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
    to_ret = Getch();
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
  if(creature_is_asleep(creature)){
    sprintf(format, "Is asleep");
    output[cur] = str_to_ints(format, max_stat_length);
    cur++;
  }
  if(creature_is_immobile(creature)){
    sprintf(format, "Is immobile");
    output[cur] = str_to_ints(format, max_stat_length);
    cur++;
  }
  if(creature_is_flying(creature)){
    sprintf(format,"Is flying");
    output[cur] = str_to_ints(format, max_stat_length);
    cur++;
  }
  if(creature_is_blind(creature)){
    sprintf(format, "Is blind");
    output[cur] = str_to_ints(format, max_stat_length);
    cur++;
  }
  if(creature_is_telepathic(creature)){
    sprintf(format, "Sees with their mind");
    output[cur] = str_to_ints(format, max_stat_length);
    cur++;
  }

  //Actually display them
  char *instr = Calloc(TERMINAL_WIDTH+1, sizeof(char));
  snprintf(instr, TERMINAL_WIDTH,
	   "Viewing stats for %s    ESC to quit",
	   creature_get_name(creature));
  display_list(instr, output, num_stats, max_stat_length);

  free(instr);
  free(format);
  for(int i = 0; i < num_stats; i++){
    free(output[i]);
  }
  free(output);
}



void status_init(){
  char welcome_msg[MAX_MSG_LEN];
  sprintf(welcome_msg, "Welcome to the game, %s!", creature_get_name(player));
  msg_add(welcome_msg);
}



/*********************
 * PRIVATE FUNCTIONS *
 *********************/

char* get_cur_msg(){
  return last_msg == NULL ? "" : last_msg->msg;
}



void message_nav_helper(int dir){
  prev_msg += dir;

  if(prev_msg > num_msgs){
    prev_msg = 0;
  }
  else if(prev_msg < 0){
    prev_msg = num_msgs;
  }

  /* When we're at 0, nothing should be printed, so that the player
   * knows that we're about to loop.
   */
  if(prev_msg == 0){
    return;
  }
  
  msg_t *cur = first_msg;
  for(int i = 1; i < prev_msg && cur != NULL; i++){
    cur = cur->next;
  }

  //Get coordinates for centering
  int y0, x0;
  get_centered_box_ul_coord(&y0, &x0, TERMINAL_HEIGHT, TERMINAL_WIDTH);
  
  attron(COLOR_PAIR(CP_GREEN_BLACK));
  mvaddstr(y0 + MSG_ROW, x0 + 0, cur->msg);
  for(int i = strlen(cur->msg); i < TERMINAL_WIDTH; i++){
    addch(' ');
  }
  attroff(COLOR_PAIR(CP_GREEN_BLACK));

  was_prev_msg = true;
}
