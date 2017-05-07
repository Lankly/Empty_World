#include <stdlib.h>
#include <stdarg.h>
#include "maps.h"
#include "creatures.h"
#include "tiles.h"
#include "trees.h"

/*********
 * TYPES *
 *********/

BTREE(property_tree_t)

struct map_t{
  int width, height;
  
  tile_t *tiles;

  struct map_t *north;
  struct map_t *south;
  struct map_t *east;
  struct map_t *west;
  struct map_t *up;
  struct map_t *down;

  property_tree_t *properties;
  clist_t *creatures;
};

/******************************
 * HELPER FUNCTION PROTOTYPES *
 ******************************/

bool xycoord_is_valid(map_t *m, int x, int y);
bool coord_is_valid(map_t *m, int c);
void map_draw_box(map_t *m, int x, int y, tile_t t);
void map_draw_circle(map_t *m, int x, int y, tile_t t);
void map_draw_line(map_t *m, int x, int y, tile_t t);


/************************
 * FUNCTION DEFINITIONS *
 ************************/

map_t *map_new(map_generator ptr){
  map_t *to_return = Calloc(1, sizeof(map_t));

  ptr(to_return);
  
  return to_return;
}

clist_t *get_creatures(map_t *m){
  if(m == NULL){
    return false;
  }

  return m->creatures;
}

/* PROPERTIES FUNCTIONS */

bool has_property(map_t *m, property_t p){
  if(m == NULL){
    return false;
  }
  
  return tree_search(m->properties, &p, int_cmp);
}

void add_property(map_t *m, property_t p){
  if(m == NULL){
    return;
  }

  m->properties = tree_insert(m->properties, &p, &int_cmp);
}

void add_properties(map_t *m, int num_properties, ...){
  if(num_properties < 0){
    return;
  }

  va_list args;
  va_start(args, num_properties);

  for(int i = 0; i < num_properties; i++){
    property_t arg = va_arg(args, attribute_t);

    add_property(m, arg);
  }
  
  va_end(args);
}

void remove_property(map_t *m, property_t p){
  if(m == NULL){
    return;
  }

  m->properties = tree_remove(m->properties, &p, &int_cmp);
}

void remove_properties(map_t *m, int num_properties, ...){
  if(num_properties < 0){
    return;
  }

  va_list args;
  va_start(args, num_properties);

  for(int i = 0; i < num_properties; i++){
    property_t arg = va_arg(args, property_t);
    remove_property(m, arg);
  }
  
  va_end(args);
}

/* TILE FUNCTIONS */

tile_t get_tile_at(map_t *m, int x, int y){
  if(m == NULL || !xycoord_is_valid(m, x, y)){
    return TILE_UNKNOWN;
  }

  return m->tiles[get_coord_in_arr(x, y, m->width)];
}

void set_tile_at(map_t *m, int x, int y, tile_t new){
  if(m == NULL || !xycoord_is_valid(m, x, y)){
    return;
  }

  m->tiles[get_coord_in_arr(x, y, m->width)] = new;
}

/* MAP GENERATORS */

void new_desert(map_t *base){
  if(base == NULL){
    return;
  }

  //TODO
  
  add_property(base, PROP_IS_DESERT);
}

void new_main_dungeon(map_t *base){
  if(base == NULL){
    return;
  }

  //TODO
  
  add_property(base, PROP_IS_MAIN_DUNGEON);
}


/*******************************
 * HELPER FUNCTION DEFINITIONS *
 *******************************/

bool xycoord_is_valid(map_t *m, int x, int y){
  return m != NULL
    && x > 0
    && y > 0
    && x < m->width
    && y < m->height;
}

bool coord_is_valid(map_t *m, int c){
  return m != NULL
    && c > 0
    && c < (m->width * m->height);
}

void map_draw_box(map_t *m, int x, int y, tile_t t){
  if(m == NULL || m->tiles == NULL || !xycoord_is_valid(m, x, y)){
    return;
  }

  for(int j = 0; j < y; j++){
    int coord = j * m->width;
    for(int i = 0; i < x; i++){
      coord++;
      m->tiles[coord] = t;
    }
  }
}

void map_draw_line(map_t *m, int c1, int c2, tile_t t, bool only_unknown){
  if(m == NULL || m->tiles == NULL
     || !coord_is_valid(m, c1) || !coord_is_valid(m, c2)){
    return;
  }
  
  //Extract coordinates
  int x1 = c1 % m->width, x2 = c2 % m->width;
  int y1 = c1 / m->width, y2 = c2 / m->width;
  
  float deltax = (x2 < x1) ? x1 - x2 : x2 - x1;
  float deltay = (y2 < y1) ? y1 - y2 : y2 - y1;
  float deltaerr = (deltax == 0) ? 0 : abs(deltay / deltax);
  float error = deltaerr - 0.5;
  int y = (y1 < y2) ? y1 : y2;
  
  for(int x = (x1 < x2) ? x1 : x2; x < (x1 < x2) ? x2 : x1; x++){ 
    int coord = get_coord_in_arr(x, y, m->width);
    if(!only_unknown || m->tiles[coord] != TILE_UNKNOWN){
      m->tiles[coord] = t;
    }
    
    error = error + deltaerr;
    if(error >= 0.5){
      y++;
      x--;
      error = error - 1.0;
    }
  }

  //Place tile while moving closer to other coord
  while(x1 != x2 && y1 != y2){

    int x_diff = x1 < x2 ? x2 - x1 : x1 - x2;
    int y_diff = y1 < y2 ? y2 - y1 : y1 - y2;
    
    if(x1 < x2 && (x_diff % ((x_diff < y_diff) ? y_diff / x_diff : 1)) > 0){
      x1++;
    }
    else if(x1 > x2){
      x1--;
    }
    else if(y1 < y2){
      y1++;
    }
    else{
      y1--;
    }
  }
}
