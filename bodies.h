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
 * Initializes all internal bodies data. MUST be called before creating any new
 * bodies.
 */
void bodies_init();

/**
 * Returns a new body represented by the given species.
 */
body_t *new_body_by_species(species_t s);

/**
 * Handles freeing an allocated body.
 * @param A body.
 */
void body_free(body_t *b);

#endif
