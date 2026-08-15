#ifndef PHYSICS_H
#define PHYSICS_H

#include "mathlib.h"
#include "entity.h"

#define PHYS_GRAVITY 9.81f
#define PHYS_REST_VELOCITY 0.5f
#define PHYS_MAX_IGNORE_ENTITES 1

typedef enum phys_material_t {
    PHYS_MAT_NONE,
    PHYS_MAT_METAL,
    PHYS_MAT_WOOD,
    PHYS_MAT_STONE,
    PHYS_MAT_SAND,
    PHYS_MAT_GRASS,
    PHYS_MAT_WATER,
    PHYS_MAT_PLAYER,
    NUM_PHYS_MAT,
} phys_material_t;

typedef struct trace_result_t {
    vec3 point;
    vec3 normal;
    float distance;
    bool starts_solid;
    vec3 enter_water_point;
    bool starts_in_water;
    float water_level;
    entity_t* entity;
    int player_id;
    material_t* material;
    phys_material_t phys_mat;
} trace_result_t;

bool phys_line_trace(
    quark_t* quark,
    ray_t ray,
    float max_distance,
    entlist_t* entlist,
    int ignore_player_id,
    int ignore_entities[PHYS_MAX_IGNORE_ENTITES],
    size_t nignore_entities,
    trace_result_t* out);

#endif
