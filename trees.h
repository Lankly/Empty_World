#ifndef TREES_H
#define TREES_H

#include <stdbool.h>
#include "helpers.h"

/**********************
 * USEFUL DEFINITIONS *
 **********************/
#define BTREE(LL_NAME) typedef struct LL_NAME {           \
    void *elem;                                           \
    struct LL_NAME *left;                                 \
    struct LL_NAME *right;                                \
  } LL_NAME;


/***********************
 * FUNCTION PROTOTYPES *
 ***********************/

/**
 * Frees a given tree, but none of the items inside of it.
 */
void tree_free(void *node);

/**
 * Creates a copy of the given tree.
 * @param node A valid tree.
 * @returns A valid tree that is identical to the given one.
 */
void *tree_copy(void *node);

/**
 * Returns the element of a given node.
 */
void *tree_get_elem(void *node);

/**
 * Returns the left child of a given node.
 */
void *tree_get_left(void *node);

/**
 * Returns the right child of a given node.
 */
void *tree_get_right(void *node);

/**
 * Inserts a given item into the given tree and balances it. Will return a new
 * tree if the root was NULL.
 *
 * Returns the new root of the tree after balancing.
 *
 * Otherwise, does nothing if the comparator is NULL, or if the item was already
 * inserted.
 *
 * @param root The desired tree to insert into.
 * @param item The desired element to add.
 * @param cmp The comparator function to use.
 * @returns The root of the tree after insertion. Sometimes root may be
 *          different than before insertion.
 */
void *tree_insert(void *root, void *item, cmp func);

/**
 * Removes a given item from the given tree and balances it.
 *
 * Returns the new root of the tree after balancing.
 *
 * Does nothing if the root or comparator is NULL, or if the item does not exist
 * in the tree.
 */
void *tree_remove(void *root, void *item, cmp func);

/**
 * Searches a tree for a given item. If the comparator func is NULL, will return
 * NULL.
 *
 * Returns the item if it was found, NULL if it was not.
 */
void *tree_search(void *node, void *item, cmp func);

#endif
