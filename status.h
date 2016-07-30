#include "creature.h"
#ifndef STATUS_H
#define STATUS_H

#define MAX_MSGS 15
#define MAX_MSG_LEN 76
#define MAX_NAME_LEN 32 > (TERMINAL_WIDTH-4) ? TERMINAL_WIDTH-4 : 32
#define MSG_ROW 21

/* Draws the message bar and two stat bars on screen. Requires map and c to be
 * non-NULL.
 */
void draw_status(map_t *map, struct creature_t *c);

/* Displays the previous message in message history on-screen */
void prev_message();
/* Displays the next message in message history on-screen */
void next_message();

/* Adds a new message to the list of messages and displays it on-screen*/
void msg_add(char* new_msg);
void msg_addf(char* msg, ...);

/* Asks the user a question, and returns the string response. Not added to
 * message history.
 */
char* msg_prompt(char* prompt);
/* Like msg_prompt, but only gets one character from the user */
int msg_promptchar(char* prompt);

/* Displays to the player a page of all their current stats. */
void display_stats(struct creature_t *creature);

/* Init method
 */
void status_init();

#endif
