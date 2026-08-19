#include "entity.h"
#include "quark.h"

#define PROJECTILE_MAX_LIFETIME 3.0f
#define PROJECTILE_PARTICLE_RATE 0.00001f

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
    entity_t** out)
{
	entity_t* entity = NULL;
	entity_init_common(quark, name, ENTITY_PROJECTILE, position, &entity);
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
    entity->data.projectile.last_particle = quark->time;

    *out = entity;
}

void entity_tick_projectile(quark_t* quark, entity_t* entity, entity_projectile_t* projectile) {
    if (quark->time - entity->spawn_time >= PROJECTILE_MAX_LIFETIME)
        entlist_remove(quark, &quark->map.entlist, entity);

    ray_t ray;
    glm_vec3_copy(entity->position, ray.origin);
    glm_vec3_copy(projectile->velocity, ray.dir);
    glm_normalize(ray.dir);

    float max_distance = projectile->speed * quark->dt;

    entlist_t* entlist = &quark->map.entlist;
    trace_result_t trace;
    int ignore[] = {entity->id};
    size_t nignore = 1;

    if (phys_line_trace(
        quark, ray, max_distance, entlist, projectile->owner_id, ignore, nignore, &trace))
    {
        entity_t* explosion = NULL;
        entity_init_explosion(quark, "explosion", entity->position, 2.0f, ray.dir,
            2.0f, 8.0f, &explosion);
        entlist_add(quark, &quark->map.entlist, explosion);
        entlist_remove(quark, &quark->map.entlist, entity);

    } else {
        vec3 prev_position;
        glm_vec3_copy(entity->position, prev_position);

        vec3 move;
        glm_vec3_copy(projectile->velocity, move);
        glm_vec3_scale(move, quark->dt, move);
        glm_vec3_add(entity->position, move, entity->position);

        if (r_debug_bullets.value)
            r_add_line(quark, &quark->renderer, prev_position, entity->position,
                COLOR_LIGHT_BLUE, 2.5f);
    }

    if (projectile->gravity) {
        projectile->velocity[1] -= PHYS_GRAVITY * quark->dt;
    }

    if (projectile->particles &&
        quark->time - projectile->last_particle >= 
            (r_particles.value == 0.0f) ? INFINITY : PROJECTILE_PARTICLE_RATE / r_particles.value)
    {
        projectile->last_particle = quark->time;
        r_add_partfx_projectile_trail(quark, &quark->renderer, entity->position);
    }
}
