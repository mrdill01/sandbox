#include "render.h"
#include "sbox.h"

#include "../include/gl.h"

void r_add_partfx_shoot_hit(sbox_t* sbox, renderer_t* renderer, trace_result_t trace) {
    for (int i = 0; i < 100; i++) {
        /*vec3 velocity;
        glm_vec3_copy(trace.normal, velocity);
        glm_vec3_scale(velocity, 10.0f, velocity);*/

        vec3 velocity = {
            random(-10.0f, 10.0f),
            random(-10.0f, 10.0f),
            random(-10.0f, 10.0f)};
        
        texture_t* texture = renderer->p_fire;
        if (trace.phys_mat == PHYS_MAT_PLAYER)
            texture = renderer->p_blood;

        particle_t* particle = r_add_particle(sbox, &sbox->renderer,
            trace.point, velocity, texture,
            1.0f, random(0.025f, 0.05f), random(0.1f, 0.15f));
        particle->apply_gravity = true;
    }

    if (trace.phys_mat != PHYS_MAT_PLAYER) {
        vec3 bullet_hole_position;
        glm_vec3_copy(trace.point, bullet_hole_position);
        
        vec3 offset;
        glm_vec3_copy(trace.normal, offset);
        glm_vec3_scale(offset, 0.025f, offset);

        glm_vec3_sub(bullet_hole_position, offset, bullet_hole_position);

        r_add_particle(sbox, &sbox->renderer,
            bullet_hole_position, GLM_VEC3_ZERO, renderer->p_bullet_hole,
            1.0f, random(0.065f, 0.085f), 15.0f);
    }
}

void r_add_partfx_shoot_beam(
    sbox_t* sbox, renderer_t* renderer, vec3 start, vec3 dir, float distance)
{
    const float PARTICLES_PER_UNIT = 5.0f;

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

        if (random(0.0f, 1.0f) > 0.5f) {
            vec3 forward;
            glm_vec3_scale(dir, 20.0f, forward);
            glm_vec3_add(velocity, forward, velocity);
        }
        
        r_add_particle(sbox, &sbox->renderer,
            position, velocity, renderer->p_steam,
            0.25f, random(0.04f, 0.065f), random(0.5f, 1.0f));
    }
}

void r_add_partfx_hit_ground(sbox_t* sbox, renderer_t* renderer, vec3 position, material_t* material) {
    for (int i = 0; i < 20; i++) {
        vec3 velocity = {
            random(-2.0f, 2.0f),
            random(2.0f, 4.0f),
            random(-2.0f, 2.0f)};
        particle_t* particle =
            r_add_particle(sbox, &sbox->renderer, position, velocity, material->albedo,
                0.75f, random(0.025f, 0.075f), 0.5f);
        particle->apply_gravity = true;

        vec3 smoke_position = {
            position[0] + random(-0.35f, 0.35f),
            position[1] + random(-0.35f, 0.35f),
            position[2] + random(-0.35f, 0.35f)};
        glm_vec3_zero(velocity);
        r_add_particle(sbox, &sbox->renderer, smoke_position, velocity,
            renderer->p_smoke, 0.5f, 0.4f, random(2.0f, 3.0f));
    }
}

void r_add_partfx_enter_water(
    sbox_t* sbox, renderer_t* renderer, vec3 position, vec3 velocity)
{
    for (int i = 0; i < 200; i++) {
        vec3 new_position;
        glm_vec3_copy(position, new_position);
        new_position[0] += random(-0.5f, 0.5f);
        new_position[1] += random(0.0f, 0.5f);
        new_position[2] += random(-0.5f, 0.5f);

        vec3 new_velocity;
        glm_vec3_copy(velocity, new_velocity);
        new_velocity[0] += random(-2.0f, 2.0f);
        new_velocity[1] = random(2.0f, 5.0f);
        new_velocity[2] += random(-2.0f, 2.0f);

        particle_t* particle =
            r_add_particle(sbox, &sbox->renderer, new_position, new_velocity, renderer->p_water,
                random(0.1f, 1.0f), random(0.11f, 0.14f), 3.0f);
        particle->apply_gravity = true;
    }
}

particle_t* r_add_particle(
    sbox_t* sbox,
    renderer_t* renderer,
    vec3 position,
    vec3 velocity,
    texture_t* texture,
    float alpha,
    float size,
    float lifetime) {
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
    particle->alpha = alpha;
    particle->size = size;
    particle->mesh = renderer->quad_mesh;
    particle->spawn_time = sbox->time;
    particle->lifetime = lifetime;
    particle->apply_gravity = false;
    return particle;
}

static int sort_back_to_front(const void* a_ptr, const void* b_ptr) {
	particle_t* a = (particle_t*)a_ptr;
	particle_t* b = (particle_t*)b_ptr;
    if (!a || !b) return 1;
	if (a->dist_to_camera < b->dist_to_camera) return 1;
	return -1;
}

void r_tick_particles(sbox_t* sbox, renderer_t* renderer) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particle_t* particle = &renderer->particles[i];
        if (particle->is_free) continue;

        if (sbox->time - particle->spawn_time >= particle->lifetime) {
            particle->is_free = true;
            continue;
        }

        if (particle->apply_gravity)
            particle->velocity[1] -= PHYS_GRAVITY * sbox->dt;

        vec3 move;
        glm_vec3_copy(particle->velocity, move);
        glm_vec3_scale(move, sbox->dt, move);

        glm_vec3_add(particle->position, move, particle->position);

        vec3 tmp;
		glm_vec3_sub(particle->position, sbox->renderer.camera.position, tmp);
		particle->dist_to_camera = glm_vec3_norm2(tmp);
    }

	qsort(renderer->particles, MAX_PARTICLES, sizeof(particle_t), sort_back_to_front);
}

void r_render_particles(sbox_t* sbox, renderer_t* renderer) {
    r_set_framebuffer(renderer, renderer->screen_buffer);
    r_set_shader(renderer, renderer->partfx_shader);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    r_set_mat4(sbox, renderer, "view", renderer->view);
    r_set_mat4(sbox, renderer, "projection", renderer->projection);

    for (int i = 0; i < MAX_PARTICLES; i++) {
        particle_t* particle = &renderer->particles[i];
        if (particle->is_free) continue;

        r_set_vec3(sbox, renderer, "position", particle->position);
        r_set_float(sbox, renderer, "size", particle->size);
        r_set_vec3(sbox, renderer, "camera_right", renderer->camera.right);
        r_set_vec3(sbox, renderer, "camera_up", renderer->camera.up);

        r_set_int(sbox, renderer, "particle", 0);
        r_set_texture(renderer, particle->texture, 0);
        r_set_float(sbox, renderer, "alpha", particle->alpha);

        r_draw_mesh(renderer, particle->mesh);
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    r_set_framebuffer(renderer, NULL);
}

int r_get_particle_count(sbox_t* sbox, renderer_t* renderer) {
    int count = 0;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particle_t* particle = &renderer->particles[i];
        if (particle->is_free) continue;
        count++;
    }
    return count;
}
