#ifndef ENT_H
#define ENT_H

#include "math.h"
#include "render.h"

typedef enum {
    ENT_PROP,
    ENT_LIGHT,
} entity_type_t;

typedef struct {
    mesh_t* mesh;
    size_t nmaterials;
    material_t* materials[MAX_MATERIALS];
    bool is_viewmodel;
} entity_prop_t;

typedef struct {
    vec3 color;
} entity_light_t;

typedef struct {
    const char* name;
    entity_type_t type;
    vec3 position;
    quat rotation;

    union {
        entity_prop_t prop;
        entity_light_t light;
    } data;
} entity_t;

typedef struct {
    size_t len;
    entity_t** ents;
} entlist_t;

void entity_init_prop(sbox_t* sbox,
    const char* name, float x, float y, float z, entity_t** out);
void entity_init_light(sbox_t* sbox,
    const char* name, float x, float y, float z, vec3 color, entity_t** out);
void entity_free(sbox_t* sbox, entity_t* entity);

void entlist_init(sbox_t* sbox, entlist_t* entlist);
void entlist_free(sbox_t* sbox, entlist_t* entlist);
void entlist_tick(sbox_t* sbox, entlist_t* entlist);
void entlist_add(sbox_t* sbox, entlist_t* entlist, entity_t* entity);
entity_t* entlist_find_by_name(sbox_t* sbox, entlist_t* entlist, const char* name);

#endif
