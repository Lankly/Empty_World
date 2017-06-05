#include <stdbool.h>

#ifndef TILES_H
#define TILES_H

/****************
 * USEFUL TYPES *
 ****************/

typedef enum {
  TILE_UNKNOWN,
  /* DIRT TILES*/
  TILE_DIRT_FLOOR,
  TILE_DIRT_FLOOR_ALT,
  
  /* SAND TILES */
  TILE_SAND_FLOOR,
  TILE_SAND_FLOOR_ALT,
  TILE_SAND_HILL,
  
  /* STONE TILES*/
  TILE_STONE_FLOOR,
  TILE_STONE_WALL,
  TILE_STONE_CORRIDOR,
  TILE_MAX
} tile_t;

typedef enum {
  TPROP_DEEP,        /* Tile has some kind of deep hole in it. */
  TPROP_OPEN,        /* Tile can be passed through by solid beings. */
  TPROP_STOPME,      /* Tile stops automove. */
  TPROP_TRANSPARENT, /* Tile can be easily seen through. Default is Opaque. */
  TPROP_TRANSLUCENT, /* Tile can be only partially seen through. */

  /* TILE TYPES */
  TPROP_IS_CORRIDOR,
  TPROP_IS_FLOOR,
  TPROP_IS_WALL,
} tile_property_t;


/***********************
 * FUNCTION PROTOTYPES *
 ***********************/

void tile_data_init();

/* DATA FUNCTIONS */

/**
 * Returns the display character associated with the given tile as an integer
 * with NCURSES color information.
 * @param t A tile other than TILE_MAX.
 * @returns An integer printable by NCURSES.
 */
int tile_get_display(tile_t t);

/* PROPERTIES FUNCTIONS */

bool tile_has_property(tile_t t, tile_property_t p);

#endif
