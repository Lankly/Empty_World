#ifndef MAPS_H
#define MAPS_H

/*********
 * TYPES *
 *********/

typedef struct map_t map_t;
typedef struct dijkstra_t dijkstra_t;

typedef enum {
  PROP_IS_ABOVEGROUND,
  PROP_IS_DARK,
  PROP_IS_DESERT,
  PROP_IS_MAIN_DUNGEON,
  PROP_IS_UNDERGROUND,
} property_t;

typedef void (*map_generator)(map_t *);


/************
 * INCLUDES *
 ************/

#include <stdbool.h>
#include "creatures.h"
#include "tiles.h"


/***********************
 * FUNCTION PROTOTYPES *
 ***********************/

/**
 * Returns a new map with terrain generated by the given function pointer. A
 * corresponding property will be added to the map.
 * @param ptr A function that will generate the terrain on the map passed to it.
 * @returns A non-null map.
 */
map_t *map_new(map_generator ptr);

/**
 * Returns the creatures currently on the map.
 * @param m A valid map.
 * @returns A creature list containing every creature on the given map.
 */
clist_t *get_creatures(map_t *m);

/* PROPERTIES FUNCTIONS */

/**
 * Determines whether or not the given map has the given property.
 * A null map always returns false.
 * @param m A valid map.
 * @param p A map property.
 * @returns True if the given map has the given property.
 */
bool has_property(map_t *m, property_t p);
/**
 * Adds a property to the given map.
 * @param m A valid map.
 * @param p A map property.
 */
void add_property(map_t *m, property_t p);
/**
 * Adds any number of properties to the given map.
 * @param m A valid map.
 * @param num_properties The number of properties passed to the function.
 */
void add_properties(map_t *m, int num_properties, ...);
/**
 * Removes a property from the given map.
 * @param m A valid map.
 * @param p A map property.
 */
void remove_property(map_t *m, property_t p);
/**
 * Removes any number of properties from the given map.
 * @param m A valid map.
 * @param num_properties The number of properties passed to the function.
 */
void remove_properties(map_t *m, int num_properties, ...);

/* TILE FUNCTIONS */

/**
 * Gets the tile at the specified coordinate.
 * @param m A valid map.
 * @param x The x-coordinate of the desired tile on the map.
 * @param y The y-coordinate of the desired tile on the map.
 * @returns The tile at the coordinate.
 */
tile_t get_tile_at(map_t *m, int x, int y);
/**
 * Sets the tile at the specified coordinate.
 * @param m A valid map.
 * @param x The x-coordinate of the tile to set on the map.
 * @param y The y-coordinate of the tile to set on the map.
 * @param new The new tile to place on the map.
 */
void set_tile_at(map_t *m, int x, int y, tile_t new);

/* MAP GENERATORS */

/**
 * Generates a desert-like terrain on the given map.
 * @param base The map to generate the terrain on.
 */
void new_desert(map_t *base);
/**
 * Generates a basic dungeon on the given map.
 * @param base The map to generate the dungeon on.
 */
void new_main_dungeon(map_t *base);

#endif
