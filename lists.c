#include <stdlib.h>
#include "lists.h"

/* HELPER TYPES */
LINKED_LIST(ll_helper_t)
DOUBLY_LINKED_LIST(dll_helper_t)

/******************************
 * HELPER FUNCTION PROTOTYPES *
 ******************************/
void *ll_new();
void *dll_new();

/****************************
 * FUNCTION IMPLEMENTATIONS *
 ****************************/

void ll_free(void *ll){
  ll_helper_t *next = NULL;
  for(ll_helper_t *cur = ll; cur != NULL; cur = next){
    next = cur->next;
    free(cur);
  }
}

void dll_free(void *dll){
  if(dll == NULL){
    return;
  }
  
  dll_helper_t *cur = dll;
  while(cur->prev != NULL){
    cur = cur->prev;
  }

  ll_free(cur);
}

void *ll_get_elem(void *ll){
  if(ll == NULL){
    return NULL;
  }

  return ((ll_helper_t *)ll)->elem;
}

void *ll_get_next(void *ll){
  if(ll == NULL){
    return NULL;
  }

  return ((ll_helper_t *)ll)->next;
}

void *ll_get_prev(void *dll){
  if(dll == NULL){
    return NULL;
  }

  return ((dll_helper_t *)dll)->prev;
}

void *ll_insert(void *ll, void *item){
  ll_helper_t *new = ll_new();
  new->elem = item;
  new->next = ll;
  return new;
}

void *dll_insert(void *dll, void *item){
  dll_helper_t *new = dll_new();
  new->elem = item;
  new->next = dll;
  if(dll != NULL){
    new->prev = new->next->prev;
    new->next->prev = new;
  }

  return new;
}

void *ll_remove(void *ll, void *item, cmp func){
  if(ll == NULL || item == NULL){
    return ll;
  }

  //If it's the first node
  if(func(((ll_helper_t *)ll)->elem, item) == 0){
    ll_helper_t *next = ((ll_helper_t *)ll)->next;
    free(ll);
    return next;
  }

  //Otherwise, search the list
  for(ll_helper_t *cur = ll; cur->next != NULL; cur = cur->next){
    if(func(cur->next->elem, item) == 0){
      ll_helper_t *next = cur->next;
      cur->next = next->next;
      free(next);
      return ll;
    }
  }

  //Was not found
  return ll;
}

void *dll_remove(void *dll, void *item, cmp func){
  if(dll == NULL || item == NULL){
    return dll;
  }

  //Back up to first node
  dll_helper_t *cur = dll;
  while(cur->prev != NULL){
    cur = cur->prev;
  }
  
  //If it's the first node
  if(func(cur->elem, item) == 0){
    dll_helper_t *next = cur->next;
    next->prev = NULL;
    free(dll);
    return next;
  }

  //Otherwise, search the list
  for(dll_helper_t *cur = dll; cur->next != NULL; cur = cur->next){
    if(func(cur->next->elem, item) == 0){
      dll_helper_t *next = cur->next;
      next->next->prev = cur;
      cur->next = next->next;
      free(next);
      return dll;
    }
  }

  //Was not found
  return dll;
}

void *ll_search(void *ll, void *item, cmp func){
  for(ll_helper_t *cur = ll; cur != NULL; cur = cur->next){
    void *cur_elem = ll_get_elem(cur);
    
    //If their addresses are the same, they have to be equal
    if(item == cur_elem){
      return item;
    }

    //Otherwise, try the comparator function we were given
    if(func != NULL && func(item, cur_elem) == 0){
      return item;
    }
  }

  return NULL;
}

void *dll_search(void *dll, void *item, cmp func){
  //We're just going to back up to the start of the dll
  dll_helper_t *cur = dll;
  while(cur != NULL && cur->prev != NULL){
    cur = cur->prev;
  }

  //And then use our other search function
  return ll_search(cur, item, func);
}

/***********************************
 * HELPER FUNCTION IMPLEMENTATIONS *
 ***********************************/

void *ll_new(){
  return Calloc(1, sizeof(ll_helper_t));
}

void *dll_new(){
  return Calloc(1, sizeof(dll_helper_t));
}
