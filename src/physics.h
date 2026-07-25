#ifndef PHYSICS_H
#define PHYSICS_H

#include "math.h"
#include "entity.h"

#define PHYS_GRAVITY 9.81f
#define PHYS_TRACE_STEP 0.01f

typedef enum phys_material_t {
    PHYS_MAT_NONE,
    PHYS_MAT_METAL,
    PHYS_MAT_WOOD,
    PHYS_MAT_STONE,
    PHYS_MAT_SAND,
    PHYS_MAT_GRASS,
    PHYS_MAT_WATER,
    PHYS_MAT_MAX,
} phys_material_t;

typedef struct {
    vec3 point;
    vec3 normal;
    float distance;
    float water_level;
    entity_t* entity;
    material_t* material;
    phys_material_t phys_mat;
} trace_result_t;

bool phys_line_trace(
    vec3 start, vec3 dir, double max_distance, entlist_t* entlist, trace_result_t* out);

#endif
