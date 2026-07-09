#ifndef ENT_H
#define ENT_H

#include "math.h"
#include "render.h"

typedef struct {
    const char* name;
    vec3 position;
    quat rotation;

    mesh_t* mesh;
    size_t nmaterials;
    material_t* materials[MAX_MATERIALS];

    float dist_to_camera;
    bool is_viewmodel;
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
entity_t* entlist_find_by_name(sbox_t* sbox, entlist_t* entlist, const char* name);

#endif
