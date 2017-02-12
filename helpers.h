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
/* Functions of this type should return 0 if the items are equal. If it makes
 * sense for the first item to be compared to the second, negative numbers 
 * should be used to show a less-than relationship and positive numbers to show
 * a greater_than relationship.
 *
 * If it does not make sense, any non-zero number will serve to show that they
 * are not equivalent.
 */
typedef int (*cmp)(void *, void *);


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

#endif
