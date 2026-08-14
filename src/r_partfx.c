#include "render.h"
#include "quark.h"

#include "../include/gl.h"

void r_add_partfx_shoot_hit(quark_t* quark, renderer_t* renderer, trace_result_t trace) {
    for (int i = 0; i < 100; i++) {
        /*vec3 velocity;
        glm_vec3_copy(trace.normal, velocity);
        glm_vec3_scale(velocity, 10.0f, velocity);*/

        vec3 velocity = {
            random(-10.0f, 10.0f),
            random(-10.0f, 10.0f),
            random(-10.0f, 10.0f)};
        
        texture_t* texture = renderer->p_fire;
        float alpha = 1.0f;
        float size = random(0.025f, 0.05f);
        float lifetime = random(0.1f, 0.15f);
        if (trace.phys_mat == PHYS_MAT_PLAYER) {
            texture = renderer->p_blood;
            alpha = 0.75f;
            size = random(0.15f, 0.2f);
            lifetime = random(0.15f, 0.2f);
        }

        particle_t* particle = r_add_particle(quark, &quark->renderer,
            trace.point, velocity, texture, GLM_VEC3_ONE,
            alpha, size, lifetime, PARTICLE_FADE_OUT);
        particle->apply_gravity = true;
    }

    if (trace.phys_mat != PHYS_MAT_PLAYER) {
        vec3 bullet_hole_position;
        glm_vec3_copy(trace.point, bullet_hole_position);
        
        vec3 offset;
        glm_vec3_copy(trace.normal, offset);
        glm_vec3_scale(offset, 0.025f, offset);

        glm_vec3_sub(bullet_hole_position, offset, bullet_hole_position);

        r_add_particle(quark, &quark->renderer,
            bullet_hole_position, GLM_VEC3_ZERO, renderer->p_bullet_hole, GLM_VEC3_ONE,
            1.0f, random(0.065f, 0.085f), 30.0f, 0);
    }
}

void r_add_partfx_shoot_hit_water(quark_t* quark, renderer_t* renderer, trace_result_t trace) {
    for (int i = 0; i < 100; i++) {
        vec3 velocity;
        glm_vec3_zero(velocity);
        velocity[0] += random(-2.5f, 2.5f);
        velocity[1] = random(2.5f, 5.25f);
        velocity[2] += random(-2.5f, 2.5f);

        particle_t* particle =
            r_add_particle(quark, &quark->renderer, trace.enter_water_point,
                velocity, renderer->p_water, GLM_VEC3_ONE,
                0.4f, random(0.11f, 0.14f), 3.0f, PARTICLE_FADE_OUT);
        particle->apply_gravity = true;
    }
}

void r_add_partfx_shoot_beam(
    quark_t* quark, renderer_t* renderer, vec3 start, vec3 dir, float distance)
{
    const float PARTICLES_PER_UNIT = 15.0f;

    vec3 position;
    glm_vec3_copy(start, position);

    for (int i = 0; i < distance * PARTICLES_PER_UNIT; i++) {
        vec3 step;
        glm_vec3_scale(dir, 1.0f / PARTICLES_PER_UNIT, step);
        glm_vec3_add(position, step, position);

        vec3 velocity = {
            random(-0.1f, 0.1f),
            random(-0.1f, 0.1f),
            random(-0.1f, 0.1f)};

        if (random(0.0f, 1.0f) >= 0.5f) {
            vec3 forward;
            glm_vec3_scale(dir, 20.0f, forward);
            glm_vec3_add(velocity, forward, velocity);
        }
        
        texture_t* texture = renderer->p_steam[(int)random(0, NUM_STEAM_PARTICLES)];
        r_add_particle(quark, &quark->renderer,
            position, velocity, texture, GLM_VEC3_ONE,
            0.15f, random(0.05f, 0.065f), random(1.0f, 1.5f),
            PARTICLE_FADE_OUT | PARTICLE_SCALE_X2);
    }
}

void r_add_partfx_projectile_smoke(
    quark_t* quark, renderer_t* renderer, vec3 position)
{
    vec3 velocity = {
        random(-0.2f, 0.2f),
        random(-0.2f, 0.2f),
        random(-0.2f, 0.2f)};

    texture_t* texture = renderer->p_steam[(int)random(0, NUM_STEAM_PARTICLES)];
    r_add_particle(quark, &quark->renderer,
        position, velocity, texture, GLM_VEC3_ONE,
        0.3f, random(0.125f, 0.175f), random(1.3f, 1.5f), PARTICLE_FADE_OUT);
}

void r_add_partfx_hit_ground(quark_t* quark, renderer_t* renderer, vec3 position, material_t* material) {
    for (int i = 0; i < 20; i++) {
        vec3 velocity = {
            random(-2.0f, 2.0f),
            random(2.0f, 4.0f),
            random(-2.0f, 2.0f)};
        particle_t* particle =
            r_add_particle(quark, &quark->renderer, position, velocity, material->albedo,
                GLM_VEC3_ONE, 0.75f, random(0.025f, 0.075f), 0.5f, PARTICLE_FADE_OUT);
        particle->apply_gravity = true;

        vec3 smoke_position = {
            position[0] + random(-0.35f, 0.35f),
            position[1] + random(-0.35f, 0.35f),
            position[2] + random(-0.35f, 0.35f)};
        vec3 smoke_velocity = {
            random(-0.25f, 0.25f),
            random(-0.25f, 0.25f),
            random(-0.25f, 0.25f)};
        r_add_particle(quark, &quark->renderer, smoke_position, smoke_velocity,
            renderer->p_smoke, GLM_VEC3_ONE, 0.5f, 0.4f, random(2.0f, 3.0f), PARTICLE_FADE_OUT);
    }
}

void r_add_partfx_enter_water(
    quark_t* quark, renderer_t* renderer, vec3 position, vec3 velocity)
{
    for (int i = 0; i < 300; i++) {
        vec3 new_position;
        glm_vec3_copy(position, new_position);
        new_position[0] += random(-0.5f, 0.5f);
        new_position[1] += random(0.0f, 0.5f);
        new_position[2] += random(-0.5f, 0.5f);

        vec3 new_velocity;
        glm_vec3_copy(velocity, new_velocity);
        new_velocity[0] += random(-3.0f, 3.0f);
        new_velocity[1] = random(3.0f, 6.0f);
        new_velocity[2] += random(-3.0f, 3.0f);

        particle_t* particle =
            r_add_particle(quark, &quark->renderer, new_position, new_velocity, renderer->p_water,
                GLM_VEC3_ONE, 0.4f, random(0.11f, 0.14f), 3.0f, PARTICLE_FADE_OUT);
        particle->apply_gravity = true;
    }
}

void r_add_partfx_step_water(
    quark_t* quark, renderer_t* renderer, vec3 position, vec3 velocity)
{
    for (int i = 0; i < 65; i++) {
        vec3 new_position;
        glm_vec3_copy(position, new_position);
        new_position[0] += random(-0.25f, 0.25f);
        new_position[1] += random(0.0f, 0.25f);
        new_position[2] += random(-0.25f, 0.25f);

        vec3 new_velocity;
        glm_vec3_copy(velocity, new_velocity);
        new_velocity[0] += random(-2.0f, 2.0f);
        new_velocity[1] = random(0.1f, 1.0f);
        new_velocity[2] += random(-2.0f, 2.0f);

        particle_t* particle =
            r_add_particle(quark, &quark->renderer, new_position, new_velocity, renderer->p_water,
                GLM_VEC3_ONE, 0.35f, random(0.11f, 0.14f), 3.0f, PARTICLE_FADE_OUT);
        particle->apply_gravity = true;
    }
}

void r_add_partfx_explosion(
    quark_t* quark, renderer_t* renderer, vec3 position, float radius)
{
    for (int i = 0; i < 75; i++) {
        vec3 velocity = {
            random(-3.0f, 3.0f),
            random(-3.0f, 3.0f),
            random(-3.0f, 3.0f)};
        
        r_add_particle(quark, &quark->renderer,
            position, velocity, renderer->p_fire,
            (vec3){1.0f, random(0.25f, 1.0f), 0.5f},
            random(0.75f, 1.0f), random(0.35f, 0.5f), random(0.225f, 0.275f),
            PARTICLE_FADE_OUT | PARTICLE_SCALE_X2);
    }

    for (int i = 0; i < 100; i++) {
        vec3 velocity = {
            random(-10.0f, 10.0f),
            random(-10.0f, 10.0f),
            random(-10.0f, 10.0f)};
        
        particle_t* particle = r_add_particle(quark, &quark->renderer,
            position, velocity, renderer->p_fire, GLM_VEC3_ONE,
            1.0f, random(0.025f, 0.05f), random(0.2f, 0.3f), PARTICLE_FADE_OUT);
        particle->apply_gravity = true;
    }

    for (int i = 0; i < 100; i++) {
        vec3 new_position;
        glm_vec3_copy(position, new_position);
        new_position[0] += random(-0.5f, 0.5f);
        new_position[1] += random(-0.5f, 0.5f);
        new_position[2] += random(-0.5f, 0.5f);

        vec3 velocity = {
            random(-0.2f, 0.2f),
            random(4.0f, 6.5f),
            random(-0.2f, 0.2f)};

        texture_t* texture = renderer->p_steam[(int)random(0, NUM_STEAM_PARTICLES)];
        particle_t* particle = r_add_particle(quark, &quark->renderer,
            new_position, velocity, texture, (vec3){1.0f, random(0.5f, 1.0f), 0.2f},
            1.0f, random(0.2f, 0.3f), random(1.5f, 2.0f),
            PARTICLE_FADE_OUT | PARTICLE_SCALE_X2);
        particle->apply_gravity = true;
    }

    for (int i = 0; i < 25; i++) {
        vec3 velocity = {
            random(-0.25f, 0.25f),
            random(0.0f, 2.0f),
            random(-0.25f, 0.25f)};

        vec3 new_position = {
            position[0] + random(-0.25f, 0.25f),
            position[1] + random(-0.25f, 0.25f),
            position[2] + random(-0.25f, 0.25f)};
        r_add_particle(quark, &quark->renderer, new_position, velocity,
            renderer->p_smoke, GLM_VEC3_ONE, 0.5f, random(0.5f, 1.0f), random(2.0f, 3.0f),
            PARTICLE_FADE_OUT | PARTICLE_SCALE_X2);
    }
}

void r_add_partfx_pickup_coin(quark_t* quark, renderer_t* renderer, vec3 position) {
    for (int i = 0; i < 10; i++) {
        vec3 velocity = {
            random(-3.0f, 3.0f),
            random(-3.0f, 3.0f),
            random(-3.0f, 3.0f)};
        
        particle_t* particle = r_add_particle(quark, &quark->renderer,
            position, velocity, renderer->p_coin, GLM_VEC3_ONE,
            1.0f, random(0.05f, 0.1f), random(1.0f, 1.5f),
            PARTICLE_FADE_OUT);
        particle->apply_gravity = true;
    }
}

particle_t* r_add_particle(
    quark_t* quark,
    renderer_t* renderer,
    vec3 position,
    vec3 velocity,
    texture_t* texture,
    vec3 color,
    float alpha,
    float size,
    float lifetime,
    uint32_t flags)
{
    particle_t* particle = NULL;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (renderer->particles[i].is_free) {
            particle = &renderer->particles[i];
            break;
        }
    }

    if (!particle)
        particle = &renderer->particles[0];

    particle->is_free = false;
    glm_vec3_copy(position, particle->position);
    glm_vec3_copy(velocity, particle->velocity);
    particle->texture = texture;
    glm_vec3_copy(color, particle->color);
    particle->alpha = alpha;
    particle->init_alpha = alpha;
    particle->size = size;
    particle->init_size = size;
    particle->mesh = renderer->quad_mesh;
    particle->spawn_time = quark->time;
    particle->lifetime = lifetime;
    particle->flags = flags;
    particle->apply_gravity = false;
    return particle;
}

static int sort_back_to_front(const void* a_ptr, const void* b_ptr) {
	particle_t* a = (particle_t*)a_ptr;
	particle_t* b = (particle_t*)b_ptr;
    if (!a || !b) return 1;
	if (a->distance_to_camera < b->distance_to_camera) return 1;
	return -1;
}

void r_tick_particles(quark_t* quark, renderer_t* renderer) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particle_t* particle = &renderer->particles[i];
        if (particle->is_free) continue;

        if (quark->time - particle->spawn_time >= particle->lifetime) {
            particle->is_free = true;
            continue;
        }

        if (particle->flags & PARTICLE_FADE_OUT) {
            particle->alpha = lerp(particle->init_alpha, 0.0f,
                (quark->time - particle->spawn_time) / particle->lifetime);
        }

        if (particle->flags & PARTICLE_SCALE_X2) {
            particle->size = lerp(particle->init_size, particle->init_size * 2.0f,
                (quark->time - particle->spawn_time) / particle->lifetime);
        }

        if (particle->apply_gravity)
            particle->velocity[1] -= PHYS_GRAVITY * quark->dt;

        vec3 move;
        glm_vec3_copy(particle->velocity, move);
        glm_vec3_scale(move, quark->dt, move);
        glm_vec3_add(particle->position, move, particle->position);

		particle->distance_to_camera = glm_vec3_distance(
            particle->position, quark->renderer.camera.position);
    }

	qsort(renderer->particles, MAX_PARTICLES, sizeof(particle_t), sort_back_to_front);
}

void r_render_particles(quark_t* quark, renderer_t* renderer) {
    r_set_framebuffer(renderer, renderer->screen_buffer);
    r_set_shader(renderer, renderer->partfx_shader);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    glDepthMask(GL_FALSE);

    r_set_mat4(quark, renderer, "view", renderer->view);
    r_set_mat4(quark, renderer, "projection", renderer->projection);

    for (int i = 0; i < MAX_PARTICLES; i++) {
        particle_t* particle = &renderer->particles[i];
        if (particle->is_free) continue;

        r_set_vec3(quark, renderer, "position", particle->position);
        r_set_float(quark, renderer, "size", particle->size);
        r_set_vec3(quark, renderer, "camera_right", renderer->camera.right);
        r_set_vec3(quark, renderer, "camera_up", renderer->camera.up);

        r_set_texture(quark, renderer, "particle", particle->texture, 0);
        r_set_vec3(quark, renderer, "color", particle->color);
        r_set_float(quark, renderer, "alpha", particle->alpha);

        r_draw_mesh(renderer, particle->mesh);
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    r_set_framebuffer(renderer, NULL);
}

int r_get_particle_count(quark_t* quark, renderer_t* renderer) {
    int count = 0;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particle_t* particle = &renderer->particles[i];
        if (particle->is_free) continue;
        count++;
    }
    return count;
}
