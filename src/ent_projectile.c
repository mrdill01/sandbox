#include "entity.h"
#include "sbox.h"

#define PROJECTILE_MAX_LIFETIME 3.0f
#define PROJECTILE_PARTICLE_RATE 0.00001f

void entity_init_projectile(sbox_t* sbox,
    const char* name,
    vec3 position,
    int owner_id,
    mesh_t* mesh,
    vec3 velocity,
    float speed,
    bool gravity,
    float damage,
    bool particles,
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
    entity->data.projectile.speed = speed;
    entity->data.projectile.gravity = gravity;
    entity->data.projectile.damage = damage;
    entity->data.projectile.particles = particles;
    entity->data.projectile.last_particle = sbox->time;

    *out = entity;
}

void entity_tick_projectile(sbox_t* sbox, entity_t* entity, entity_projectile_t* projectile) {
    if (sbox->time - entity->spawn_time >= PROJECTILE_MAX_LIFETIME)
        entlist_remove(sbox, &sbox->map.entlist, entity);

    vec3 direction;
    glm_vec3_copy(projectile->velocity, direction);
    glm_normalize(direction);

    float max_distance = projectile->speed * sbox->dt;

    entlist_t* entlist = &sbox->map.entlist;
    trace_result_t trace;
    if (phys_line_trace(
        sbox, entity->position, direction, max_distance, entlist, projectile->owner_id, &trace))
    {
        entity_t* explosion = NULL;
        entity_init_explosion(sbox, "explosion", entity->position, M_PI, direction,
            2.0f, 12.0f, &explosion);
        entlist_add(sbox, &sbox->map.entlist, explosion);
        entlist_remove(sbox, &sbox->map.entlist, entity);

    } else {
        vec3 prev_position;
        glm_vec3_copy(entity->position, prev_position);

        vec3 move;
        glm_vec3_copy(projectile->velocity, move);
        glm_vec3_scale(move, sbox->dt, move);
        glm_vec3_add(entity->position, move, entity->position);

        if (r_debug_draw_bullets.value)
            r_add_line(sbox, &sbox->renderer, prev_position, entity->position,
                COLOR_LIGHT_BLUE, 2.5f);
    }

    if (projectile->gravity) {
        projectile->velocity[1] -= PHYS_GRAVITY * sbox->dt;
    }

    if (projectile->particles && sbox->time - projectile->last_particle >= PROJECTILE_PARTICLE_RATE) {
        projectile->last_particle = sbox->time;
        r_add_partfx_projectile_smoke(sbox, &sbox->renderer, entity->position);
    }
}
