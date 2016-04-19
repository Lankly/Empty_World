#include "macros.h"
#include "status.h"
#include "helpers.h"
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

/* Begins recording a new macro. If we're currently playing one back, 
 * or already recording another one, then this does nothing.
 */
void start_recording(){
  if(!playing_macro && !recording_macro){
    
    int new_macro = msg_promptchar("Press a key to bind to new macro");

    //ERR means that ESC was pressed
    if(new_macro == ERR || new_macro == cmd_data[CMD_MACRO]){
      recording_macro = false;
      msg_add("Canceled");
      return;
    }
    while(get_macro_by_cmd(new_macro) != NULL
	  || cmd_exists(new_macro)){
      new_macro = msg_promptchar("Key in use. Press a key to bind to new macro"); 
      if(new_macro == ERR){
	recording_macro = false;
	msg_add("Canceled");
	return;
      }
    }

    //Set up the macro
    reclist_t *new_reclist = Calloc(1, sizeof(reclist_t));
    new_reclist->bind = new_macro;
    new_reclist->next = macros;
    macros = new_reclist;
    recording_macro = true;
    msg_add("Recording has started.");
    next_cmd = NULL;
  }
  else if(playing_macro){
    msg_add("Cannot start recording while playback is happening.");
  }
  else{
    msg_add("Cannot start recording while recording.");
  }
}

/* Adds the given command to the list of commands for the macro we're currently
 * recording. If we're not recording, do nothing.
 */
void record_cmd(int cmd){
  //Make an exception for the macro button (Do not record it)
  if(recording_macro && cmd_data[CMD_MACRO] != cmd){
    rec_t *new_cmd = Calloc(1, sizeof(rec_t));
    new_cmd->cmd = cmd;
    //Check to make sure not first key of macro
    if(next_cmd != NULL){
      next_cmd->next = new_cmd;}
    else{
      macros->recording = new_cmd;
    }
    next_cmd = new_cmd;
  }
}

/* Just sets the recording_macro flag to false, since that's all that needs to
 * be done for the other functions to stop recording.
 */
void stop_recording(){
  recording_macro = false;
  if(macros == NULL || macros->recording == NULL){
    msg_addf("Nothing recorded.");
  }
  else{
    msg_addf("Finished recording. Bound to %c", macros->bind);
  }
}

int usleep();
/* Returns the next command in the current recording. If there is no recording
 * currently playing, or we have reached the end of the recording, then we send
 * the NULL-Terminator, like a string.
 */
int get_next_cmd(){
  int to_ret = 0;
  if(next_cmd == NULL){
    playing_macro = false;
  }
  else{
    to_ret = next_cmd->cmd;
    next_cmd = next_cmd->next;
    usleep(80000);
    //nanosleep((const struct timespec[]){{0, 100000000}}, NULL);
  }
  return to_ret;
}

rec_t *get_macro_by_cmd(int cmd){
  for(reclist_t *cur = macros; cur != NULL; cur = cur->next){
    if(cur->bind == cmd){
      return cur->recording;
    }
  }
  return NULL;
}

/* Attempts to start playing back a recording. If no recording with the given
 * binding exists, then we return false. If we successfully started playback,
 * return true. Also returns false if we're recording or playing back a macro.
 */
bool playback_macro(int cmd){
  if(!recording_macro && !playing_macro){
    next_cmd = get_macro_by_cmd(cmd);
    if(next_cmd != NULL){
      playing_macro = true;
    }
    return playing_macro;
  }
  return false;
}

/* This getch behaves like normal getch, except if we're running a macro, it
 * gets the next key in that macro instead of user input
 */
int Getch(){
  int to_ret = 0;
  if((recording_macro || (to_ret = get_next_cmd()) == 0)){
    while((to_ret = getch()) == ERR);
    record_cmd(to_ret);
  }
  return to_ret;
}
