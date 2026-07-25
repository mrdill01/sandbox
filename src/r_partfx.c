#include "render.h"
#include "sbox.h"

#include "../include/gl.h"

void r_add_partfx_shoot_hit(sbox_t* sbox, renderer_t* renderer, vec3 position, vec3 normal) {
    for (int i = 0; i < 50; i++) {
        /*vec3 velocity;
        glm_vec3_copy(normal, velocity);
        glm_vec3_scale(velocity, 10.0f, velocity);*/

        vec3 velocity = {
            random(-10.0f, 10.0f),
            random(-10.0f, 10.0f),
            random(-10.0f, 10.0f)};
        
        particle_t* particle = r_add_particle(sbox, &sbox->renderer, position, velocity, 0.05f);
        particle->apply_gravity = false;
    }
}

void r_add_partfx_hit_ground(sbox_t* sbox, renderer_t* renderer, vec3 position) {
    for (int i = 0; i < 20; i++) {
        vec3 velocity = {
            random(-10.0f, 10.0f),
            random(10.0f, 40.0f),
            random(-10.0f, 10.0f)};
        r_add_particle(sbox, &sbox->renderer, position, velocity, 0.05f);
    }
}

particle_t* r_add_particle(sbox_t* sbox, renderer_t* renderer, vec3 position, vec3 velocity, float size) {
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
    particle->size = size;
    particle->mesh = renderer->quad_mesh;
    particle->spawn_time = sbox->time;
    particle->lifetime = 2.0f;
    particle->apply_gravity = true;
    return particle;
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
            particle->velocity[1] -= PHYS_GRAVITY;

        vec3 move;
        glm_vec3_copy(particle->velocity, move);
        glm_vec3_scale(move, sbox->dt, move);

        glm_vec3_add(particle->position, move, particle->position);
    }
}

void r_render_particles(sbox_t* sbox, renderer_t* renderer) {
    r_set_framebuffer(renderer, renderer->screen_buffer);
    r_set_shader(renderer, renderer->partfx_shader);
    glEnable(GL_DEPTH_TEST);

    r_set_mat4(sbox, renderer, "view", renderer->view);
    r_set_mat4(sbox, renderer, "projection", renderer->projection);

    for (int i = 0; i < MAX_PARTICLES; i++) {
        particle_t* particle = &renderer->particles[i];
        if (particle->is_free) continue;

        r_set_vec3(sbox, renderer, "position", particle->position);
        r_set_float(sbox, renderer, "size", particle->size);
        r_set_vec3(sbox, renderer, "camera_right", renderer->camera.right);
        r_set_vec3(sbox, renderer, "camera_up", renderer->camera.up);

        r_draw_mesh(renderer, particle->mesh);
    }

    glDisable(GL_DEPTH_TEST);
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
