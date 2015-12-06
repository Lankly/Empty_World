#include <stdbool.h>
#include "map.h"

#ifndef HELPERS_H
#define HELPERS_H

#define TERMINAL_WIDTH  80
#define TERMINAL_HEIGHT 24
#define DEFAULT_ITEMS_STACK_SIZE 10

#define PASS_WEIGHT 50

//Define the keys in the game
#define CMD_DEBUG 0
#define CMD_UP 1    //KEY_UP will always work for this
#define CMD_DOWN 2  //KEY_DOWN will always work for this
#define CMD_LEFT 3  //KEY_LEFT will always work for this
#define CMD_RIGHT 4 //KEY_RIGHT will always work for this
#define CMD_UP_RIGHT 5
#define CMD_UP_LEFT 6
#define CMD_DOWN_RIGHT 7
#define CMD_DOWN_LEFT 8
#define CMD_OPEN 9
#define CMD_CLOSE 10
#define CMD_QCKMV 11
#define CMD_PICKUP 12
#define CMD_INVENTORY 13
#define CMD_REMAP 14
#define CMD_EXTENDED 15
#define CMD_EXAMINE 16
#define CMD_ASCEND 17
#define CMD_DESCEND 18
#define CMD_MANUAL 19
#define CMD_WAIT 20
#define CMD_STATS 21
#define CMD_MAX 21

//Please keep this sorted alphabetically (after unknown)
#define EXT_UNKNOWN 0
#define EXT_8_COLORS 1
#define EXT_NUM_LOCK 2
#define EXT_TOGGLE_NUMPAD 3
#define EXT_QUIT 4
#define EXT_MAX 4

int cmd_data[CMD_MAX+1];
char* cmd_data_extended[EXT_MAX+1];

WINDOW *WIN;

bool qckmv;
bool use_8_colors;
bool use_num_lock;
bool use_numpad;
char *term;
int qckmv_cmd;
int num_turns;

void* Calloc(int items, int size);
char* str_lowercase(char* str);
bool str_is_num(char* str);
int get_coord(int x,int y,int width);
int get_distance(int x_0, int y_0, int x_1, int y_1);
void get_coord_via_cursor(int* y, int* x);
void quit(const char* error_msg);
void cmd_init();
void cmd_remap();

bool coord_in_range(int target_x, int target_y, struct creature_t *seer);
bool in_range(struct creature_t *target, struct creature_t *seer);
bool qckmv_continue(map_t*, int x, int y, int qckmv_cmd);

void draw_borders();
char display_list(char *instr, char **items, int num_items, int col_width);

void game_init();
void game_over();
void draw_map(struct map_t* map);

void open_tile();
void close_tile();
void pickup_tile();
void debug();
void manual();
void xscend();
void analyze_cmd_extended();
bool analyze_cmd(int cmd, int* x, int* y);

#endif
