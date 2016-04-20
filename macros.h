#ifndef MACRO_H
#define MACRO_H

#include <stdbool.h>

/* We need to know where we left off in the command that's currently being run,
 * so this is the pointer to the next command that has not yet been pressed.
 * NULL indicates that there is no next command.
 */

void start_recording();
void record_cmd(int cmd);
void stop_recording();
int get_next_cmd();

bool playback_macro(int cmd);

int Getch();

#endif
