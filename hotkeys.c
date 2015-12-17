#include "hotkeys.h"
#include "status.h"
#include "helpers.h"
#include "player.h"
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

/* Begins recording a new hotkey. If we're currently playing one back, 
 * or already recording another one, then this does nothing.
 */
void start_recording(){
  if(!playing_hotkey && !recording_hotkey){
    
    int new_hotkey = msg_promptchar("Press a key to bind to new hotkey");

    //ERR means that ESC was pressed
    if(new_hotkey == ERR || new_hotkey == cmd_data[CMD_HOTKEY]){
      recording_hotkey = false;
      msg_add("Canceled");
      return;
    }
    while(get_hotkey_by_cmd(new_hotkey) != NULL
	  || cmd_exists(new_hotkey)){
      new_hotkey = msg_promptchar("Key in use. Press a key to bind to new hotkey"); 
      if(new_hotkey == ERR){
	recording_hotkey = false;
	msg_add("Canceled");
	return;
      }
    }

    //Set up the hotkey
    reclist_t *new_reclist = Calloc(1, sizeof(reclist_t));
    new_reclist->bind = new_hotkey;
    new_reclist->next = hotkeys;
    hotkeys = new_reclist;
    recording_hotkey = true;
    msg_add("Recording has started.");
    next_cmd = NULL;
  }
  else if(playing_hotkey){
    msg_add("Cannot start recording while playback is happening.");
  }
  else{
    msg_add("Cannot start recording while recording.");
  }
}

/* Adds the given command to the list of commands for the hotkey we're currently
 * recording. If we're not recording, do nothing.
 */
void record_cmd(int cmd){
  //Make an exception for the hotkey button (Do not record it)
  if(recording_hotkey && cmd_data[CMD_HOTKEY] != cmd){
    rec_t *new_cmd = Calloc(1, sizeof(rec_t));
    new_cmd->cmd = cmd;
    //Check to make sure not first key of macro
    if(next_cmd != NULL){
      next_cmd->next = new_cmd;}
    else{
      hotkeys->recording = new_cmd;
    }
    next_cmd = new_cmd;
  }
}

/* Just sets the recording_hotkey flag to false, since that's all that needs to
 * be done for the other functions to stop recording.
 */
void stop_recording(){
  recording_hotkey = false;
  if(hotkeys == NULL || hotkeys->recording == NULL){
    msg_addf("Nothing recorded.");
  }
  else{
    msg_addf("Finished recording. Bound to %c",hotkeys->bind);}
}

int usleep();
/* Returns the next command in the current recording. If there is no recording
 * currently playing, or we have reached the end of the recording, then we send
 * the NULL-Terminator, like a string.
 */
int get_next_cmd(){
  int to_ret = 0;
  if(next_cmd == NULL){
    playing_hotkey = false;
  }
  else{
    to_ret = next_cmd->cmd;
    next_cmd = next_cmd->next;
    usleep(80000);
    //nanosleep((const struct timespec[]){{0, 100000000}}, NULL);
  }
  return to_ret;
}

rec_t *get_hotkey_by_cmd(int cmd){
  for(reclist_t *cur = hotkeys; cur != NULL; cur = cur->next){
    if(cur->bind == cmd){
      return cur->recording;
    }
  }
  return NULL;
}

/* Attempts to start playing back a recording. If no recording with the given
 * binding exists, then we return false. If we successfully started playback,
 * return true. Also returns false if we're recording or playing back a hotkey.
 */
bool playback_hotkey(int cmd){
  if(!recording_hotkey && !playing_hotkey){
    next_cmd = get_hotkey_by_cmd(cmd);
    if(next_cmd != NULL){
      playing_hotkey = true;
    }
    return playing_hotkey;
  }
  return false;
}

int Getch(){
  int to_ret = 0;
  if((recording_hotkey || (to_ret = get_next_cmd()) == 0)
     && ((to_ret = getch()) != ERR)){
    record_cmd(to_ret);
  }
  return to_ret;
}
