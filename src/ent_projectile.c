#include "entity.h"
#include "sbox.h"

#define PROJECTILE_MAX_LIFETIME 5.0f
#define PROJECTILE_PARTICLE_RATE 0.01f

void entity_init_projectile(sbox_t* sbox,
    const char* name, vec3 position, mesh_t* mesh, vec3 velocity, entity_t** out)
{
	entity_t* entity = NULL;
	entity_init_common(sbox, name, ENTITY_PROJECTILE, position, &entity);
    entity->data.projectile.mesh = mesh;
    for (int i = 0; i < MAX_MATERIALS; i++)
        entity->data.projectile.materials[i] = NULL;

    glm_vec3_copy(position, entity->data.projectile.start);
    glm_vec3_copy(velocity, entity->data.projectile.velocity);
    entity->data.projectile.last_particle = sbox->time;

    *out = entity;
}

void entity_tick_projectile(sbox_t* sbox, entity_t* entity, entity_projectile_t* projectile) {
    if (sbox->time - entity->spawn_time >= PROJECTILE_MAX_LIFETIME)
        entlist_remove(sbox, &sbox->map.entlist, entity);
    
    vec3 move;
    glm_vec3_copy(projectile->velocity, move);
    glm_vec3_scale(move, sbox->dt, move);

    glm_vec3_add(entity->position, move, entity->position);

    entlist_t* entlist = &sbox->map.entlist;
    for (size_t i = 0; i < entlist->len; i++) {
        entity_t* other = entlist->ents[i];
        if (!other || other == entity) continue;
        material_t* materials[MAX_MATERIALS] = {0};
        entity_get_materials(sbox, other, materials, NULL);

        if (bbox_point_intersects(&other->world_bbox, entity->position) &&
            materials[0] && strcmp(materials[0]->name, "water") != 0)
        {
            entity_t* explosion = NULL;
            entity_init_explosion(sbox, "explosion", entity->position, M_PI, &explosion);
            entlist_add(sbox, &sbox->map.entlist, explosion);
            entlist_remove(sbox, &sbox->map.entlist, entity);
        }
    }

    float dist = glm_vec3_distance(projectile->start, entity->position);
    if (dist > projectile->last_particle * PROJECTILE_PARTICLE_RATE) {
        projectile->last_particle = dist * PROJECTILE_PARTICLE_RATE;
        r_add_partfx_projectile_smoke(sbox, &sbox->renderer, entity->position);
    }
}
