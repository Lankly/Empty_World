#include <curses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "tiles.h"
#include "colors.h"
#include "display.h"
#include "trees.h"

/*********
 * TYPES *
 *********/

BTREE(property_tree_t)

typedef int (*get_display_func)(int, int);

typedef struct {
  int display;
  int display_color;
  int display_color_alt;
  int hardness;                 /* Scale of 0 to 100*/
  char* exam_text;
  property_tree_t *properties;
  get_display_func disp_func;  /* Should pass in x, y, current time */
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

/**
 * Returns the display character for the given tile, and its correct color
 * scheme.
 * @param t Any tile other than TILE_MAX
 * @returns An integer printable by ncurses. 
 */
int tile_get_default_display(tile_t t);

/* Tile animation functions */

int get_sand_floor_display(int x, int y);


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
    .hardness = 5,
    .disp_func = get_sand_floor_display
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

/* DATA FUNCTIONS */

int tile_get_display(tile_t t, int x, int y){
  if( t == TILE_MAX){
    return 0;
  }

  if(tile_data[t].disp_func != NULL){
    return (tile_data[t].disp_func)(x, y);
  }
  
  return tile_get_default_display(t);
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

int tile_get_default_display(tile_t t){
  if(t == TILE_MAX){
    return 0;
  }

  int display_color;
  if(compatibility_mode_on()){
    display_color = tile_data[t].display_color_alt;
  }
  else{
    display_color = tile_data[t].display_color;
  }
  
  return tile_data[t].display | COLOR_PAIR(display_color);
}

/* Tile animation functions */

int get_sand_floor_display(int x, int y){
  /* Constants */
  int primary_width = get_pane_width(PANE_PRIMARY);
  int primary_height = get_pane_height(PANE_PRIMARY);
  int num_waves = 4;
  int wave_width = primary_width / num_waves;
  int head_radius_min = 1;
  int head_radius_max = 4;
  int wave_head = '~';
  int standing_sand = '.';
  
  /* Calculated */
  int cur_time = time(NULL);
  int cur_column = x / wave_width;
  int cur_wave_id = cur_time / wave_width - cur_column;
  unsigned int cur_wave_hash = hash(cur_wave_id);
  int wave_head_y = cur_wave_hash % primary_height ;
  int head_radius = MAX(head_radius_min, cur_wave_hash % (head_radius_max + 1));
  int wave_head_top = wave_head_y - head_radius;
  int wave_head_bottom = wave_head_y + head_radius;
  int column_index = x % wave_width;

  //Return anything in the wave head
  if(column_index == cur_time % wave_width
     && (y >= wave_head_top && y <= wave_head_bottom)){
    return wave_head;
  }
  
  return standing_sand;
}
