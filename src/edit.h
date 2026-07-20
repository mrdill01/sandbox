#ifndef EDIT_H
#define EDIT_H

#include "math.h"
#include "entity.h"
#include "physics.h"

#include <stdbool.h>

typedef struct sbox_t sbox_t;

typedef struct {
    entity_t* selection;
    trace_result_t selection_trace;
    vec3 selection_position;
} editor_t;

void edit_init(sbox_t* sbox, editor_t* editor);
void edit_tick(sbox_t* sbox, editor_t* editor);

#endif
