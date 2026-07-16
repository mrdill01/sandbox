#ifndef ENTITY_H
#define ENTITY_H

#include "math.h"
#include "render.h"

#define PICKUP_SPIN_RATE 24.0f

typedef enum {
    ENTITY_PROP,
    ENTITY_DIR_LIGHT,
    ENTITY_POINT_LIGHT,
} entity_type_t;

typedef struct {
    mesh_t* mesh;
    material_t* materials[MAX_MATERIALS];
    bool is_visible;
    bool is_viewmodel;
    bool is_pickup;
    bool collision_enabled;
} entity_prop_t;

typedef struct {
    vec3 color;
    vec3 direction;
} entity_sun_light_t;

typedef struct {
    vec3 color;
} entity_point_light_t;

typedef struct {
    const char* name;
    entity_type_t type;
    vec3 position;
    quat rotation;
    vec3 scale;

    union {
        entity_prop_t prop;
        entity_sun_light_t sun_light;
        entity_point_light_t point_light;
    } data;
} entity_t;

typedef struct entlist_t {
    size_t len;
    entity_t** ents;
} entlist_t;

void entity_init_prop(sbox_t* sbox,
    const char* name, float x, float y, float z, mesh_t* mesh, entity_t** out);
void entity_init_sun_light(sbox_t* sbox,
    const char* name,
    float x, float y, float z,
    vec3 dir, vec3 color, entity_t** out);
void entity_init_point_light(sbox_t* sbox,
    const char* name, float x, float y, float z, vec3 color, entity_t** out);
void entity_free(sbox_t* sbox, entity_t* entity);

void entity_prop_set_material(sbox_t* sbox,
    entity_t* entity, material_t* material, int slot);

void entlist_init(sbox_t* sbox, entlist_t* entlist);
void entlist_free(sbox_t* sbox, entlist_t* entlist);
void entlist_tick(sbox_t* sbox, entlist_t* entlist);
void entlist_add(sbox_t* sbox, entlist_t* entlist, entity_t* entity);
entity_t* entlist_find_by_name(sbox_t* sbox, entlist_t* entlist, const char* name);

#endif
