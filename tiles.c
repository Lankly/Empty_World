#include <curses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "tiles.h"
#include "colors.h"
#include "trees.h"

/*********
 * TYPES *
 *********/
BTREE(property_tree_t)

typedef struct {
  int display;
  int display_color;
  int display_color_alt;
  int hardness;                 /* Scale of 0 to 100*/
  char* exam_text;
  property_tree_t *properties;
} tile_data_t;


/*********************
 * PRIVATE VARIABLES *
 *********************/

tile_data_t tile_data[TILE_MAX + 1];


/******************************
 * HELPER FUNCTION PROTOTYPES *
 ******************************/

/**
 * Adds a given property to a given tile. Assumes that the tile data has already
 * been initialized.
 * @param t Any tile other than TILE_MAX.
 * @param p Any tile property.
 */
void tile_add_property(tile_t t, tile_property_t p);

/**
 * Adds many properties to a given tile at once.
 * @param t Any tile other than TILE_MAX.
 * @param num_properties The number of properties that follow.
 */
void tile_add_properties(tile_t t, int num_properties, ...);

/************************
 * FUNCTION DEFINITIONS *
 ************************/

void tile_data_init(){
  //TILE_UNKNOWN
  tile_data[TILE_UNKNOWN] = (tile_data_t){
    .display = ' ',
    .exam_text = "You do not know what this is",
  };

  /* DIRT TILES */

  tile_data[TILE_DIRT_FLOOR] = (tile_data_t){
    .display = ',',
    .exam_text = "It is a loose dirt floor.\n",
    .hardness = 10
  };
  tile_add_properties(TILE_DIRT_FLOOR, 3, TPROP_OPEN, TPROP_TRANSPARENT);

  /* SAND TILES */
  
  tile_data[TILE_SAND_FLOOR] = (tile_data_t){
    .display = '.',
    .exam_text = "It is loose sand.\n",
    .hardness = 5
  };
  tile_add_properties(TILE_SAND_FLOOR, 2, TPROP_OPEN, TPROP_TRANSPARENT);
  
  tile_data[TILE_SAND_FLOOR_ALT] = (tile_data_t){
    .display = ',',
    .exam_text = tile_data[TILE_SAND_FLOOR].exam_text,
    .hardness = tile_data[TILE_SAND_FLOOR].hardness
  };
  tile_add_properties(TILE_SAND_FLOOR, 2, TPROP_OPEN, TPROP_TRANSPARENT);

  tile_data[TILE_SAND_HILL] = (tile_data_t){
    .display = '*',
    .exam_text = "It is a sandy hill.\n",
    .hardness = 6
  };
  tile_add_property(TILE_SAND_HILL, TPROP_IS_WALL);
  
  /* STONE TILES */
  
  tile_data[TILE_STONE_FLOOR] = (tile_data_t){
    .display = ACS_BULLET,
    .exam_text = "It is a hard stone floor.\n",
    .hardness = 50
  };
  tile_add_properties(TILE_STONE_FLOOR, 2, TPROP_OPEN, TPROP_TRANSPARENT);
  
  tile_data[TILE_STONE_WALL] = (tile_data_t){
    .display = ACS_PLUS,
    .exam_text = "It is a hard stone wall.\n",
    .hardness = 3
  };
  tile_add_property(TILE_STONE_WALL, TPROP_IS_WALL);
  
  tile_data[TILE_STONE_CORRIDOR] = (tile_data_t){
     .display = ACS_CKBOARD,
    .display_color = CP_DARK_GREY_BLACK,
    .display_color_alt = CP_YELLOW_BLACK,
    .exam_text = 
    "It is a hard stone corridor.\n",
    .hardness = 3,
   };
  tile_add_properties(TILE_STONE_CORRIDOR, 4,
                      TPROP_OPEN, TPROP_STOPME, TPROP_TRANSPARENT,
                      TPROP_IS_CORRIDOR);
}

/* PROPERTIES FUNCTIONS */

bool tile_has_property(tile_t t, tile_property_t p){
  return tree_search(tile_data[t].properties, &p, int_cmp) != NULL;
}


/*******************************
 * HELPER FUNCTION DEFINITIONS *
 *******************************/

void tile_add_property(tile_t t, tile_property_t p){
  tile_data[t].properties = tree_insert(tile_data[t].properties, &p, &int_cmp);
}

void tile_add_properties(tile_t t, int num_properties, ...){
  if(num_properties < 1){
    return;
  }

  va_list args;
  va_start(args, num_properties);

  for(int i = 0; i < num_properties; i++){
    tile_property_t arg = va_arg(args, tile_property_t);

    tile_add_property(t, arg);
  }
  
  va_end(args);
}
