#ifndef LISTS_H
#define LISTS_H

#include <stdbool.h>
#include "helpers.h"

/**********************
 * USEFUL DEFINITIONS *
 **********************/
#define LINKED_LIST(LL_NAME) typedef struct LL_NAME {     \
    void *elem;                                           \
    struct LL_NAME *next;                                 \
  } LL_NAME;

#define DOUBLY_LINKED_LIST(DLL_NAME) typedef struct DLL_NAME {  \
    void *elem;                                                 \
    struct DLL_NAME *next;                                      \
    struct DLL_NAME *prev;                                      \
  } DLL_NAME;


/***********************
 * FUNCTION PROTOTYPES *
 ***********************/

/**
 * Frees a given linked_list, but does not change any of its elements.
 */
void ll_free(void *ll);

/**
 * Like ll_free, but for doubly_linked_lists
 */
void dll_free(void *dll);

/**
 * Returns the element of a given linked_list or doubly_linked_list.
 */
void *ll_elem(void *ll);

/**
 * Returns the next node of a linked_list or doubly_linked_list.
 */
void *ll_next(void *ll);

/**
 * Returns the previous node of a doubly_linked_list (dll MUST be a valid DLL).
 */
void *ll_prev(void *dll);

/**
 * Inserts an element into a given linked_list and returns the new head of that 
 * list.
 */
void *ll_insert(void *ll, void *item);

/**
 * Inserts an element at the location that dll is in.
 */
void *dll_insert(void *dll, void *item);

/**
 * Removes an element from a given linked_list and returns the head of that
 * list. If the comparator func is NULL, will simply check addresses. 
 *
 * Does nothing if ll or item is NULL.
 */
void *ll_remove(void *ll, void *item, cmp func);

/**
 * Like ll_remove, but for doubly_linked_lists
 */
void *dll_remove(void *dll, void *item, cmp func);

/**
 * Searches a linked_list or doubly_linked_list for a given item. If the 
 * comparator func is NULL, will simply check addresses.
 *
 * Returns the item if it was found, NULL if it was not.
 */
void *ll_search(void *ll, void *item, cmp func);

/**
 * Like ll_search, but for doubly_linked_lists.
 */
void *dll_search(void *dll, void *item, cmp func);

#endif
