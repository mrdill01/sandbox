#ifndef EDITOR_H
#define EDITOR_H

#include "../shared/mathlib.h"
#include "../shared/entity.h"
#include "../shared/physics.h"

#include <stdbool.h>

typedef struct quark_t quark_t;
typedef struct player_t player_t;

typedef struct {
    entity_t* selection;
    trace_result_t trace;
} editor_t;

void edit_init(quark_t* quark, editor_t* editor);
void edit_tick(quark_t* quark, editor_t* editor, player_t* player);

void edit_select(quark_t* quark, editor_t* editor, entity_t* entity);
void edit_deselect(quark_t* quark, editor_t* editor);

#endif
