#ifndef ENT_H
#define ENT_H

#include "video.h"

#include "../include/cglm/cglm.h"

#define MAX_MATERIALS 16

typedef struct {
    const char* name;
    vec3 position;

    mesh_t* mesh;
    size_t nmaterials;
    material_t* materials[MAX_MATERIALS];

    float dist_to_camera;
} ent_t;

typedef struct {
    ent_t* ents;
    size_t len;
} entlist_t;

#endif
