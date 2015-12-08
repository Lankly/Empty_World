#include <stdbool.h>

#ifndef HOTKEY_H
#define HOTKEY_H

/* Holds a command from a given recording and the next command */
typedef struct rec_t {
  int cmd;
  struct rec_t *next;
} rec_t;

/* Holds a recording and the next recording in our list of recordings */
typedef struct reclist_t {
  rec_t *recording;
  int bind;
  struct reclist_t *next;
} reclist_t;

/* We need to know where we left off in the command that's currently being run,
 * so this is the pointer to the next command that has not yet been pressed.
 * NULL indicates that there is no next command.
 */
rec_t *next_cmd;

reclist_t *hotkeys;

void start_recording();
void record_cmd(int cmd);
void stop_recording();
int get_next_cmd();

rec_t *get_hotkey_by_cmd(int cmd);
bool playback_hotkey(int cmd);

#endif
