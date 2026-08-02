#include "entity.h"
#include "sbox.h"

#define PROJECTILE_MAX_LIFETIME 5.0f
#define PROJECTILE_PARTICLE_RATE 0.0001f

void entity_init_projectile(sbox_t* sbox,
    const char* name,
    vec3 position,
    int owner_id,
    mesh_t* mesh,
    vec3 velocity,
    float damage,
    entity_t** out)
{
	entity_t* entity = NULL;
	entity_init_common(sbox, name, ENTITY_PROJECTILE, position, &entity);
    entity->data.projectile.owner_id = 0;
    entity->data.projectile.mesh = mesh;
    for (int i = 0; i < MAX_MATERIALS; i++)
        entity->data.projectile.materials[i] = NULL;

    glm_vec3_copy(position, entity->data.projectile.start);
    glm_vec3_copy(velocity, entity->data.projectile.velocity);
    entity->data.projectile.damage = damage;
    entity->data.projectile.last_particle = sbox->time;

    *out = entity;
}

void entity_tick_projectile(sbox_t* sbox, entity_t* entity, entity_projectile_t* projectile) {
    if (sbox->time - entity->spawn_time >= PROJECTILE_MAX_LIFETIME)
        entlist_remove(sbox, &sbox->map.entlist, entity);

    vec3 dir;
    glm_vec3_copy(projectile->velocity, dir);
    glm_vec3_norm(dir);

    vec3 move;
    glm_vec3_copy(projectile->velocity, move);
    glm_vec3_scale(move, sbox->dt, move);

    float max_distance = 16.0f * sbox->dt;
    printf("%g\n", max_distance);

    vec3 end;
    end[0] = entity->position[0] + dir[0] * 0.511;
    end[1] = entity->position[1] + dir[1] * 0.511;
    end[2] = entity->position[2] + dir[2] * 0.511;
    r_add_line(sbox, &sbox->renderer, entity->position, end, COLOR_BLUE, 5.0f);
    
    entlist_t* entlist = &sbox->map.entlist;
    trace_result_t trace;
    if (phys_line_trace(
        sbox, entity->position, dir, max_distance, entlist, projectile->owner_id, &trace))
    {
        if (trace.entity && trace.entity->name)
            printf("%s\n", trace.entity->name);
        entity_t* explosion = NULL;
        entity_init_explosion(sbox, "explosion", entity->position, 4.0f, &explosion);
        entlist_add(sbox, &sbox->map.entlist, explosion);
        entlist_remove(sbox, &sbox->map.entlist, entity);
    } else {
        glm_vec3_add(entity->position, move, entity->position);
    }

    if (sbox->time - projectile->last_particle >= PROJECTILE_PARTICLE_RATE) {
        projectile->last_particle = sbox->time;
        r_add_partfx_projectile_smoke(sbox, &sbox->renderer, entity->position);
    }
}
