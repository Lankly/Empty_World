#ifndef DISPLAY_H
#define DISPLAY_H

/**
 * The User Interface is divided into sections, and has several possible modes.
 *
 * +--------------------------------------+----------------+
 * | ALERT LINE                           |                |
 * |                                      |                |
 * |                                      |                |
 * |                                      |                |
 * |                                      |                |
 * |             PRIMARY                  |   SECONDARY    |
 * |                                      |                |
 * |                                      |                |
 * |                                      |                |
 * |                                      |                |
 * |                                      |                |
 * |                                      |                |
 * |                                      |                |
 * +--------------------------------------+----------------+
 * |                                      |                |
 * |                                      |                |
 * |               ALERT                  |   SECONDARY2   |
 * |                                      |                |
 * |                                      |                |
 * +--------------------------------------+----------------+
 * |                                                       |
 * |                        STATUS                         |
 * +-------------------------------------------------------+
 *
 * Possible Modes:
 * Classic:  ALERT LINE + PRIMARY   + STATUS
 * Detail:   PRIMARY    + SECONDARY + ALERT       + STATUS
 * Expanded: PRIMARY    + SECONDARY + ALERT       + SECONDARY2  + STATUS
 */

/****************
 * USEFUL TYPES *
 ****************/

typedef enum display_mode_t {
  MODE_CLASSIC,
  MODE_DETAIL,
  MODE_EXPANDED,
} display_mode_t;
typedef enum pane_t{
  PANE_PRIMARY,
  PANE_SECONDARY,
  PANE_ALERT,
  PANE_STATUS
} pane_t;
typedef enum pane_style_t{
  STYLE_NONE,
  STYLE_CLASSIC,                /* Uses +, -, and | */
  STYLE_CLASSIC_MERGED,
  STYLE_CLEAN,                  /* Uses ACS characters */
  STYLE_CLEAN_MERGED
} pane_style_t;


/***********************
 * FUNCTION PROTOTYPES *
 ***********************/

/* Pane functions */

/**
 * Changes the style of all panes to the specified style.
 */
void change_panes_style(pane_style_t s);

/**
 * Increases or decreases the width or height of the specied pane by the
 * given amounts.
 */
void resize_pane(pane_t p, int additional_width, int additional_height);

/**
 * Refreshes the contents of the specified pane.
 */
void refresh_pane(pane_t p);

/**
 * Writes the given test to the specified pane. If that pane is not supported
 * by the current display_mode, or the text is NULL or empty, this does nothing.
 *
 * PRIMARY will print the entirety of chars, until it reaches a 0.
 *
 * SECONDARY will print the entirety of chars, until it reaches 2 newlines in a
 * row. Each single 0 and '\n' means "end of current line".
 *
 * If the current mode shows the ALERT LINE instead of the ALERT area, and the
 * given text is longer than can fit in the ALERT LINE, the game will be paused
 * and the user will be asked to press a key to see the next n characters of the
 * text until the user has been shown the entire message.
 *
 * The ALERT area will show alert history, in addition to the current message.
 * As a convenience, commands are provided to scroll through this and the ALERT
 * LINE history.
 *
 * STATUS will try to print as much as possible from chars, until it runs out of
 * room.
 */
void write_to_pane(pane_t p, int *text);


/* Display Mode functions */

/**
 * Initializes internal display data. MUST be called before any other functions.
 */
void display_init();

/**
 * Changes the current mode to the specified one.
 */
void change_display_mode(display_mode_t d);

/* History Functions */

/**
 * Scrolls backwards through the history in the ALERT LINE or ALERT area.
 * In the alert area, if that history's text was on screen, it will be
 * highlighted instead of scrolling. If the end of history was reached,
 * will loop back to the front.
 */
void history_prev();

/**
 * Like history_prev, but forward instead of backward.
 */
void history_next();

#endif
