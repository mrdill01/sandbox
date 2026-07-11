#ifndef PHYSICS_H
#define PHYSICS_H

#include "math.h"
#include "entity.h"

#define PHYS_TRACE_STEP 0.005

typedef enum phys_material_t {
    PHYSMAT_NONE,
    PHYSMAT_METAL,
    PHYSMAT_WOOD,
    PHYSMAT_STONE,
    PHYSMAT_VEGETATION,
    PHYSMAT_WATER,
    PHYSMAT_MAX,
} phys_material_t;

typedef struct {
    vec3 point;
    float distance;
    float water_level;
    phys_material_t phys_mat;
} trace_result_t;

bool phys_line_trace(
    vec3 start, vec3 dir, double max_distance, entlist_t* entlist, trace_result_t* out);

#endif
