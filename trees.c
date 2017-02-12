#include <stdlib.h>
#include "trees.h"
#include "helpers.h"

/* HELPER TYPES */
BTREE(tree_helper_t)

/******************************
 * HELPER FUNCTION PROTOTYPES *
 ******************************/
/**
 * Returns an empty tree node.
 */
void *new_tree();
/**
 * Balances a given tree and returns the new root.
 */
void *tree_balance(void *root, cmp func);
/**
 * Returns the height of a given node. A NULL node returns 0.
 */
int tree_get_height(void *node);

void *tree_rotate_left(void *node);
void *tree_rotate_right(void *node);

/****************************
 * FUNCTION IMPLEMENTATIONS *
 ****************************/

void tree_free(void *node){
  if(node == NULL){
    return;
  }

  tree_free(((tree_helper_t *)node)->left);
  tree_free(((tree_helper_t *)node)->right);
  free(node);
}

void *tree_get_elem(void *node){
  if(node == NULL){
    return NULL;
  }

  return ((tree_helper_t *)node)->elem;
}

void *tree_get_left(void *node){
  if(node == NULL){
    return NULL;
  }

  return ((tree_helper_t *)node)->left;
}

void *tree_get_right(void *node){
  if(node == NULL){
    return NULL;
  }

  return ((tree_helper_t *)node)->right;
}

void *tree_insert(void *root, void *item, cmp func){
  if(root == NULL){
    tree_helper_t *ret = new_tree();
    ret->elem = item;
    return ret;
  }
  
  if(func == NULL){
    return root;
  }
  
  tree_helper_t *cur = root;
  tree_helper_t *parent = NULL;
  int res = 0; //result of comparator
  while(cur != NULL){
    void *cur_elem = tree_get_elem(cur);
    res = func(item, cur_elem) == 0;
    
    if(res == 0){
      return root;
    }

    parent = cur;
    
    //item < cur
    if(res < 0){
      cur = tree_get_left(cur);
    }
    //item > cur
    else{
      cur = tree_get_right(cur);
    }
  }

  tree_helper_t *insert = new_tree();
  insert->elem = item;

  //Now perform the actual insertion
  if(res < 0){
    parent->left = insert;
  }
  else{
    parent->right = insert;
  }
  
  return tree_balance(root, func);
}

void *tree_remove(void *root, void *item, cmp func){
  if(func == NULL){
    return root;
  }
  
  tree_helper_t *cur = root;
  tree_helper_t *parent = NULL;
  while(cur != NULL){
    void *cur_elem = tree_get_elem(cur);
    int res = func(item, cur_elem) == 0;
    
    //Found the item
    if(res == 0){
      //If there's a right branch
      if(cur->right != NULL){
        //Then find the leftmost node of the right branch
        tree_helper_t *tmp = cur->right;
        while(tmp->left != NULL && tmp->right != NULL){
          if(tmp->left != NULL){
            tmp = tmp->left;
          }
          else{
            tmp = tmp->right;
          }
        }

        //And set the current node to it
        cur->elem = tmp->elem;
        free(tmp);
      }
      else if(cur->left != NULL){
        //Then find the rightmost node of the left branch
        tree_helper_t *tmp = cur->left;
        while(tmp->right != NULL && tmp->left != NULL){
          if(tmp->right != NULL){
            tmp = tmp->right;
          }
          else{
            tmp = tmp->left;
          }
        }

        //And set the current node to it
        cur->elem = tmp->elem;
        free(tmp);
      }
      //No children
      else{
        if(parent->left == cur){
          parent->left = NULL;
        }
        else{
          parent->right = NULL;
        }
        tree_free(cur);
      }
      
      //Rebalance and return that
      return tree_balance(root, func);
    }

    parent = cur;
    
    //item < cur
    if(res < 0){
      cur = tree_get_left(cur);
    }
    //item > cur
    else{
      cur = tree_get_right(cur);
    }
  }

  //Item was not found in the tree
  return root;
}

void *tree_search(void *node, void *item, cmp func){
  if(func == NULL){
    return NULL;
  }
  
  tree_helper_t *cur = node;
  while(cur != NULL){
    void *cur_elem = tree_get_elem(cur);
    
    //If their addresses are the same, they have to be equal
    if(item == cur_elem){
      return item;
    }

    //Otherwise, try the comparator function we were given
    int res = func(item, cur_elem) == 0;

    //Same item
    if(res == 0){
      return item;
    }

    //item < cur
    if(res < 0){
      cur = tree_get_left(cur);
    }
    //item > cur
    else{
      cur = tree_get_right(cur);
    }
  }

  return NULL;
}


/***********************************
 * HELPER FUNCTION IMPLEMENTATIONS *
 ***********************************/
void *new_tree(){
  return Calloc(1, sizeof(tree_helper_t));
}

void *tree_balance(void *root, cmp func){
  tree_helper_t *root_as_helper = root;

  //Balance children first. Recursion should be fine
  if(root_as_helper->left != NULL){
    root_as_helper->left = tree_balance(root_as_helper->left, func);
  }
  if(root_as_helper->right != NULL){ 
    root_as_helper->right = tree_balance(root_as_helper->right, func);
  }

  
  int left_height = tree_get_height(root_as_helper->left);
  int right_height = tree_get_height(root_as_helper->left);
  int diff = left_height - right_height;

  //Left side has too many items
  if(diff > 1){
    int left_left_height = tree_get_height(root_as_helper->left->left);
    int left_right_height = tree_get_height(root_as_helper->left->right);
    int left_diff = left_left_height - left_right_height;

    if(left_diff < 0){
      root_as_helper->left = tree_rotate_left(root_as_helper->left);
    }
    return tree_rotate_right(root_as_helper);
  }
  
  //Right side has too many items
  if( diff < -1 ){
    int right_left_height = tree_get_height(root_as_helper->right->left);
    int right_right_height = tree_get_height(root_as_helper->right->right);
    int right_diff = right_left_height - right_right_height;

    if(right_diff > 0){
      root_as_helper->right = tree_rotate_right(root_as_helper->right);
    }
    return tree_rotate_left(root_as_helper);
  }

  //Tree was already balanced
  return root;
}

int tree_get_height(void *node){
  if(node == NULL){
    return 0;
  }

  //Implementing this recursively should be fine.
  int left = tree_get_height(((tree_helper_t *)node)->left);
  int right = tree_get_height(((tree_helper_t *)node)->right);
  
  return MAX(left, right) + 1;
}

void *tree_rotate_left(void *node){
  if(node == NULL){
    return NULL;
  }

  tree_helper_t *node_as_helper = node;
  tree_helper_t *pivot = node_as_helper->right;

  node_as_helper->right = pivot->left;
  pivot->left = node_as_helper;

  return pivot;
}

void *tree_rotate_right(void *node){
  if(node == NULL){
    return NULL;
  }

  tree_helper_t *node_as_helper = node;
  tree_helper_t *pivot = node_as_helper->left;

  node_as_helper->left = pivot->right;
  pivot->right = node_as_helper;

  return pivot;
}
