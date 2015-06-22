#include "helpers.h"
#include "classes.h"
#include "colors.h"
#include "creature.h"
#include "inventory.h"
#include "items.h"
#include "map.h"
#include "player.h"
#include "status.h"
#include "tiles.h"
#include <ctype.h>
#include <curses.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

//Helper Methods

void quit(const char* error_msg){
  endwin();
  printf("%s\n",error_msg);
  exit(1);
}

int get_coord(int x,int y,int width){
  return y*width+x;
}

/* This function gives the player a cursor to move around on-scren with. When
 * they hit enter, the pointers we were given will be updated with that location
 */
void get_coord_via_cursor(int* y, int* x){
  int temp_x = x != NULL? *x : 0;
  int temp_y = y != NULL? *y : 0;
  
  int ch = ' ';
  curs_set(2);
  while(ch != '\n'){
    move(temp_y, temp_x);
    ch = getch();
    if(ch == cmd_data[CMD_UP] 
       || ch == cmd_data[CMD_UP_RIGHT] 
       || ch == cmd_data[CMD_UP_LEFT]){
      temp_y -= temp_y > 0 ? 1 :0;
    }
    else if(ch == cmd_data[CMD_DOWN]
	    || ch == cmd_data[CMD_DOWN_RIGHT]
	    || ch == cmd_data[CMD_DOWN_LEFT]){
      //The -4 is -1 for movement, -3 for the status bar
      temp_y += temp_y < TERMINAL_HEIGHT-4 ? 1 : 0;
    }
    if(ch == cmd_data[CMD_LEFT]
       || ch == cmd_data[CMD_UP_LEFT]
       || ch == cmd_data[CMD_DOWN_LEFT]){
      temp_x -= temp_x > 0 ? 1 : 0;
    }
    else if(ch == cmd_data[CMD_RIGHT]
	    || ch == cmd_data[CMD_UP_RIGHT]
	    || ch == cmd_data[CMD_DOWN_RIGHT]){
      temp_x += temp_x < TERMINAL_WIDTH-1 ? 1 : 0;
    }
  }
  curs_set(0);
  //Enter was pressed, so update the pointers
  *x = temp_x;
  *y = temp_y;
}

char* str_lowercase(char* str){
  char* ret = Calloc(strlen(str)+1,sizeof(char));
  for(int i=0;i<strlen(str);i++){
    ret[i]=tolower(str[i]);
  }
  return ret;
}

bool str_is_num(char* str){
  for(int i=0;i<strlen(str);i++){
    if(!isdigit(str[i])){
      return false;
    }
  }
  return true;
}

void* Calloc(int items, int size)
{
  void* ret = calloc(items, size);
  if (ret == 0)
  {
    quit("Error: out of memory\n");
  }
  return ret;
}

/* This command will give all the initial definitions to the keys for the game.
 */
void cmd_init(){
  cmd_data[CMD_DEBUG] = '~';
  cmd_data[CMD_UP] = KEY_UP;
  cmd_data[CMD_DOWN] = KEY_DOWN;
  cmd_data[CMD_LEFT] = KEY_LEFT;
  cmd_data[CMD_RIGHT] = KEY_RIGHT;
  cmd_data[CMD_UP_RIGHT] = KEY_PPAGE; //Upper Right of keypad
  cmd_data[CMD_UP_LEFT] = KEY_HOME; //Upper Left of keypad
  cmd_data[CMD_DOWN_RIGHT] = KEY_NPAGE; //Lower Right of keypad
  cmd_data[CMD_DOWN_LEFT] = KEY_END; //Lower Left of keypad
  cmd_data[CMD_OPEN] = 'o';
  cmd_data[CMD_CLOSE] = 'C';
  cmd_data[CMD_QCKMV] = KEY_B2; //5 on keypad
  cmd_data[CMD_PICKUP] = ',';
  cmd_data[CMD_INVENTORY] = 'i';
  cmd_data[CMD_REMAP] = '=';
  cmd_data[CMD_EXTENDED] = '#';
  cmd_data[CMD_EXAMINE] = 'x';
  cmd_data[CMD_ASCEND] = '<';
  cmd_data[CMD_DESCEND] = '>';
  cmd_data[CMD_MANUAL] = '?';

  cmd_data_extended[EXT_UNKNOWN] = "";
  cmd_data_extended[EXT_NUM_LOCK] = "num-lock";
  cmd_data_extended[EXT_TOGGLE_NUMPAD] = "toggle-numpad";
  cmd_data_extended[EXT_QUIT] = "quit";
  cmd_data_extended[EXT_8_COLORS] = "8-colors";
}

/* This function will remap a given CMD to a new key, assuming that the input
 * is actually valid.
 */
void cmd_remap(){
  int cmd = msg_promptchar("What key do you want to remap?");
  int new = msg_promptchar("What key should it be now?");
  int CMD = -1;

  //CMD should now be made to be the index of the command given.
  for(int i=0; i <= CMD_MAX; i++){
    if(cmd_data[i] == cmd){
      CMD = i;
    }
    else if(cmd_data[i] == new){
      msg_promptchar("That key is already in use!");
      return;
    }
  }
  //If it was never found, quit
  if(CMD == -1){
    msg_promptchar("The key you want to remap does not exist.");
    return;
  }

  //Make sure the new command does not collide with any old ones
  for(int i=CMD; i <= CMD_MAX; i++){
    if(cmd_data[i] == new){
      msg_promptchar("That key is already in use!");
      return;
    }
  }

  cmd_data[CMD] = new;
}

void analyze_cmd(int cmd, int*x, int* y);
/* This function handles what to do when the open action is executed. It will
 * attempt to turn a closed door into an open door at this time. 
 * TODO: Include the ability to open items on the floor like chests.
 */
void open_tile(int x, int y, int direction){
  int open_x = x, open_y = y;
  analyze_cmd(direction, &open_x, &open_y);
  
  int otile=map_get_tile(cur_map,open_x,open_y);
  if(tile_data[otile].openable){
    if(otile==TILE_DOOR_CLOSE){
      map_set_tile(cur_map,open_x,open_y,TILE_DOOR_OPEN);
    }else if(otile==TILE_DOOR_OPEN){
      msg_add("That door is already open.");
    }else if(otile==TILE_DOOR_BROKEN){
      msg_add("That door is broken.");
    }
  }else if(open_x==player->x && open_y==player->y){
    msg_add("Invalid direction.");
  }else{msg_add("That cannot be opened.");}
}

/* This function handles what to do when the close action is executed. It will
 * attempt to turn an open door into a closed door at this time.
 */
void close_tile(int x, int y, int direction){
  int close_x = x, close_y = y;
  analyze_cmd(direction, &close_x, &close_y);
  
  int ctile = map_get_tile(cur_map, close_x, close_y);
  if(tile_data[ctile].openable){
    if(ctile == TILE_DOOR_OPEN){
      map_set_tile(cur_map, close_x, close_y, TILE_DOOR_CLOSE);
    }else if(ctile == TILE_DOOR_CLOSE){
      msg_add("That door is already closed.");
    }else if(ctile == TILE_DOOR_BROKEN){
	msg_add("That door is broken");
    }
  }else{msg_add("That cannot be closed.");}
}

/* This function handles picking an item up off of the floor.
 */
void pickup_tile(struct creature_t* creature, struct map_t* map){
  if(creature == NULL){quit("Error: Cannot give item to NULL Creature");}
  if(map == NULL){quit("Error: Cannot get item of NULL Map");}

  int count = count_items(map, creature->x, creature->y);
  item_t* to_add;
  /* This is the case where the player has tried to pick up an item,
   * but there is nothing on the ground  beneath them.
   */
  if(count == 0){
    if(creature == player){
      msg_add("No items to pick up!");}
    return;
  }
  
  /* This is the case where the there is only one item on the ground
   * beneath the player.
   */
  else if(count == 1){
    to_add = remove_item(map, creature->x, creature->y, 0);
  }

  /* This is the case where there are multiple items on the ground 
   * beneath the player. It will take the user to a screen where they
   * can select which item it is that they're trying to pick up.
   */
  else if(creature == player){
    to_add = remove_item(map,
			 creature->x,
			 creature->y,
			 items_display(map, creature->x, creature->y));
  }

  /* TODO: Nonplayer creature picking up from many items.
   */
  else{
    return;
  }
  inventory_add(creature, to_add);
}

void debug(){
  char* debug_cmd = msg_prompt("~ ");
  char* debug_cmd_lower = str_lowercase(debug_cmd);
  if(!strcmp(debug_cmd_lower,"place")){
    free(debug_cmd_lower);
    free(debug_cmd);
    
    char* debug_cmd = msg_prompt("Place where? ");
    char* debug_cmd_lower = str_lowercase(debug_cmd);
    int place_x = player->x; 
    int place_y = player->y;
    
    if(!strcmp(debug_cmd_lower, "nw")){
      place_x--;
      place_y--;
    }else if(!strcmp(debug_cmd_lower, "n")){
      place_y--;
    }else if(!strcmp(debug_cmd_lower,"ne")){
      place_x++;
      place_y--;
    }else if(!strcmp(debug_cmd_lower, "w")){
      place_x--;
    }else if(!strcmp(debug_cmd_lower, "e")){
      place_x++;
    }else if(!strcmp(debug_cmd_lower, "sw")){
      place_x--;
      place_y++;
    }else if(!strcmp(debug_cmd_lower, "s")){
      place_y++;
    }else if(!strcmp(debug_cmd_lower, "se")){
      place_x++;
      place_y++;
    }
    
    free(debug_cmd_lower);
    free(debug_cmd);
    debug_cmd=msg_prompt("Place what? ");
    debug_cmd_lower=str_lowercase(debug_cmd);

    if(!strcmp(debug_cmd_lower,"tile")){
      free(debug_cmd_lower);
      free(debug_cmd);
      debug_cmd=msg_prompt("TILE ID: ");
      debug_cmd_lower=str_lowercase(debug_cmd);

      if(str_is_num(debug_cmd_lower)){
	map_set_tile(cur_map,place_x,place_y,atoi(debug_cmd)); 
	msg_add("Done");
      }
    }else if(!strcmp(debug_cmd_lower,"item")){
      free(debug_cmd_lower);
      free(debug_cmd);
      debug_cmd=msg_prompt("ITEM ID: ");
      debug_cmd_lower=str_lowercase(debug_cmd);

      if(str_is_num(debug_cmd_lower)){
	add_item(cur_map,
		 place_x,
		 place_y,
		 (struct item_t*)item_create_from_data(atoi(debug_cmd)),
		 false);
	//msg_add("Done");
      }
    }
    free(debug_cmd_lower);
    free(debug_cmd);
  }else if(!strcmp(debug_cmd_lower,"print items")){
    endwin();
    struct item_map_t* cur_items = cur_map->items;
    for(int j=0; j<cur_map->height; j++){
      for(int i=0; i<cur_map->width; i++){
	if(cur_items!=NULL 
	   && cur_items->size!=0 
	   && cur_items->x==i 
	   && cur_items->y==j){
	  printf("%c (%d,%d) [%d,%d]\n",
		 get_top_item_sym_from_stack(cur_items),
		 cur_items->x, cur_items->y, i, j);
	  cur_items=cur_items->next;
	}
      }
    }
    exit(0);
  }else if(!strcmp(debug_cmd_lower,"reveal")){
    cur_map->known_map = cur_map;
  }else if(!strcmp(debug_cmd_lower,"kill")){
    int k_x = player->x;
    int k_y = player->y;
    get_coord_via_cursor(&k_y,&k_x);

    if(cur_map->creatures != NULL){
      for(struct creature_list_node_t *cur = cur_map->creatures->first;
	  cur != NULL;
	  cur = cur->next){
	if(cur->creature->x == k_x && cur->creature->y == k_y){
	  damage_creature(cur->creature, "GOD", 10000);
	  break;
	}
      }
    }
  }else if(!strcmp(debug_cmd_lower,"toomuchhealth")){
    set_health(player, 10000);
  }else if(!strcmp(debug_cmd_lower,"maketelepathic")){
    set_telepathic(player, true);
  }
}

/* This command toggles whether or not the player wants to use the numpad on and
 * off. This affects only movement at this time, including quickmove.
 */
void toggle_numpad(){
  int u, d, r, l, ur, ul, dr, dl, q;
  if(cmd_data[CMD_UP_RIGHT] == KEY_PPAGE){
    u = 'k';  d = 'j';  l = 'h';  
    r = 'l';  ur = 'u'; ul = 'y';
    dr = 'n'; dl = 'b'; q = 'q';
  }
  else{
    u = KEY_UP;     d = KEY_DOWN;    l = KEY_LEFT;  
    r = KEY_RIGHT;  ur = KEY_PPAGE;  ul = KEY_HOME;
    dr = KEY_NPAGE; dl = KEY_END;    q = KEY_B2;
  }
  
  for(int i = 0; i<CMD_MAX+1; i++){
    if(cmd_data[i] == u
       || cmd_data[i] == d
       || cmd_data[i] == l
       || cmd_data[i] == r
       || cmd_data[i] == ur
       || cmd_data[i] == ul
       || cmd_data[i] == dr
       || cmd_data[i] == dl
       || cmd_data[i] == q){
      msg_promptchar("Cannot rebind keys - custom keybinds in place.");
      return;
    }
  }
  cmd_data[CMD_UP] = u;
  cmd_data[CMD_DOWN] = d;
  cmd_data[CMD_LEFT] = l;
  cmd_data[CMD_RIGHT] = r;
  cmd_data[CMD_UP_RIGHT] = ur;
  cmd_data[CMD_UP_LEFT] = ul;
  cmd_data[CMD_DOWN_RIGHT] = dr;
  cmd_data[CMD_DOWN_LEFT] = dl;
  cmd_data[CMD_QCKMV] = q;
}

/* This function allows a player to ascend or descend a staircase if they are
 * standing on one. The id is used to determine if they are trying to ascend
 * or descend by checking the id of the items on the tile.
 */
void xscend(int id){
  int x = player->x;
  int y = player->y;
  struct map_t* m = cur_map;

  for(struct item_map_t* items = m->items;
      items != NULL;
      items = items->next){
    if(x == items->x && y == items->y){
      for(struct item_map_node_t* i = items->first;
	  i != NULL;
	  i = i->next){
	if(i->item->id == id){
	  struct item_use_t* data = 
	    (struct item_use_t*)Calloc(1, sizeof(item_use_t));
	  data->item = i->item;
	  i->item->use(data);
	}
      }
      return;
    }
  }
}

/* Displays the user manual.
 */
void manual(){
  FILE *f = fopen("./manuals/manual.txt","r");
  if(f == NULL){
    msg_add("Manual missing!");
    return;
  }

  int section = 0; 
  int page = 0;
  char ch = 0; //the character that the player presses

  while(ch != 'q'){
    clear();
    int buffer_pos = 0;

    //Read the entire file into memory
    fseek(f, 0, SEEK_END);
    int buffer_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *file = (char*)calloc(buffer_size + 1, sizeof(char));
    fread(file, buffer_size, 1, f);
    fclose(f);

    buffer_pos = 0;

    //Turn the file into an array of lines
    int num_lines = 10;
    int ar_pos = 0;
    char **lines = (char**)calloc(num_lines, sizeof(char*));
    while(buffer_pos < buffer_size){
      //Check for realloc
      if(ar_pos >= num_lines){
	num_lines *= 2;
	lines = (char**)realloc(lines, num_lines * sizeof(char*)); 
      }

      /* Get data from file, put it into array,
       * -3 leaves room for border, accounts for \0
       */
      lines[ar_pos] = (char*)calloc(TERMINAL_WIDTH-3, sizeof(char));
      int s_pos = buffer_pos;
      buffer_pos += TERMINAL_WIDTH-4;
    
      /* Copy end of string into the buffer for the current line, 
       * with word wrap
       */
      if(buffer_pos >= buffer_size){
	strncpy(lines[ar_pos], file+s_pos, TERMINAL_WIDTH-3);
	buffer_pos = buffer_size;
      }
      else{
	while(file[buffer_pos] != ' '){
	  buffer_pos--;
	}
      }
      strncpy(lines[ar_pos], file+s_pos, buffer_pos-s_pos);
      int newline = strcspn(lines[ar_pos], "\n");
      if(newline != strlen(lines[ar_pos])){
	buffer_pos -= strlen(lines[ar_pos]) - newline-1; //1 to get around newline
	lines[ar_pos][newline] = 0;
      }
      ar_pos++;
    }

    //Print the top and bottom
    mvaddch(0,0, '+');
    mvaddch(0,TERMINAL_WIDTH-1, '+');
    mvaddch(TERMINAL_HEIGHT-1,0, '+');
    mvaddch(TERMINAL_HEIGHT-1, TERMINAL_WIDTH-1, '+');
    //Add top and bottom border
    mvaddch(1,0, ACS_VLINE);
    mvaddch(TERMINAL_HEIGHT-2,0, ACS_VLINE);
    mvaddch(1, TERMINAL_WIDTH-1, ACS_VLINE);
    mvaddch(TERMINAL_HEIGHT-3,0, ACS_VLINE);
    mvaddch(TERMINAL_HEIGHT-3, TERMINAL_WIDTH-1, ACS_VLINE);
    mvaddch(TERMINAL_HEIGHT-2, TERMINAL_WIDTH-1, ACS_VLINE);
    for(int i=1; i < TERMINAL_WIDTH-1; i++){
      mvaddch(0,i, ACS_HLINE);
      mvaddch(TERMINAL_HEIGHT-1, i, ACS_HLINE);
    }

    for(int j=2;  j < TERMINAL_HEIGHT-3; j++){
      mvaddch(j,0, ACS_VLINE);
      if(page*(TERMINAL_HEIGHT-5)+j-2 < ar_pos){
	mvaddstr(j,2, lines[page*(TERMINAL_HEIGHT-5)+j-2]);}
      mvaddch(j, TERMINAL_WIDTH-1, ACS_VLINE);
      
    } //for

    //Add quick nav information
    mvaddstr(TERMINAL_HEIGHT-2, 2, "< > To navigate, 1-5 to skip, q to quit");

    //Get input
    ch = getch();
    while(ch != '<' && ch != '>' && ch != 'q' && ch != 'Q'
	  && ch != '0' && ch != '1' && ch != '2'
	  && ch != '3' && ch != '4' && ch != '5'){
      ch = getch();}

    //If the section or page is changed, change it
    ////Change back
    if(ch == '<'){
      if(page==0 && section > 0){
	section--;
      }
      else if(page > 0){page--;}
    }
    ////Change forward
    else if(ch == '>'){
      if(section < 5){
	if((page+1)*(TERMINAL_HEIGHT-2-3) >= ar_pos){
	  section++;
	  page = 0;
	}
	else{page++;}
      }
    }
    ////Jump to a section
    else{
      section = ch - '0';
      page = 0;
    }
    if(section != 0){
      char path[17];
      sprintf(path, "./manuals/%d.txt", section);
      f = fopen(path, "r");
    }
    else{
      f = fopen("./manuals/manual.txt", "r");
    }
    free(file);
    if(f == NULL){return;} //This should happen when q is pressed
  } //while
}

/* This command will let the user type a command
 */
void analyze_cmd_extended(){
  curs_set(1);
  
  int ch = (int)' ';
  int ret_pos = 0;
  char* ret = (char*)Calloc(MAX_MSG_LEN-1, sizeof(char));
  
  while(ch != '\n'){
    move(MSG_ROW, 0);
    addch('#' | COLOR_PAIR(CP_GREEN_BLACK));
    for(int i=0; i < MAX_MSG_LEN; i++){
      if(i < strlen(ret)){
	addch(ret[i] | COLOR_PAIR(CP_GREEN_BLACK));
      }
      else{
	addch(' ' | COLOR_PAIR(CP_GREEN_BLACK));
      }
    }

    int i = 0; bool done = false;
    //Predict what they want
    if(strlen(ret) > 0){
      //Loop through all the extended commands
      for(i=0; i <= EXT_MAX && !done; i++){
	//For each, examine for similarity to what's been typed
	for(int j=0; j < strlen(ret) && j < strlen(cmd_data_extended[i]); j++){
	  if(cmd_data_extended[i][j] != ret[j]){break;}
	  //If everything typed so far is equal to a command, autocomplete
	  if(j == strlen(ret)-1){
	    move(MSG_ROW, strlen(ret)+1);
	    for(int k=j+1; k < strlen(cmd_data_extended[i]); k++){
	      addch(cmd_data_extended[i][k] | (use_8_colors ?
					       COLOR_PAIR(CP_YELLOW_BLACK)
					       : COLOR_PAIR(CP_GREY_BLACK)));
	    }
	    done = true; i--; break;
	  }
	}
      }
    }

    move(MSG_ROW, strlen(ret)+1);
    
    ch = getch();
    //special case for backspace
    if((ch==KEY_BACKSPACE || ch == 127) && ret_pos>=0){
      ret_pos--;
      ret[ret_pos]=0;
    }
    else if(strlen(ret)<=MAX_MSG_LEN){
      //do not add \n
      if(ch!='\n' && (isalnum(ch) || ch=='-')){
	ret[ret_pos]=ch;
	ret_pos++;
      }
      else if(ch == '\n' && i != EXT_MAX+1){
	ret = cmd_data_extended[i];
      }
    }
    
  }

  curs_set(0);
  
  if(strcmp(ret, cmd_data_extended[EXT_TOGGLE_NUMPAD])==0){
    toggle_numpad();
  }
  else if(strcmp(ret, cmd_data_extended[EXT_NUM_LOCK])==0){
    for(int i=0; i < CMD_MAX; i++){
      if(cmd_data[i] == KEY_UP){
	cmd_data[i] = '8';}
      else if(cmd_data[i] == KEY_DOWN){
	cmd_data[i] = '2';}
      else if(cmd_data[i] == KEY_LEFT){
	cmd_data[i] = '4';}
      else if(cmd_data[i] == KEY_RIGHT){
	cmd_data[i] = '6';}
      else if(cmd_data[i] == KEY_PPAGE){
	cmd_data[i] = '9';}
      else if(cmd_data[i] == KEY_NPAGE){
	cmd_data[i] = '3';}
      else if(cmd_data[i] == KEY_HOME){
	cmd_data[i] = '7';}
      else if(cmd_data[i] == KEY_END){
	cmd_data[i] = '1';}
      else if(cmd_data[i] == KEY_B2){
	cmd_data[i] = '5';}
    }
  }
  else if(strcmp(ret, cmd_data_extended[EXT_QUIT])==0){
    endwin();
    exit(0);
  }
  else if(strcmp(ret, cmd_data_extended[EXT_8_COLORS])==0){
    use_8_colors = !use_8_colors;
  }
}

/* This function will process user input and make the game respond to that.
 */
void analyze_cmd(int cmd, int* x, int* y){
  if(cmd == cmd_data[CMD_QCKMV]){
    qckmv_cmd = getch();
    if(qckmv_cmd == cmd_data[CMD_UP]
       || qckmv_cmd == cmd_data[CMD_DOWN] 
       || qckmv_cmd == cmd_data[CMD_LEFT]
       || qckmv_cmd == cmd_data[CMD_RIGHT]
       || qckmv_cmd == cmd_data[CMD_UP_RIGHT]
       || qckmv_cmd == cmd_data[CMD_UP_LEFT]
       || qckmv_cmd == cmd_data[CMD_DOWN_RIGHT]
       || qckmv_cmd == cmd_data[CMD_DOWN_LEFT]){
      qckmv = true;
      cmd = qckmv_cmd;
    }
    else{return;}
  }
  if(cmd == cmd_data[CMD_UP]){*y-=1;}
  else if(cmd == cmd_data[CMD_DOWN]){*y+=1;}
  else if(cmd == cmd_data[CMD_LEFT]){*x-=1;}
  else if(cmd == cmd_data[CMD_RIGHT]){*x+=1;}
  else if(cmd == cmd_data[CMD_UP_LEFT]){ 
    *x-=1;*y-=1;
  }
  else if(cmd == cmd_data[CMD_UP_RIGHT]){ 
    *x+=1;*y-=1;
  }
  else if(cmd == cmd_data[CMD_DOWN_LEFT]){
    *x-=1;*y+=1;
  }
  else if(cmd == cmd_data[CMD_DOWN_RIGHT]){
    *x+=1;*y+=1;
  }
  else if(cmd == cmd_data[CMD_OPEN]){
    open_tile(player->x, player->y, getch());
  }
  else if(cmd == cmd_data[CMD_CLOSE]){
    close_tile(player->x, player->y, getch());
  }else if(cmd == cmd_data[CMD_PICKUP]){
    pickup_tile(player, cur_map);
  }else if(cmd == cmd_data[CMD_INVENTORY]){
    display_inventory();
  }else if(cmd == cmd_data[CMD_REMAP]){
    cmd_remap();
  }else if(cmd == cmd_data[CMD_EXTENDED]){
    analyze_cmd_extended();
  }else if(cmd == cmd_data[CMD_EXAMINE]){
    map_examine_tile(cur_map);
  }else if(cmd == cmd_data[CMD_ASCEND]){
    xscend(ITEM_UP_STAIR);
  }else if(cmd == cmd_data[CMD_DESCEND]){
    xscend(ITEM_DOWN_STAIR);
  }else if(cmd == cmd_data[CMD_MANUAL]){
    manual();
  }else if(cmd == cmd_data[CMD_DEBUG]){
    debug();
  }
}


/* This function takes in a target creature's coordinates and determines if that
 * tile is in the range of the seer creature. Returns true if it is, false
 * otherwise.
 */
bool coord_in_range(int target_x, int target_y, struct creature_t *seer){
  return sqrt(pow((target_x - seer->x), 2) + pow((target_y - seer->y), 2))
	      <= creature_see_distance(seer);
}

/* This function takes in a target creature and determines if that tile is 
 * in the range of the seer creature. Returns true if it is, false otherwise.
 */
bool in_range(struct creature_t *target, struct creature_t *seer){
  return coord_in_range(target->x, target->y, seer);
}

bool qckmv_continue(struct map_t* map, int x, int y, int qckmv_cmd){
  int cur_tile = map->tiles[y*map->width+x];
  int ul=map->tiles[(y-1)*map->width+x-1];
  int ur=map->tiles[(y-1)*map->width+x+1];
  int dl=map->tiles[(y+1)*map->width+x-1];
  int dr=map->tiles[(y+1)*map->width+x+1];
  int u=map->tiles[(y-1)*map->width+x];
  int d=map->tiles[(y+1)*map->width+x];
  int l=map->tiles[y*map->width+x-1];
  int r=map->tiles[y*map->width+x+1];

  //Check for at-corner in corridor
  if(cur_tile == TILE_CORRIDOR){
    if(qckmv_cmd==cmd_data[CMD_DOWN]
       && (ul==TILE_CORRIDOR 
	   || ur==TILE_CORRIDOR)){
      return false;
    }
    if(qckmv_cmd==cmd_data[CMD_DOWN] 
       && (dl==TILE_CORRIDOR 
	   || dr==TILE_CORRIDOR)){
      return false;
    }
    if(qckmv_cmd==cmd_data[CMD_LEFT]
       && (ul==TILE_CORRIDOR 
	   || dl==TILE_CORRIDOR)){
      return false;
    }
    if(qckmv_cmd==cmd_data[CMD_RIGHT]
       && (ur==TILE_CORRIDOR 
	   || dr==TILE_CORRIDOR)){
      return false;
    }
  }

  //Check for change in tile type during quickmove
  if(qckmv_cmd==cmd_data[CMD_UP]
     && (u!=cur_tile 
	 || tile_data[ul].stopme 
	 || tile_data[ur].stopme
	 || map_tile_stopme(map, x-1, y-1)
	 || map_tile_stopme(map, x, y-1)
	 || map_tile_stopme(map, x+1, y-1)
	 || (cur_tile == TILE_FLOOR 
	     && (r != ur || l != ul))
	 )){
    return false;
  }
  if(qckmv_cmd==cmd_data[CMD_DOWN]
     && (d!=cur_tile 
	 || tile_data[dl].stopme 
	 || tile_data[dr].stopme
	 || map_tile_stopme(map, x-1, y+1)
	 || map_tile_stopme(map, x, y+1)
	 || map_tile_stopme(map, x+1, y+1)
	 || (cur_tile == TILE_FLOOR 
	     && (r != dr || l != dl))
	 )){
    return false;
  }
  if(qckmv_cmd==cmd_data[CMD_LEFT]
     && (l!=cur_tile 
	 || tile_data[dl].stopme 
	 || tile_data[ul].stopme
	 || map_tile_stopme(map, x-1, y-1)
	 || map_tile_stopme(map, x-1, y)
	 || map_tile_stopme(map, x-1, y+1)
	 || (cur_tile == TILE_FLOOR 
	     && (u != ul || d != dl))
	 )){
    return false;
  }
  if(qckmv_cmd==cmd_data[CMD_RIGHT]
     && (r!=cur_tile 
	 || tile_data[dr].stopme 
	 || tile_data[ur].stopme
	 || map_tile_stopme(map, x+1, y-1)
	 || map_tile_stopme(map, x+1, y)
	 || map_tile_stopme(map, x+1, y+1)
	 || (cur_tile == TILE_FLOOR
	     && (u != ur || d != dr))
	 )){
    return false;
  }
  if((qckmv_cmd==cmd_data[CMD_UP_LEFT] 
      && (ul!=cur_tile
	  || map_tile_stopme(map, x-1, y-1)
	  || map_tile_stopme(map, x, y-1)
	  || map_tile_stopme(map, x-1, y)))
     || (qckmv_cmd==cmd_data[CMD_UP_RIGHT]
	 && (ur!=cur_tile
	     || map_tile_stopme(map, x+1, y-1)
	     || map_tile_stopme(map, x, y-1)
	     || map_tile_stopme(map, x+1, y)))
      || (qckmv_cmd==cmd_data[CMD_DOWN_LEFT] 
	  && (dl!=cur_tile
	      || map_tile_stopme(map, x-1, y+1)
	      || map_tile_stopme(map, x, y+1)
	      || map_tile_stopme(map, x-1, y)))
      || (qckmv_cmd==cmd_data[CMD_DOWN_RIGHT]
	  && (dr!=cur_tile
	      || map_tile_stopme(map, x+1, y+1)
	      || map_tile_stopme(map, x, y+1)
	      || map_tile_stopme(map, x+1, y)))){
    return false;
  }
  return true;
}

void game_init(int seed){
  if(seed == 0){seed = time(NULL);}

  WIN = initscr(); 

  color_init(); cbreak(); noecho();
  keypad(stdscr, true); curs_set(0);  

  //Set flags
  term = getenv("TERM");
  qckmv_cmd = 0;
  qckmv = false;
  use_8_colors = term != NULL && !strstr(term, "xterm");
  use_num_lock = false;
  use_numpad = true;

  //Initialize game data
  tile_data_init();
  item_data_init();
  classes_data_init();
  creature_data_init();
  player_init();
  inventory_init(player);
  status_init();
  cmd_init();
  num_turns = 0;

  //Setup floor
  cur_map = (map_t*)Calloc(1,sizeof(map_t));
  map_init(cur_map,
	   TERMINAL_WIDTH,
	   TERMINAL_HEIGHT-3,
	   DEFAULT_ITEMS_STACK_SIZE,
	   1);
  map_draw_random_rooms(cur_map, -1, -1);
  map_cleanup(cur_map);
  map_draw_borders(cur_map);

  map_add_creature(cur_map, player);
  map_place_down_stair_randomly(cur_map);
  map_place_spawners(cur_map);  

  //Place character randomly
  while(cur_map->tiles[player->y*cur_map->width+player->x]!=TILE_FLOOR){
    player->x=rand() % cur_map->width;
    player->y=rand() % cur_map->height;
  }
}

/* This function is called whenever the player is killed. It will prompt the 
 * player if they would like to continue. If they choose not to, it will free
 * everything and exit the game. If they choose to continue, it will free 
 * everything and restart the game.
 */
void game_over(){
    bool done = false;
    while(!done){
      char res = msg_promptchar("Game Over! Restart? (Y/n)");
      if(res == 'n' || res =='N'){
	res = msg_promptchar("Are you sure you want to quit? (y/N)");
	if(res == 'y' || res == 'Y'){

	  //TODO: Free everything.

	  endwin();
	  printf("You died!\n");
	  exit(0);
	}
      }
      else{
	done = true;}
    }

    //TODO: Free everything.
    free(player);

    game_init(0);
}
