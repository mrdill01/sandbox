#include "entity.h"
#include "sbox.h"

void entity_init_projectile(sbox_t* sbox,
    const char* name, vec3 position, mesh_t* mesh, vec3 velocity, entity_t** out)
{
	entity_t* entity = NULL;
	entity_init_common(name, ENTITY_PROJECTILE, position, &entity);
    entity->data.projectile.mesh = mesh;
    for (int i = 0; i < MAX_MATERIALS; i++) {
        entity->data.projectile.materials[i] = NULL;
	}

    glm_vec3_copy(velocity, entity->data.projectile.velocity);

    *out = entity;
}

void entity_tick_projectile(sbox_t* sbox, entity_t* entity, entity_projectile_t* projectile) {
    vec3 move;
    glm_vec3_copy(projectile->velocity, move);
    glm_vec3_scale(move, sbox->dt, move);

    glm_vec3_add(entity->position, move, entity->position);

    entlist_t* entlist = &sbox->map.entlist;
    for (size_t i = 0; i < entlist->len; i++) {
        entity_t* other = entlist->ents[i];
        if (!other || other == entity) continue;
        
        if (bbox_point_intersects(&other->world_bbox, entity->position)) {
            entity_t* explosion = NULL;
            entity_init_explosion(sbox, "explosion", entity->position, 2.5f, &explosion);
            entlist_add(sbox, entlist, explosion);

            info(sbox, "collided with %s", other->name);

            entlist_remove(sbox, &sbox->map.entlist, entity);
        }
    }
}
