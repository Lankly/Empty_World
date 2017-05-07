#include <stdlib.h>
#include "bodies.h"
#include "lists.h"

/***************
 * DEFINITIONS *
 ***************/


/*********
 * TYPES *
 *********/

LINKED_LIST(bodylist_t)
LINKED_LIST(afflictionlist_t)

typedef enum {
  AFFL_BLEEDING,
  AFFL_BROKEN,
  AFFL_BRUISED,
  AFFL_BURNT,
  AFFL_FLAMING,
  AFFL_FROZEN,
  AFFL_ROTTING,
  AFFL_MAX,
} affliction_t;

typedef enum {
  PART_ANTENNA,
  PART_ARM,
  PART_CHEST,
  PART_EYE,
  PART_FINGER,
  PART_FOOT,
  PART_HAND,
  PART_HEAD,
  PART_HORN,
  PART_LEG,
  PART_STOMACH,
  PART_TAIL,
  PART_TORSO,
  PART_THUMB,
  PART_WHOLE_BODY,
  PART_WING,
  PART_MAX
} part_t;

typedef enum {
  IMAGE_HUMAN_ARM,     IMAGE_HUMAN_CHEST,  IMAGE_HUMAN_CHEST_W_BREASTS,
  IMAGE_HUMAN_EYE,     IMAGE_HUMAN_FINGER, IMAGE_HUMAN_FOOT,
  IMAGE_HUMAN_HAND,    IMAGE_HUMAN_HEAD,   IMAGE_HUMAN_LEG,
  IMAGE_HUMAN_STOMACH, IMAGE_HUMAN_TORSO,  IMAGE_HUMAN_THUMB,
    
  IMAGE_RAT_HEAD1, IMAGE_RAT_BODY1, IMAGE_RAT_LEGS1, IMAGE_RAT_TAIL1,
  IMAGE_RAT_HEAD2, IMAGE_RAT_BODY2, IMAGE_RAT_LEGS2, IMAGE_RAT_TAIL2,
  IMAGE_RAT_HEAD3, IMAGE_RAT_BODY3, IMAGE_RAT_LEGS3, IMAGE_RAT_TAIL3,

  IMAGE_MAX
} image_t;

struct body_t {
  char *name;
  char *image;
  int image_x, image_y;  /* Used when placing part in overall image */
  int health;
  afflictionlist_t afflictions;
  bodylist_t subparts;
};


/*********************
 * PRIVATE VARIABLES *
 *********************/
body_t templates[PART_MAX];
const char *images[IMAGE_MAX];


/****************************
 * FUNCTION IMPLEMENTATIONS *
 ****************************/

void bodies_init(){
  templates[PART_ANTENNA] = (body_t){
    .name = "Antenna"
  };
  templates[PART_ARM] = (body_t){
    .name = "Arm"
  };
  templates[PART_CHEST] = (body_t){
    .name = "Chest"
  };
  templates[PART_EYE] = (body_t){
    .name = "Eye"
  };
  templates[PART_FINGER] = (body_t){
    .name = "Finger"
  };
  templates[PART_FOOT] = (body_t){
    .name = "Foot"
  };
  templates[PART_HAND] = (body_t){
    .name = "Hand"
  };
  templates[PART_HEAD] = (body_t){
    .name = "Head"
  };
  templates[PART_HORN] = (body_t){
    .name = "Horn"
  };
  templates[PART_LEG] = (body_t){
    .name = "Leg"
  };
  templates[PART_STOMACH] = (body_t){
    .name = "Stomach"
  };
  templates[PART_TORSO] = (body_t){
    .name = "Torso"
  };
  templates[PART_THUMB] = (body_t){
    .name = "Thumb"
  };
  templates[PART_WHOLE_BODY] = (body_t){
    .name = "Body"
  };
  templates[PART_WING] = (body_t){
    .name = "Wing"
  };

  images[IMAGE_RAT_HEAD1] =
    " (\\,/)\n"
    "  oo\n"
    ",/_;~,\n"
    "\"'";
  images[IMAGE_RAT_HEAD2] = images[IMAGE_RAT_HEAD1];
  images[IMAGE_RAT_HEAD3] =
    "(\\,;,/)\n"
    " (o o)\\\n"
    "  \\ /\n"
    "  `+'";
  images[IMAGE_RAT_BODY1] =
    "'''//,\n"
    "       \\,\n"
    "        \\";
  images[IMAGE_RAT_BODY2] =
    "\\//,\n"
    "    \\,\n"
    "      \\";
  images[IMAGE_RAT_BODY3] =
    "\\//,\n"
    "     \\,\n"
    "       \\";
  images[IMAGE_RAT_LEGS1] =
    "\\    (\n"
    "|,|  \\    |'\n"
    "'~  '~----'";
  images[IMAGE_RAT_LEGS2] =
    "  (  (\n"
    " //  \\   |\n"
    "'~  '~----'";
  images[IMAGE_RAT_LEGS3] =
    "   (  ("
    "  //  \\   |_./\n"
    "'~' '~----'";
  images[IMAGE_RAT_TAIL1] =
    "    _\n"
    "   /\n"
    "   !\n"
    "__.\\'\n"
    "'";
  images[IMAGE_RAT_TAIL2] =
    "    _\n"
    "   / '\n"
    "   !\n"
    "__.\\'\n"
    "'";
  images[IMAGE_RAT_TAIL3] =
    "   )\n"
    "_./\n"
    "\"";
}
