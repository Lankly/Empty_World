#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <curses.h>
#include "display.h"
#include "colors.h"
#include "lists.h"

/*********
 * TYPES *
 *********/

LINKED_LIST(winlist_t)
DOUBLY_LINKED_LIST(history_t)


/***************
 * DEFINITIONS *
 ***************/

#define MAX_HISTORY_LEN 16


/******************************
 * HELPER FUNCTION PROTOTYPES *
 ******************************/

/**
 * Returns a list of all the windows that are being used by the current display
 * mode.
 */
winlist_t *get_cur_windows();

/**
 * Returns the ncurses WINDOW associated with the specified window
 */
WINDOW *get_window(pane_t p);

/**
 * Moves each window to where it should be. This is useful for when the game is
 * started, or when the border style has been changed.
 */
void reseat_windows();

/**
 * Simply calls wclear on all current panes.
 */
void clear_all_panes();

/**
 * Helper functions for the write_to_pane(pane_t, int) function.
 *
 * If no message is provided for write_to_alert_* then it will simply print out
 * the history from where cur_hist is.
 */
void write_to_primary(int *chars, int width, int height);
void write_to_secondary(int *chars);
void write_to_alert_area(int *msg);
void write_to_alert_line(int *msg);
void write_to_status(int *stats);

/**
 * These functions help determine if the screen is large enough to accomodate
 * larger display modes.
 */
void calculate_secondary_width();
void calculate_alert_height();

/**
 * Helper functions for writing to the alert panes. They will automatically
 * update hist and cur_hist to the newest item, as well as incrementing
 * history_len if necessary. Does nothing if chars is NULL.
 */
void add_history(int *msg);
void remove_history();          /* Only call when full */

/*********************
 * PRIVATE VARIABLES *
 *********************/
/**
 * These defaults must not change, because the game must be compatible with the
 * standard terminal size 80x24. The user can change them later.
 */
int primary_width = 80;
int primary_height = 20;   /* 24 - status_area_height */
int secondary_width = 0;   /* Initially determined by terminal size */
int alert_width = 0;
int alert_height = 0;
int status_height = 4;

history_t *hist;
history_t *cur_hist;       /* Where we currently are in the history */
int hist_len;

display_mode_t mode;            /* The currently applied mode */
pane_style_t style;             /* The currently applied style */

WINDOW *WIN, *primary, *secondary, *alert, *alert_line, *status;


/****************************
 * FUNCTION IMPLEMENTATIONS *
 ****************************/

void display_init(){
  mode = MODE_CLASSIC;

  //Initialize all windows
  WIN = initscr();
  primary = newwin(primary_height, primary_width, 1, 1);
  secondary = newwin(0, 0, 0, 0);
  alert = newwin(0, 0, 0, 0);
  alert_line = newwin(1, primary_width, 0, 0);
  status = newwin(status_height, primary_width, 0, 0);

  refresh();
  
  //Add a border and position them.
  change_panes_style(STYLE_CLEAN);

  //Set the mode to EXPANDED if the screen can accomodate
  calculate_secondary_width();
  calculate_alert_height();
  if(secondary_width > 0 && alert_height > 0){
    change_display_mode(MODE_EXPANDED);
  }
  else{
    secondary_width = 0; alert_height = 0;
  }
}

void change_panes_style(pane_style_t s){
  style = s;
  winlist_t *wins = get_cur_windows();

  //Need to put the windows in the spots where they should go
  reseat_windows();

  //Now change their borders
  while(wins != NULL){
    WINDOW *cur_win = ll_elem(wins);
    switch(s){
    case STYLE_NONE:
      wborder(cur_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
      break;
    case STYLE_CLASSIC_MERGED:
    case STYLE_CLASSIC:
      wborder(cur_win, '|', '|', '-', '-', '+', '+', '+', '+');
      break;
    case STYLE_CLEAN:
      wborder(cur_win,
              ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
              ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
      break;
    case STYLE_CLEAN_MERGED:
      if(cur_win == primary){
        wborder(cur_win,
                ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
                ACS_ULCORNER,
                mode == MODE_CLASSIC ? ACS_URCORNER : ACS_TTEE,
                ACS_LTEE,
                ACS_RTEE);
      }
      else if(cur_win == secondary){
        wborder(cur_win,
                ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
                ACS_TTEE, ACS_URCORNER,
                mode == MODE_DETAIL ? ACS_RTEE : ACS_BTEE,
                ACS_RTEE);
      }
      else if(cur_win == alert){
        wborder(cur_win,
                ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
                ACS_LTEE, ACS_RTEE, ACS_LTEE, ACS_RTEE);
      }
      else if(cur_win == status){
        wborder(cur_win,
                ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
                ACS_LTEE, ACS_RTEE, ACS_LLCORNER, ACS_LRCORNER);
      }
      break;
    }

    wrefresh(cur_win);
    
    wins = ll_next(wins);
  }
  
  ll_free(wins);
}

void resize_pane(pane_t p, int additional_width, int additional_height){
  int maxx, maxy;
  getmaxyx(stdscr, maxy, maxx);

  int overall_width = additional_width + primary_width + secondary_width;
  int overall_height =
    additional_height + primary_height + alert_height + status_height;
  if(overall_width > maxx || overall_height > maxy)
    {
      return;
    }

  switch(p){
  case PANE_PRIMARY:
    if((additional_width + primary_width) < 0
       || (additional_height + primary_height) < 0){
      return;
    }
    primary_width += additional_width;
    primary_height += additional_height;

    wresize(primary, primary_height, primary_width);

    break;
  case PANE_SECONDARY:
    //You should only be able to change the secondary pane width
    if((additional_width + secondary_width) < 0){
      return;
    }
    secondary_width += additional_width;
    break;
  case PANE_ALERT:
    if((additional_width + alert_width) < 0
       || (additional_height + alert_height) < 0){
      return;
    }
    alert_width += additional_width;
    alert_height += additional_height;
    break;
  case PANE_STATUS:
    //You should not be able to change the status pane size directly.
    //fallthrough
  default:
    return;
  }

  //Changes might have also been made to our secondary pane
  if(mode == MODE_DETAIL){
    wresize(secondary, primary_height, secondary_width);
  }
  else if(mode == MODE_EXPANDED){
    wresize(secondary, primary_height + alert_height, secondary_width);
  }
    
  //Changes might have also been made to the status pane and alert line
  wresize(status, status_height, overall_width);
  wresize(alert_line, 1, primary_width);
}

void refresh_pane(pane_t p){
  WINDOW *w = get_window(p);
  wrefresh(w);
}

void write_to_pane(pane_t p, int *text, int width, int height){
  switch(p){
  case PANE_PRIMARY:
    write_to_primary(text, width, height);
    break;
  case PANE_SECONDARY:
    write_to_secondary(text);
    break;
  case PANE_ALERT:
    if(mode == MODE_CLASSIC){
      write_to_alert_line(text);
    }
    else{
      write_to_alert_area(text);
    }
    break;
  case PANE_STATUS:
    write_to_status(text);
    break;
  default:
    break;
  }

  refresh_pane(p);
}

/* Data Functions */

int get_pane_height(pane_t p){
  switch(p){
  case PANE_PRIMARY:
    return primary_height - 2;
  case PANE_SECONDARY:
    return primary_height - 2 + (mode == MODE_EXPANDED ? alert_height : 0);
  case PANE_ALERT:
    return mode == MODE_CLASSIC ? 1 : alert_height - 2;
  case PANE_STATUS:
    return status_height - 2;
  }
  
  //Impossible to get here
  return -1;
}

int get_pane_width(pane_t p){
  switch(p){
  case PANE_PRIMARY:
    return primary_width - 2 ;
  case PANE_SECONDARY:
    return secondary_width - 2;
  case PANE_ALERT:
    return alert_width - (mode == MODE_CLASSIC ? 0 : 2);
  case PANE_STATUS:
    return primary_width + secondary_width - 2;
  }

  //Impossible to get here
  return -1;
}

/* Display Mode Functions */

void change_display_mode(display_mode_t d){
  clear_all_panes();
  mode = d;

  calculate_secondary_width();
  calculate_alert_height();

  /* Set sizes */ 
  if(d == MODE_CLASSIC){
    secondary_width = 0;
    alert_height = 0;
  }
  else{
    if(d == MODE_DETAIL){
      alert_width = primary_width + secondary_width;
    }
    else{ // d == MODE_EXPANDED
      alert_width = primary_width;
    }
  }
  
  change_panes_style(style);
}

void cycle_display_mode(){
  if(mode < MODE_EXPANDED){
    change_display_mode(mode + 1);
  }
  else{
    change_display_mode(0);
  }
}

/* Input Functions */

char *get_input(int max_len, int *prompt, char **autocomplete){
  if(max_len < 1 || prompt == NULL){
    return NULL;
  }

  char *to_return = Calloc(max_len + 1, sizeof(char));
  
  //Create a window to put the visuals in
  int wx = 0, wy = 0, width = 0;
  if(mode == MODE_CLASSIC){
    getbegyx(primary, wy, wx);
    wy++; wx++;
    width = get_pane_width(PANE_PRIMARY);
  }
  else{
    getbegyx(alert, wy, wx);
    wy += alert_height;
    wy++; wx++;
    width = alert_width;
  }
  WINDOW *temp = newwin(1, width, wy, wx);

  //Visible cursor (for typing)
  curs_set(1);

  int ch = '\0';
  while(ch != '\n'){
    wclear(temp);
    wmove(temp, 0, 0);
    
    size_t i = 0, cur_len = strlen(to_return), prompt_len = 0;
    bool found = false;

    //Print out prompt
    for(i = 0; prompt[i] != 0; i++){
      waddch(temp, prompt[i] | COLOR_PAIR(CP_GREEN_BLACK));
      prompt_len++;
    }
        
    //Print out what we've got so far
    for(i = 0; i < cur_len; i++){
      if(i < strlen(to_return)){
	waddch(temp, to_return[i] | COLOR_PAIR(CP_GREEN_BLACK));
      }
      else{
        break;
      }
    }

    //Predict what they want
    if(autocomplete != NULL){
      
      //Loop through all the autocomplete options
      for(i = 0; autocomplete[i] != NULL && !found; i++){
        
	//For each, examine for similarity to what's been typed
	for(size_t j = 0; j < cur_len && autocomplete[i][j] != '\0'; j++){

          if(autocomplete[i][j] != to_return[j]){break;}

          //If everything typed so far is equal to a command, autocomplete
	  if(j == cur_len - 1){
	    wmove(temp, 0, prompt_len + cur_len);

            for(size_t k = j + 1; k < strlen(autocomplete[i]); k++){
	      waddch(temp, autocomplete[i][k] | COLOR_PAIR(CP_GREY_BLACK));
	    }

            found = true;
            i--;
            break;
	  }
	}
      }
    }

    wmove(temp, 0, prompt_len + cur_len);
    wrefresh(temp);

    do{ch = getch();}while(ch == ERR);

    //special case for backspace
    if((ch == KEY_BACKSPACE || ch == 127) && cur_len > 0){
      to_return[cur_len - 1] = '\0';
    }
    else if(cur_len <= (size_t)max_len){
      //do not add \n
      if(ch != '\n' && (isalnum(ch) || ch == ' ' || ch=='-')){
	to_return[cur_len] = ch;
      }
      //Add autocomplete if enter is hit and we found a match
      else if(ch == '\n' && found){
        memcpy(to_return, autocomplete[i], strlen(autocomplete[i]));
      }
    }
  }

  //Set cursor back to invisible
  curs_set(0);

  delwin(temp);  
  return to_return;
}


/***********************************
 * HELPER FUNCTION IMPLEMENTATIONS *
 ***********************************/

winlist_t *get_cur_windows(){
  winlist_t *ret = ll_insert(NULL, primary);
  ret = ll_insert(ret, status);

  if(mode == MODE_CLASSIC){
    ret = ll_insert(ret, alert_line);
  }
  else{
    ret = ll_insert(ret, alert);
    ret = ll_insert(ret, secondary);
  }
  
  return ret;
}

WINDOW *get_window(pane_t p){
  switch(p){
  case PANE_SECONDARY:
    return secondary;
  case PANE_ALERT:
    return alert;
  case PANE_STATUS:
    return status;
  case PANE_PRIMARY:
  default:
    return primary;
  }
}

void reseat_windows(){
  winlist_t *wins = get_cur_windows();
  
  // Maximum possible x and y, based on the terminal size
  int maxx, maxy;
  getmaxyx(stdscr, maxy, maxx);

  //Center of the terminal
  int centerx, centery;
  
  centery = maxy/2;
  centerx = maxx/2;

  centery = centery < 0 ? 0 : centery;
  centerx = centerx < 0 ? 0 : centerx;

  int overall_width = primary_width + secondary_width;
  int overall_height = primary_height + alert_height + status_height;
  
  for(winlist_t *cur = wins; cur != NULL; cur = ll_next(cur)){
    WINDOW *cur_win = ll_elem(cur);
    if(cur_win == primary || cur_win == alert_line){
      mvwin(cur_win,
            centery - overall_height / 2,
            centerx - overall_width / 2);
    }
    else if(cur_win == secondary){
      wresize(cur_win, get_pane_height(PANE_SECONDARY) + 2, secondary_width);
      mvwin(cur_win,
            centery - overall_height / 2,
            centerx - overall_width / 2 + primary_width);
    }
    else if(cur_win == alert){
      wresize(cur_win, alert_height, alert_width);
      mvwin(cur_win,
            centery - overall_height / 2 + primary_height,
            centerx - overall_width / 2);
    }
    else if(cur_win == alert_line){
      wresize(cur_win, 1, get_pane_width(PANE_PRIMARY));
      mvwin(cur_win,
            centery - overall_height / 2 + 1,
            centerx - overall_width / 2 + 1);
    }
    else if(cur_win == status){
      wresize(cur_win, status_height, primary_width + secondary_width);
      mvwin(cur_win,
            centery - overall_height / 2 + primary_height + alert_height,
            centerx - overall_width / 2);
    }
    wrefresh(cur_win);
  }

  ll_free(wins);
}

void clear_all_panes(){
  winlist_t *wins = get_cur_windows();

  for(winlist_t *cur = wins; cur != NULL; cur = ll_next(cur)){
    WINDOW *win = ll_elem(cur);

    wclear(win);
    wrefresh(win);
  }
}

void write_to_primary(int *chars, int width, int height){
  //wclear(primary);
  wrefresh(primary);
  //  wmove(primary, 0, 0);
  
  if(chars == NULL){
    return;
  }

  int primary_area = primary_width * primary_height;
  
  for(int i = 0; chars[i] != 0 && i < primary_area; i++){
    //If width and height was passed, center this.
    if(width > 0 && width < primary_width && i % width == 0){
      wmove(primary,
            (i / width) + ((primary_height - height) / 2),
            (primary_width - width) / 2);
    }
    
    //waddch wraps to next line if at right border, so this is fine
    waddch(primary, chars[i]);
  }
}

void write_to_secondary(int *chars){
  wclear(secondary);
  wmove(secondary, 0, 0);
  
  if(chars == NULL){
    return;
  }

  int cur_line = 0;
  for(int i = 0; chars[i] != '\n' && chars[i] != '\0'; i++){
    wmove(secondary, cur_line, 0);
    int cur_index = 0;
    //We already know that the current line has characters in it.
    do{
      if(cur_index < secondary_width){
        waddch(secondary, chars[i]);
      }
      i++;
    }while(chars[i] != '\n' && chars[i] != '\0');
  }
}

void write_to_alert_area(int *msg){
  add_history(msg);
  wmove(alert, 1, 1);

  for(; cur_hist != NULL; cur_hist = ll_next(cur_hist)){
    int *cur_str = ll_elem(cur_hist);
    int cursor_x, cursor_y;
    
    for(int i = 0; cur_str[i] != '\0'; i++){
      if(cur_hist == hist){
        waddch(alert, cur_str[i]);
      }
      else{
        waddch(alert, ((char)(cur_str[i])) | color_pair_dark_text());
      }
      getyx(alert, cursor_y, cursor_x);
      if(cursor_y == alert_height - 2 && cursor_x == alert_width - 2){
        break;
      }
    }
  }

  cur_hist = hist;
}

void write_to_alert_line(int *msg){
  return;
  add_history(msg);
  wclear(alert_line);
  wrefresh(alert_line);

  if(cur_hist == NULL){
    return;
  }

  const char *more_msg = "[MORE]";
  int more_len = strlen(more_msg);
  
  int *cur_str = ll_elem(cur_hist);
  for(int i = 0; cur_str[i] != 0; i++){

    //TODO: Have this look ahead to words
    
    if(i != 0 && i == (i % (primary_width - more_len - 1))){
      waddstr(alert_line, more_msg);
    }

    waddch(alert_line, cur_str[i]);
  }

  wrefresh(alert_line);
}

void write_to_status(int *msg){
  wclear(status);
  wmove(status, 0, 0);

  if(msg == NULL){
    return;
  }

  int example_space = 0;;

  int i = 0;
  for(; msg[i] != 0; i++){
    waddch(status, msg[i]);

    if(example_space == 0 && ((char)(msg[i])) == ' '){
      example_space = msg[i];
    }
  }

  /* We fill with the example space just in case we opt for a white background
   * instead of the default black background.
   */
  while(i < (primary_width + secondary_width)){
    waddch(status, example_space);
  }
}

void calculate_secondary_width(){
  //Arbitrary limits
  //Panel will be capped at this maximum percentage.
  double max_secondary_width = .15;
  //Panel must meet this minimum tile count or become 0.
  int min_secondary_width = 8;


  //Get current terminal size
  int maxx, maxy;
  getmaxyx(stdscr, maxy, maxx);
  maxy = *(size_t *)(&maxy);
  
  //Translate percentages to number of tiles
  if((maxx - primary_width) >= min_secondary_width){
    max_secondary_width = (double)maxx * max_secondary_width;
  }
  else{
    max_secondary_width = 0;
  }

  secondary_width = MIN(maxx - primary_width, max_secondary_width);
}

void calculate_alert_height(){
  //Arbitrary limits
  //Panel will be capped at this maximum percentage.
  double max_alert_height = .15;
  //Panel must meet this minimum tile count or become 0.
  int min_alert_height = 4;


  //Get current terminal size
  int maxx, maxy;
  getmaxyx(stdscr, maxy, maxx);
  maxx = *(size_t *)(&maxx);

  
  //Translate percentages to number of tiles
  if((maxy - primary_height - status_height) >= min_alert_height){
    max_alert_height = (double)maxy * max_alert_height;
  }
  else{
    max_alert_height = 0;
  }

  alert_height = MIN(maxy - primary_height - status_height, max_alert_height);
}

void add_history(int *msg){
  if(msg == NULL){
    return;
  }

  if(hist_len == MAX_HISTORY_LEN){
    remove_history();
  }

  hist = dll_insert(hist, msg);
  cur_hist = hist;
}

void remove_history(){
  if(cur_hist == NULL){
    return;
  }

  history_t *next_hist = cur_hist;
  do{
    cur_hist = next_hist;
    next_hist = ll_next(cur_hist);
  } while(next_hist != NULL);

  hist = dll_remove(hist, ll_elem(cur_hist), NULL);
  cur_hist = hist;
}
