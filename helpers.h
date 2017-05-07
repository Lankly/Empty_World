#ifndef HELPERS_H
#define HELPERS_H

#include <stdlib.h>

/**********************
 * USEFUL DEFINITIONS *
 **********************/

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

/****************
 * USEFUL TYPES *
 ****************/
/**
 * Functions of this type should return 0 if the items are equal. If it makes
 * sense for the first item to be compared to the second, negative numbers 
 * should be used to show a less-than relationship and positive numbers to show
 * a greater_than relationship.
 *
 * If it does not make sense, any non-zero number will serve to show that they
 * are not equivalent.
 */
typedef int (*cmp)(void *, void *);

/**
 * Enumeration of all possible directions. None of these should be set to a
 * number explicity. DIR_NONE should always be first, DIR_COUNT should always be
 * last.
 */
typedef enum { DIR_NONE, DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT,
               DIR_UL, DIR_UR, DIR_LL, DIR_LR, DIR_COUNT } dir_t;

/***********************
 * FUNCTION PROTOTYPES *
 ***********************/

/**
 * Like calloc, but will perform a check for NULL on memory returned.
 */
void *Calloc(size_t items, size_t size);

/**
 * Quits the game and displays an error message.
 */
void quit(const char* error_msg);

/**
 * A comparator for integers.
 */
int int_cmp(void *item1, void *item2);

/**
 * Sets up the game, including the player.
 */
void game_init();

/**
 * Returns a random dir_t (excluding DIR_COUNT).
 */
dir_t rand_dir();

/**
 * Turns a 2D coordinate into the corresponding index in a 1D array.
 */
int get_coord_in_arr(int x, int y, int width);

#endif
