#ifndef ENTITY_H
#define ENTITY_H

#include "math.h"
#include "render.h"

#include <stddef.h>

#define PICKUP_SPIN_RATE 24.0f
#define PICKUP_SUCK_IN_DISTANCE 10.0f
#define PICKUP_COLLECT_DISTANCE 2.0f
#define PICKUP_SUCK_IN_SPEED 1.0f

typedef enum {
    ENTITY_MESH,
    ENTITY_PROJECTILE,
    ENTITY_EXPLOSION,
    ENTITY_PICKUP,
    ENTITY_VEHICLE,
    ENTITY_SUN_LIGHT,
    ENTITY_POINT_LIGHT,
} entity_type_t;

typedef struct {
    mesh_t* mesh;
    material_t* materials[MAX_MATERIALS];
    bool is_visible;
    bool is_pickup;
    bool enable_collision;
} entity_mesh_t;

typedef struct {
    int owner_id;
    mesh_t* mesh;
    material_t* materials[MAX_MATERIALS];
    vec3 start;
    vec3 velocity;
    float speed;
    bool gravity;
    float damage;
    bool particles;
    float last_particle;
} entity_projectile_t;

typedef struct {
    float radius;
    vec3 direction;
    float damage;
    float min_force;
    float max_force;
} entity_explosion_t;

typedef struct {
    mesh_t* mesh;
    material_t* materials[MAX_MATERIALS];
    void* pickup_sound;
    bool is_coin;
} entity_pickup_t;

typedef enum {
    VEHICLE_CAR,
    VEHICLE_HELICOPTER,
} vehicle_type_t;

typedef struct {

} veh_helicopter;

typedef struct {
    mesh_t* mesh;
    material_t* materials[MAX_MATERIALS];
    vehicle_type_t type;
    union {
        veh_helicopter heli;
    } data;
} entity_vehicle_t;

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
    vec3 velocity;
    bbox_t local_bbox;
    bbox_t world_bbox;
    float spawn_time;

    union {
        entity_mesh_t mesh;
        entity_projectile_t projectile;
        entity_explosion_t explosion;
        entity_pickup_t pickup;
        entity_vehicle_t vehicle;
        entity_sun_light_t sun_light;
        entity_point_light_t point_light;
    } data;
} entity_t;

typedef struct entlist_t {
    size_t len;
    entity_t** ents;
} entlist_t;

void entity_init_common(
	quark_t* quark, const char* name, entity_type_t type, vec3 position, entity_t** out);
void entity_init_mesh(quark_t* quark,
    const char* name, float x, float y, float z, mesh_t* mesh, entity_t** out);
void entity_init_projectile(quark_t* quark,
    const char* name,
    vec3 position,
    int owner_id,
    mesh_t* mesh,
    vec3 velocity,
    float speed,
    bool gravity,
    float damage,
    bool particles,
    entity_t** out);
void entity_init_explosion(quark_t* quark,
    const char* name, vec3 position, float radius, vec3 direction,
    float min_force, float max_force, entity_t** out);
void entity_init_pickup(quark_t* quark,
    const char* name, vec3 position, mesh_t* mesh, void* pickup_sound, entity_t** out);
void entity_init_vehicle(quark_t* quark,
    const char* name, vec3 position, mesh_t* mesh, vehicle_type_t type, entity_t** out);
void entity_init_sun_light(quark_t* quark,
    const char* name,
    float x, float y, float z,
    vec3 dir, vec3 color, entity_t** out);
void entity_init_point_light(quark_t* quark,
    const char* name, float x, float y, float z, vec3 color, entity_t** out);
void entity_free(quark_t* quark, entity_t* entity);

void entity_tick_projectile(quark_t* quark, entity_t* entity, entity_projectile_t* projectile);
void entity_tick_explosion(quark_t* quark, entity_t* entity, entity_explosion_t* explosion);
void entity_tick_pickup(quark_t* quark, entity_t* entity, entity_pickup_t* pickup);
void entity_tick_vehicle(quark_t* quark, entity_t* entity, entity_vehicle_t* vehicle);
void entity_tick_vehicle_helicopter(quark_t* quark, entity_t* entity, veh_helicopter* heli);

mesh_t* entity_get_mesh(quark_t* quark, entity_t* entity);
void entity_get_materials(quark_t* quark, entity_t* entity, material_t** materials, size_t* nmaterials);
bool entity_get_drawcall(quark_t* quark, entity_t* entity, drawcall_t* drawcall);

void entity_mesh_set_material(quark_t* quark, entity_t* entity, material_t* material, int slot);
void entity_pickup_set_material(quark_t* quark, entity_t* entity, material_t* material, int slot);
void entity_vehicle_set_material(quark_t* quark, entity_t* entity, material_t* material, int slot);

void entlist_init(quark_t* quark, entlist_t* entlist);
void entlist_free(quark_t* quark, entlist_t* entlist);
void entlist_tick(quark_t* quark, entlist_t* entlist);
void entlist_add(quark_t* quark, entlist_t* entlist, entity_t* entity);
void entlist_remove(quark_t* quark, entlist_t* entlist, entity_t* entity);
entity_t* entlist_find_by_name(quark_t* quark, entlist_t* entlist, const char* name);

#endif
