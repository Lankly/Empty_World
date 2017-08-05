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
 * @param items The number of items to allocate in a row.
 * @param size The size of each item.
 */
void *Calloc(size_t items, size_t size);

/**
 * Quits the game and displays an error message.
 * @param error_msg The error message to display.
 */
void quit(const char* error_msg);

/**
 * Sets up the game, including the player.
 */
void game_init();

/**
 * @returns A random dir_t (excluding DIR_COUNT).
 */
dir_t rand_dir();

/**
 * Converts an x-y coordinate pair into an index into an array representation of
 * that 2-D space. Returns -1 if x or y is negative, or if x is greater than
 * width.
 * @param x The x-coordinate.
 * @param y The y-coordinate.
 * @param width The width of the matrix.
 */
int get_coord_in_arr(int x, int y, int width);

/**
 * Calculates the distance between two points in a plane.
 * @param x1 The x-coordinate of the first coordinate pair.
 * @param y1 The y-coordinate of the first coordinate pair.
 * @param x2 The x-coordinate of the second coordinate pair.
 * @param y2 The y-coordinate of the second coordinate pair.
 * @returns The exact distance between the two coordinate pairs.
 */
double dist(int x1, int y1, int x2, int y2);

/**
 * Hashes the given integer. 
 * Stolen from https://stackoverflow.com/a/12996028
 * @param n Any integer.
 * @returns A random-looking integer based on that integer that is the same
 *          every time this function is called with that integer.
 */
unsigned int hash(unsigned int n);

/* Comparators */

/**
 * A comparator for integers.
 * @param item1 The first item for comparison. Must point to an integer.
 * @param item2 The second item for comparison. Must point to an integer.
 * @returns An integer indicating whether item1 was larger (>0), smaller(<0), or
 *          equal to item 2 (0).
 */
int int_cmp(void *item1, void *item2);

/**
 * A comparator for addresses.
 * @param item1 The first item for comparison. Can be anything, even NULL.
 * @param item2 The first item for comparison. Can be anything, even NULL.
 * @returns An integer indicating whether item1 was larger (>0), smaller(<0), or
 *          equal to item 2 (0).
 */
int addr_cmp(void *item1, void *item2);

#endif
