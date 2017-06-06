#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "maps.h"
#include "creatures.h"
#include "helpers.h"
#include "items.h"
#include "lists.h"
#include "tiles.h"
#include "trees.h"

/**********************
 * USEFUL DEFINITIONS *
 **********************/

#define DEFAULT_TERM_HEIGHT 24
#define DEFAULT_TERM_WIDTH 80


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
  ilist_t *items;
};

struct dijkstra_t{
  bool calculated;
  int width, height;

  int *cells;
};

/******************************
 * HELPER FUNCTION PROTOTYPES *
 ******************************/

/**
 * Determines whether or not a given coordinate exists on a given map.
 * A valid coordinate is inside the bounds of the map.
 * Always returns false on a NULL map.
 * @param m A valid map.
 * @param x The x-coordinate to check.
 * @param y The y-coordinate to check.
 * @returns True if the coordinate is valid.
 */
bool xycoord_is_valid(map_t *m, int x, int y);
/**
 * Determines whether or not a given coordinate exists on a given map.
 * A valid coordinate is inside the bounds of the map.
 * Always returns false on a NULL map.
 * @param m A valid map.
 * @param c An index into an array representation of a 2-D space.
 * @returns True if the coordinate is valid.
 */
bool coord_is_valid(map_t *m, int c);
/**
 * Draws a filled-in box of the given tile on the given map. If the box would
 * exceed the bounds of the map, the box will clip.
 * @param m A valid map.
 * @param c An index into an array representation of a 2-D space.
 * @param width The width of the box to draw.
 * @param height The height of the box to draw.
 * @param t The tile to draw the box with.
 */
void map_draw_box(map_t *m, int c, int width, int height, tile_t t);
/**
 * Draws a circle of the given tile on the given map. If the circle would exceed
 * the bounds of the map, the circle will clip.
 * @param m A valid map.
 * @param c An index into an array representation of a 2-D space.
 * @param r The radius of the circle.
 * @param t The tile to draw the circle with.
 */
void map_draw_circle(map_t *m, int c, int r, tile_t t);
/**
 * Draws a line from the first coordinate to the second coordinate.
 * @param m A valid map.
 * @param c1 An index into an array representation of a 2-D space.
 * @param c2 An index into an array representation of a 2-D space.
 * @param t The tile to draw the line with.
 * @param only_unknown If true, will only draw the line over TILE_UNKNOWN tiles.
 */
void map_draw_line(map_t *m, int c1, int c2, tile_t t, bool only_unknown);
/**
 * Draws walls around the rooms in a given map.
 * @param m A valid map.
 * @param t The tile to draw the walls with.
 */
void map_draw_walls(map_t *m, tile_t t);

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

/* CREATURES FUNCTIONS */

void map_add_creature(map_t *m, creature_t *c){
  if(m == NULL || c == NULL){
    return;
  }
  m->creatures = ll_insert(m->creatures, c);
}

void map_add_creatures(map_t *m, int num_creatures, ...){
  if(m == NULL || num_creatures < 1){
    return;
  }

  va_list args;
  va_start(args, num_creatures);

  for(int i = 0; i < num_creatures; i++){
    creature_t *arg = va_arg(args, creature_t *);

    map_add_creature(m, arg);
  }
}

clist_t *map_get_creatures(map_t *m){
  if(m == NULL){
    return NULL;
  }

  return m->creatures;
}

bool teleport_creature(map_t *m, creature_t *c, int x, int y){
  if(m == NULL || c == NULL || x < 0 || y < 0){
    return false;
  }
  
  //TODO
  //Check if tile is blocked

  return false;
}

/* DATA FUNCTIONS */

int *map2display(map_t *m){
  if(m == NULL){
    return NULL;
  }
  
  //Need to allocate all the tiles
  int alloc = m->width * m->height;
  //Plus the null-terminator
  alloc++;
  
  int *to_return = Calloc(alloc, sizeof(int));

  int cur_coord = 0;
  for(int j = 0; j < m->height; j++){
    for(int i = 0; i < m->width; i++){
      to_return[cur_coord] =
        tile_get_display(m->tiles[get_coord_in_arr(i, j, m->width)]);
      
      cur_coord++;
    }
  }

  for(clist_t *cur = m->creatures; cur != NULL; cur = ll_next(cur)){
    creature_t *cc = ll_elem(cur);
    to_return[creature_get_coord(cc, m)] = creature_get_display(cc);
  }
  
  return to_return;
}

int map_get_height(map_t *m){
  if(m == NULL){
    return -1;
  }

  return m->height;
}

int map_get_width(map_t *m){
  if(m == NULL){
    return -1;
  }

  return m->width;
}

/* DIJKSTRA FUNCTIONS */

dijkstra_t *dijkstra_new(map_t *m){
  if(m == NULL){
    return NULL;
  }

  int w = m->width, h = m->height, total = w * h;
  
  dijkstra_t *to_return = Calloc(1, sizeof(dijkstra_t));
  to_return->width = w;
  to_return->height = h;
  to_return->cells = Calloc(total, sizeof(int));

  
  //Any tile that cannot be passed through by solid objects gets marked INT_MIN
  if(m->tiles != NULL){
    for(int i = 0; i < total; i++){
      if(!tile_has_property(m->tiles[i], TPROP_OPEN)){
        to_return->cells[i] = INT_MIN;
      }
    }
  }

  return to_return;
}

void dijkstra_free(dijkstra_t *d){
  if(d == NULL){
    return;
  }

  if(d->cells != NULL){
    free(d->cells);
  }
  free(d);
}

void dijkstra_add_value(dijkstra_t *d, int weight, int x, int y){
  if(x < 0 || y < 0 || x >= d->width || y >= d->height){
    return;
  }

  d->cells[get_coord_in_arr(x, y, d->width)] = weight;
}

void dijkstra_calculate(dijkstra_t *d){
  if(d == NULL || d->calculated){
    return;
  }

  //Need to keep track of where orginal values were
  int num_cells = d->width * d->height;
  int orig[num_cells];
  memcpy(orig, d->cells, sizeof(orig));
  
  bool change_made;
  int num_iterations = 0;
  int max_iterations = 256; //This is completely arbitrary.
  do{
    change_made = false;
    int cur_coord = 0;
    for(int y = 0; y < d->height; y++){
      for(int x = 0; x < d->width; x++){
        //Don't alter original values
        if(orig[cur_coord] != 0){
          continue;
        }
        
        /* Attempt to find nearby values */
        int n = 0, nw = 0, ne = 0, s = 0, sw = 0, se = 0, e = 0, w = 0, h = 0;

        //Grab all the northern values
        if(y > 0){
          n = d->cells[get_coord_in_arr(x, y - 1, d->width)];
          //ne 
          if(x > 0){
            ne = d->cells[get_coord_in_arr(x - 1, y - 1, d->width)];
          }
          //nw
          if(x < (d->width - 1)){
            nw = d->cells[get_coord_in_arr(x + 1, y -1, d->width)];
          }
        }
        //Grab all the southern values
        if(y < (d->height - 1)){
          s = d->cells[get_coord_in_arr(x, y + 1, d->width)];
          //se
          if(x > 0){
            se = d->cells[get_coord_in_arr(x - 1, y + 1, d->width)];
          }
          //sw
          if(x < (d->width - 1)){
            sw = d->cells[get_coord_in_arr(x + 1, y + 1, d->width)];
          }
        }
        //East
        if(x > 0){
          e = d->cells[cur_coord - 1];
        }
        //West
        if(x < (d->width - 1)){
          e = d->cells[cur_coord + 1];
        }
        //Here
        h = d->cells[cur_coord];

        /* Calculate value for this cell */
        int all_values[] = {n, nw, ne, s, sw, se, e, w, h};
        int total_for_this_cell = 0, num_nonzero = 0;
        for(int k = 0; k < 9; k++){
          //Don't spread INT_MIN
          if(all_values[k] != INT_MIN){
            total_for_this_cell += all_values[k];
          }
          if(all_values[k] != 0){
            num_nonzero++;
          }
        }
        if(num_nonzero != 0){
          total_for_this_cell /= num_nonzero;
        }

        //Check if we actually wind up changing this value or not
        if(h != total_for_this_cell){
          d->cells[cur_coord] = total_for_this_cell;
          change_made = true;
        }
        
        cur_coord++;
      }
    }

    num_iterations++;
  }while(!change_made && num_iterations < max_iterations);

  d->calculated = true;
}

int *dijkstra_map(dijkstra_t *d){
  if(d == NULL || d->cells){
    return NULL;
  }

  int num_cells = d->width * d->height;
  int *to_return = Calloc(num_cells, sizeof(int));
  for(int i = 0; i < num_cells; i++){
    to_return[i] = d->cells[i];
  }

  return to_return;
}

/* ITEMS FUNCTIONS */

ilist_t *map_get_items(map_t *m){
  if(m == NULL){
    return NULL;
  }

  return m->items;
}

bool teleport_item(map_t *m, item_t *i, int x, int y){
  if(m == NULL || i == NULL || !xycoord_is_valid(m, x, y)){
    return false;
  }

  if(map_xycoord_is_blocked(m, x, y)){
    return false;
  }

  item_set_index(i, get_coord_in_arr(x, y, m->width));
  
  return true;
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
    property_t arg = va_arg(args, property_t);

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

int map_get_nearest_wall(map_t *m, int x, int y){
  if(m == NULL || !xycoord_is_valid(m, x, y)){
    return -1;
  }

  double closest_found = INT_MAX;
  int closest_index = -1;

  int cur_coord = 0;
  for(int j = 0; j < m->height; j++){
    for(int i = 0; i < m->width; i++){

      if(tile_has_property(m->tiles[cur_coord], TPROP_IS_WALL)){
        int cur_dist = dist(i, j, x, y);
        if(cur_dist < closest_found){
          closest_found = cur_dist;
          closest_index = cur_coord;
        }
      }
      
      cur_coord++;
    }
  }
  
  return closest_index;
}

bool map_coord_is_blocked(map_t *m, int c){
  if(c < 0){
    return false;
  }
  
  return map_xycoord_is_blocked(m, c % m->width, c / m->width);
}

bool map_xycoord_is_blocked(map_t *m, int x, int y){
  if(m == NULL || !xycoord_is_valid(m, x, y)){
    return false;
  }

  if(!tile_has_property(m->tiles[get_coord_in_arr(x, y, m->width)],TPROP_OPEN)){
    return true;
  }

  int cur_coord = get_coord_in_arr(x, y, m->width);
  for(clist_t *cur = m->creatures; cur != NULL; cur = ll_next(cur)){
    creature_t *cur_creature = ll_elem(cur);

    if(cur_creature != NULL && cur_coord == creature_get_coord(cur_creature,m)){
      return true;
    }
  }

  return false;
}

/* MAP GENERATORS */

void new_desert(map_t *base){
  if(base == NULL){
    return;
  }

  base->tiles = Calloc(DEFAULT_TERM_WIDTH * DEFAULT_TERM_HEIGHT, sizeof(int));
  base->height = DEFAULT_TERM_HEIGHT;
  base->width = DEFAULT_TERM_WIDTH;

  int cur_coord = 0;
  for(int j = 0; j < DEFAULT_TERM_HEIGHT; j++){
    for(int i = 0; i < DEFAULT_TERM_WIDTH; i++){
      base->tiles[cur_coord] = TILE_SAND_FLOOR;
      cur_coord++;
    }
  }
  
  add_property(base, PROP_IS_DESERT);
}

void new_main_dungeon(map_t *m, int player_coord){
  if(m == NULL || m->tiles == NULL || !coord_is_valid(m, player_coord)){
    return;
  }

  int num_rooms = rand() % 8 + 3; //min 3 rooms, max 10
  int prev_center = -1;
  
  for(int i = 0; i < num_rooms; i++){
    int x, y, w, h, ur_corner, ll_corner;

    /* If the player coordinate was given, we cannot advance until the first
     * room generated contains the given position.
     */
    do{
      w = (10 - i) * (rand() % 3 + 1) + 3;
      h = rand() % (10 - i) + 3;
      x = rand() % (m->width - w - 3) + 1;
      y = rand() % (m->height - h - 3) + 1;
      ur_corner = get_coord_in_arr(x, y, m->width);
      ll_corner = get_coord_in_arr(x + w, y + h, m->width);
    }while(prev_center < 0 && player_coord > 0
           && (ur_corner > player_coord || ll_corner < player_coord));

    //Draw the room
    map_draw_box(m, get_coord_in_arr(x, y, m->width), w, h, TILE_STONE_FLOOR);
    
    //Draw corridor connection the rooms
    int cur_center = get_coord_in_arr((x + w) / 2, (y + h) / 2, m->width);
    if(prev_center > 0){
      map_draw_line(m, cur_center, prev_center, TILE_STONE_CORRIDOR, true);
    }

    //Draw walls around the rooms
    map_draw_walls(m, TILE_STONE_WALL);
    
    prev_center = cur_center;
  }
    
  add_property(m, PROP_IS_MAIN_DUNGEON);
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

void map_draw_box(map_t *m, int c, int width, int height, tile_t t){
  if(m == NULL || m->tiles == NULL || !coord_is_valid(m, c)){
    return;
  }

  //Extract coordinates
  int x = c % m->width;
  int y = c / m->width;

  for(int j = 0; j < height && j < m->height; j++){
    int coord = (j + y) * m->width + x;
    for(int i = 0; i < width && i < m->width; i++){
      m->tiles[coord] = t;
      coord++;
    }
  }
}

void map_draw_circle(map_t *m, int c, int r, tile_t t){
  if(m == NULL || !coord_is_valid(m, c) || r < 0){
    return;
  }
  
  //Extract coordinates
  int x0 = c % m->width;
  int y0 = c / m->width;
  int x = r + x0;
  int y = y0;
  int err = 0;

  if(x >= m->width){
    return;
  }

  while (x >= y) {
    m->tiles[get_coord_in_arr(x0 + x, y0 + y, m->width)] = t;
    m->tiles[get_coord_in_arr(x0 + y, y0 + x, m->width)] = t;
    m->tiles[get_coord_in_arr(x0 - y, y0 + x, m->width)] = t;
    m->tiles[get_coord_in_arr(x0 - x, y0 + y, m->width)] = t;
    m->tiles[get_coord_in_arr(x0 - x, y0 - y, m->width)] = t;
    m->tiles[get_coord_in_arr(x0 - y, y0 - x, m->width)] = t;
    m->tiles[get_coord_in_arr(x0 + y, y0 - x, m->width)] = t;
    m->tiles[get_coord_in_arr(x0 + x, y0 - y, m->width)] = t;
    
    y += 1;
    if(err <= 0){
      err += 2*y + 1;
    }
    if (err > 0){
      x -= 1;
      err -= 2*x + 1;
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

  //c1 should be on the left
  if(x1 > x2){
    int tmp = x1;  x1 = x2;  x2 = tmp;
        tmp = y1;  y1 = y2;  y2 = tmp;
  }
  
  float deltax = x2 - x1;
  float deltay = (y2 < y1) ? y1 - y2 : y2 - y1;
  float deltaerr = (deltax == 0) ? 1 : fabs(deltay / deltax);
  float error = deltaerr - 0.5;
  
  while( x1 < x2){
    int coord = get_coord_in_arr(x1, y1, m->width);
    if(!only_unknown || m->tiles[coord] != TILE_UNKNOWN){
      m->tiles[coord] = t;
    }
    
    error += deltaerr;
    if(error > 0.5 || error < -0.5){
      if(y1 < y2){
        y1++;
        error--;
      }
      else{
        y1--;
        error++;
      }
      x1--;
    }
    
    x1++;
  }
}

void map_draw_walls(map_t *m, tile_t t){
  if(m == NULL || m->tiles == NULL){
    return;
  }

  for(int j = 0; j < m->height; j++){
    int coord = get_coord_in_arr(j, 0, m->width) - 1;
    for(int i = 0; i < m->width; i++){
      coord++;
      if(m->tiles[coord] != TILE_UNKNOWN){
        continue;
      }

      if(get_tile_at(m,    i + 1, j    ) == TILE_STONE_FLOOR
         || get_tile_at(m, i + 1, j + 1) == TILE_STONE_FLOOR
         || get_tile_at(m, i + 1, j - 1) == TILE_STONE_FLOOR
         || get_tile_at(m, i    , j + 1) == TILE_STONE_FLOOR
         || get_tile_at(m, i    , j - 1) == TILE_STONE_FLOOR
         || get_tile_at(m, i - 1, j + 1) == TILE_STONE_FLOOR
         || get_tile_at(m, i - 1, j    ) == TILE_STONE_FLOOR
         || get_tile_at(m, i - 1, j - 1) == TILE_STONE_FLOOR){
        m->tiles[j*m->width+i] = t;
      }
    }
  }
}
