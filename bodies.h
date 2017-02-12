#ifndef BODIES_H
#define BODIES_H

#include "creatures.h"

/****************
 * USEFUL TYPES *
 ****************/
typedef struct body_t body_t;


/***********************
 * FUNCTION PROTOTYPES *
 ***********************/

/**
 * Returns a new body represented by the given species.
 */
body_t *new_body_by_species(species_t s);

#endif
