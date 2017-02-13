#include <stdlib.h>
#include <string.h>
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
 * Helper functions for the write_to_pane(pane_t, int) function.
 *
 * If no message is provided for write_to_alert_* then it will simply print out
 * the history from where cur_hist is.
 */
void write_to_primary(int *chars);
void write_to_secondary(int *chars);
void write_to_alert_area(int *msg);
void write_to_alert_line(int *msg);
void write_to_status(int *stats);

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
int primary_height = 22;   /* 24 - status_area_height = 22 */
int secondary_width = 0;   /* Initially determined by terminal size */
int alert_width = 0;
int alert_height = 0;
int status_height = 2;

history_t *hist;
history_t *cur_hist;       /* Where we currently are in the history */
int hist_len;

display_mode_t mode;

WINDOW *WIN, *primary, *secondary, *alert, *alert_line, *status;


/****************************
 * FUNCTION IMPLEMENTATIONS *
 ****************************/

void change_panes_style(pane_style_t s){
  winlist_t *wins = get_cur_windows();

  //Need to put the windows in the spots where they should go
  reseat_windows();

  //Now change their borders
  while(wins != NULL){
    WINDOW *cur_win = ll_get_elem(wins);
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

  reseat_windows();
}

void refresh_pane(pane_t p){
  WINDOW *w = get_window(p);
  wrefresh(w);
}

void write_to_pane(pane_t p, int *text){ 
  switch(p){
  case PANE_PRIMARY:
    write_to_primary(text);
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

void display_init(){
  mode = MODE_CLASSIC;

  //Initialize all windows
  WIN = initscr();
  primary = newwin(primary_height, primary_width, 0, 0);
  secondary = newwin(0, 0, 0, 0);
  alert = newwin(0, 0, 0, 0);
  alert_line = newwin(1, primary_width, 0, 0);
  status = newwin(status_height, primary_width, 0, 0);

  //Add a border and position them.
  change_panes_style(STYLE_CLEAN_MERGED);
}

/***********************************
 * HELPER FUNCTION IMPLEMENTATIONS *
 ***********************************/

winlist_t *get_cur_windows(){
  winlist_t *ret = ll_insert(NULL, primary);
  ret = ll_insert(ret, status);

  if(mode != MODE_CLASSIC){
    ret = ll_insert(ret, alert);
  }
  else{
    ret = ll_insert(ret, alert_line);
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
  
  for(winlist_t *cur = wins; cur != NULL; cur = ll_get_next(cur)){
    WINDOW *cur_win = ll_get_elem(cur);
    if(cur_win == primary || cur_win == alert_line){
      mvwin(cur_win,
            centery - overall_height / 2,
            centerx - overall_width / 2);
    }
    else if(cur_win == secondary){
      mvwin(cur_win,
            centery - overall_height / 2,
            centerx - overall_width / 2 + primary_width);
    }
    else if(cur_win == alert){
      mvwin(cur_win,
            centery - overall_height / 2 + primary_height,
            centerx - overall_width / 2);
    }
    else if(cur_win == status){
      mvwin(cur_win,
            centery - overall_height / 2 + primary_height + alert_height,
            centerx - overall_width / 2);
    }
  }

  ll_free(wins);
}

void write_to_primary(int *chars){
  wclear(primary);
  wmove(primary, 0, 0);
  
  if(chars == NULL){
    return;
  }

  int primary_area = primary_width * primary_height;
  
  for(int i = 0; chars[i] != 0 && i < primary_area; i++){
    //waddch wraps to next line if at right border, so this is fine
    waddch(primary, i);
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
  wclear(alert);
  wmove(alert, 0, 0);

  for(; cur_hist != NULL; cur_hist = ll_get_next(cur_hist)){
    int *cur_str = ll_get_elem(cur_hist);
    int cursor_x, cursor_y;
    
    for(int i = 0; cur_str[i] != '\0'; i++){
      if(cur_hist == hist){
        waddch(alert, cur_str[i]);
      }
      else{
        waddch(alert, ((char)(cur_str[i])) | color_pair_dark());
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
  add_history(msg);
  wclear(alert_line);
  wmove(alert_line, 0, 0);

  if(cur_hist == NULL){
    return;
  }

  const char *more_msg = "[MORE]";
  int more_len = strlen(more_msg);
  
  int *cur_str = ll_get_elem(cur_hist);
  for(int i = 0; cur_str[i] != 0; i++){

    //TODO: Have this look ahead to words
    
    if(i != 0 && i == (i % (primary_width - more_len - 1))){
      waddstr(alert_line, more_msg);
      getch();
      wmove(alert_line, 0, 0);
      wclrtoeol(alert_line);
    }

    waddch(alert_line, cur_str[i]);
  }
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
    next_hist = ll_get_next(cur_hist);
  } while(next_hist != NULL);

  hist = dll_remove(hist, ll_get_elem(cur_hist), NULL);
  cur_hist = hist;
}
