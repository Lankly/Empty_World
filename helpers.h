#ifndef HELPERS_H
#define HELPERS_H

#include <stdbool.h>
#include "map.h"
#include <ncurses.h>

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
#define CMD_MACRO 22
#define CMD_TARGET_ATTK 23
#define CMD_PREV_MSG 24
#define CMD_NEXT_MSG 25
#define CMD_MAX 25

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
MEVENT mouse;
int mouse_prev_x, mouse_prev_y;

bool qckmv;
bool use_8_colors;
bool use_num_lock;
bool use_numpad;
bool recording_macro;
bool playing_macro;
char *term;
int qckmv_cmd;
int num_turns;

typedef struct intlist_t{
  int elem;
  struct intlist_t *next;
}intlist_t;

void *Calloc(size_t items, size_t size);
char *str_lowercase(char *str);
bool str_is_num(char *str);
int *str_to_ints(char *str, int len);
char *ints_to_str(int *ints, int len);
int get_coord(int x,int y,int width);
int get_distance(int x_0, int y_0, int x_1, int y_1);
intlist_t *intlist_add(intlist_t *list, int elem);
void intlist_remove(intlist_t *list, int elem);
void intlist_free(intlist_t *list);
intlist_t *intlist_new(int elem);
void get_coord_via_cursor(int* y, int* x);
void quit(const char* error_msg);
void cmd_init();
void cmd_remap();
bool cmd_exists(int cmd);

bool coord_in_range(int target_x, int target_y, struct creature_t *seer);
bool in_range(struct creature_t *target, struct creature_t *seer);
bool qckmv_continue(map_t *map, int x, int y, int qckmv_cmd);

void draw_borders();
char display_list(char *instr, int **items, int num_items, int col_width);

void game_init();
void game_over();
void draw_map(struct map_t *map);

void open_tile(struct map_t *map, int x, int y, int direction);
void close_tile(struct map_t *map, int x, int y, int direction);
void debug();
void manual();
void xscend();
void analyze_cmd_extended();
bool analyze_cmd(int cmd, int* x, int* y);

void display_mouse();

#endif
