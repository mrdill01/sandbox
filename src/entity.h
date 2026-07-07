#ifndef ENT_H
#define ENT_H

#include "math.h"
#include "video.h"

#define MAX_MATERIALS 4

typedef struct {
    const char* name;
    vec3 position;
    quat rotation;

    mesh_t* mesh;
    size_t nmaterials;
    material_t* materials[MAX_MATERIALS];

    float dist_to_camera;
} entity_t;

typedef struct {
    size_t len;
    entity_t** ents;
} entlist_t;

void entity_init(sbox_t* sbox, const char* name, float x, float y, float z, entity_t** out);
void entity_free(sbox_t* sbox, entity_t* entity);

void entlist_init(sbox_t* sbox, entlist_t* entlist);
void entlist_free(sbox_t* sbox, entlist_t* entlist);
void entlist_tick(sbox_t* sbox, entlist_t* entlist);
void entlist_add(sbox_t* sbox, entlist_t* entlist, entity_t* entity);

#endif
