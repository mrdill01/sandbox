#ifndef ENTITY_H
#define ENTITY_H

#include "math.h"
#include "render.h"

#include <stddef.h>

#define ITEM_SPIN_RATE 24.0f

typedef enum {
    ENTITY_MESH,
    ENTITY_PROJECTILE,
    ENTITY_VEHICLE,
    ENTITY_EXPLOSION,
    ENTITY_DROPPED_ITEM,
    ENTITY_SUN_LIGHT,
    ENTITY_POINT_LIGHT,
} entity_type_t;

typedef struct {
    mesh_t* mesh;
    material_t* materials[MAX_MATERIALS];
    bool is_visible;
    bool is_viewmodel;
    bool is_pickup;
    bool enable_collision;
} entity_mesh_t;

typedef struct {
    int owner_id;
    mesh_t* mesh;
    material_t* materials[MAX_MATERIALS];
    vec3 start;
    vec3 velocity;
    float damage;
    float last_particle;
} entity_projectile_t;

typedef struct {
    mesh_t* mesh;
    material_t* materials[MAX_MATERIALS];
} entity_vehicle_t;

typedef struct {
    float radius;
    float damage;
} entity_explosion_t;

typedef struct {
    mesh_t* mesh;
    material_t* materials[MAX_MATERIALS];
} entity_dropped_item_t;

typedef struct {
    vec3 direction;
    vec3 color;
    mat4 matrix;
} entity_sun_light_t;

typedef struct {
    vec3 color;
} entity_point_light_t;

typedef struct {
    int id;
    char* name;
    entity_type_t type;
    vec3 position;
    quat rotation;
    vec3 scale;
    bbox_t local_bbox;
    bbox_t world_bbox;
    float spawn_time;

    union {
        entity_mesh_t prop;
        entity_projectile_t projectile;
        entity_vehicle_t vehicle;
        entity_explosion_t explosion;
        entity_dropped_item_t dropped_item;
        entity_sun_light_t sun_light;
        entity_point_light_t point_light;
    } data;
} entity_t;

typedef struct entlist_t {
    size_t len;
    entity_t** ents;
} entlist_t;

void entity_init_common(
	sbox_t* sbox, const char* name, entity_type_t type, vec3 position, entity_t** out);
void entity_init_prop(sbox_t* sbox,
    const char* name, float x, float y, float z, mesh_t* mesh, entity_t** out);
void entity_init_projectile(sbox_t* sbox,
    const char* name,
    vec3 position,
    int owner_id,
    mesh_t* mesh,
    vec3 velocity,
    float damage,
    entity_t** out);
void entity_init_vehicle(sbox_t* sbox,
    const char* name, float x, float y, float z, mesh_t* mesh, entity_t** out);
void entity_init_explosion(sbox_t* sbox,
    const char* name, vec3 position, float radius, entity_t** out);
void entity_init_sun_light(sbox_t* sbox,
    const char* name,
    float x, float y, float z,
    vec3 dir, vec3 color, entity_t** out);
void entity_init_point_light(sbox_t* sbox,
    const char* name, float x, float y, float z, vec3 color, entity_t** out);
void entity_free(sbox_t* sbox, entity_t* entity);

void entity_tick_projectile(sbox_t* sbox, entity_t* entity, entity_projectile_t* projectile);
void entity_tick_explosion(sbox_t* sbox, entity_t* entity, entity_explosion_t* explosion);

mesh_t* entity_get_mesh(sbox_t* sbox, entity_t* entity);
void entity_get_materials(sbox_t* sbox, entity_t* entity, material_t** materials, size_t* nmaterials);
bool entity_get_drawcall(sbox_t* sbox, entity_t* entity, drawcall_t* drawcall);

void entity_prop_set_material(sbox_t* sbox,
    entity_t* entity, material_t* material, int slot);

void entlist_init(sbox_t* sbox, entlist_t* entlist);
void entlist_free(sbox_t* sbox, entlist_t* entlist);
void entlist_tick(sbox_t* sbox, entlist_t* entlist);
void entlist_add(sbox_t* sbox, entlist_t* entlist, entity_t* entity);
void entlist_remove(sbox_t* sbox, entlist_t* entlist, entity_t* entity);
entity_t* entlist_find_by_name(sbox_t* sbox, entlist_t* entlist, const char* name);

#endif
