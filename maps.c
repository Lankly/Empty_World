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
    map_draw_box(m, get_coord_in_arr(x, y, m->width), w, h, TILE_FLOOR);
    
    //Draw corridor connection the rooms
    int cur_center = get_coord_in_arr((x + w) / 2, (y + h) / 2, m->width);
    if(prev_center > 0){
      map_draw_line(m, cur_center, prev_center, TILE_CORRIDOR, true);
    }

    //Draw walls around the rooms
    map_draw_walls(m, TILE_WALL);
    
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
  float deltaerr = (deltax == 0) ? 1 : abs(deltay / deltax);
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

      if(get_tile_at(m,    i + 1, j    ) == TILE_FLOOR
         || get_tile_at(m, i + 1, j + 1) == TILE_FLOOR
         || get_tile_at(m, i + 1, j - 1) == TILE_FLOOR
         || get_tile_at(m, i    , j + 1) == TILE_FLOOR
         || get_tile_at(m, i    , j - 1) == TILE_FLOOR
         || get_tile_at(m, i - 1, j + 1) == TILE_FLOOR
         || get_tile_at(m, i - 1, j    ) == TILE_FLOOR
         || get_tile_at(m, i - 1, j - 1) == TILE_FLOOR){
        m->tiles[j*m->width+i] = t;
      }
    }
  }
}
