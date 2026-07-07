#include "sbox.h"
#include "video.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include <SDL2/SDL.h>
#include "../include/gl.h"
#include "../include/cglm/cglm.h"

int init(sbox_t* sbox);
void tick(sbox_t* sbox);
void render(sbox_t* sbox);
void shutdown(sbox_t* sbox);

int main(int argc, char* argv[]) {
    sbox_t sbox;
    sbox_init(&sbox);

    if (init(&sbox) == -1)
        return EXIT_FAILURE;

    sbox.world_shader = shader_load(&sbox, "res/shaders/world.vs", "res/shaders/world.fs");
    sbox_load_map(&sbox);

    while (sbox.running) {
        tick(&sbox);
        render(&sbox);
    }

    shutdown(&sbox);

    return EXIT_SUCCESS;
}

int init(sbox_t* sbox) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        error(sbox, "failed to initialize SDL: %s", SDL_GetError());
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    sbox->window = SDL_CreateWindow("sandbox",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        sbox->cfg.v_width,
        sbox->cfg.v_height,
        SDL_WINDOW_OPENGL);

    if (!sbox->window) {
        error(sbox, "failed to open window: %s", SDL_GetError());
        return -1;
    }

    sbox->gl_context = SDL_GL_CreateContext(sbox->window);

    if (!sbox->gl_context) {
        error(sbox, "failed to setup OpenGL context: %s", SDL_GetError());
        return -1;
    }

    if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress)) {
        error(sbox, "failed to initialize GLAD");
        return -1;
    }

    info(sbox, "%s", SANDBOX_VERSION);

    time_t current_time = time(NULL);
	struct tm* local_time = localtime(&current_time);
	char* time_string = asctime(local_time);
	time_string[strlen(time_string) - 1] = '\0';
    info(sbox, "current date and time: %s", time_string);

    info(sbox, "GPU: %s", glGetString(GL_RENDERER));
    info(sbox, "vendor: %s", glGetString(GL_VENDOR));
    info(sbox, "version: %s", glGetString(GL_VERSION));
    return 0;
}

void tick(sbox_t* sbox) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT: sbox->running = false; break;
        case SDL_WINDOWEVENT: {
            switch (e.window.event) {
            case SDL_WINDOWEVENT_RESIZED: {
                glViewport(0, 0, e.window.data1, e.window.data2);
                break;
            }
            }
            break;
        }
        case SDL_KEYDOWN: {
            sbox->keys[e.key.keysym.scancode] = true;
            break;
        }
        case SDL_KEYUP: {
            sbox->keys[e.key.keysym.scancode] = false;
            break;
        }
        case SDL_MOUSEMOTION: {
            sbox->mxdt = e.motion.xrel;
            sbox->mydt = e.motion.yrel;
            break;
        }
        }
    }

    sbox_tick(sbox);

    if (sbox->keys[SDL_SCANCODE_ESCAPE]) {
        sbox->running = false;
    }
}

void render(sbox_t* sbox) {
    glClearColor(0.25, 0.5, 1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    video_set_shader(sbox->world_shader);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);

    mat4 projection;
    camera_get_projection_matrix(&sbox->camera, sbox->cfg.v_width, sbox->cfg.v_height, projection);
    shader_set_mat4(sbox->world_shader, "projection", projection);

    mat4 view;
    camera_get_view_matrix(&sbox->camera, view);
    shader_set_mat4(sbox->world_shader, "view", view);
    
    shader_set_vec3(sbox->world_shader, "view_pos", sbox->camera.position);

    vec3 light_position = {1.5f, 1.5f, -1.5f};
    shader_set_vec3(sbox->world_shader, "light.position", light_position);
    
    vec3 light_color = {4.0f, 4.0f, 4.0f};
    shader_set_vec3(sbox->world_shader, "light.color", light_color);

    for (int i = 0; i < sbox->entlist.len; i++) {
        entity_t* entity = sbox->entlist.ents[i];

        mat4 model;
        glm_mat4_identity(model);
        glm_translate(model, entity->position);

        if (strcmp(entity->name, "tommy gun") == 0)
            glm_quat(entity->rotation, rad(sbox->time * 30.0f), 0.0f, 1.0f, 0.0f);
        glm_quat_rotate(model, entity->rotation, model);

        shader_set_mat4(sbox->world_shader, "model", model);

        for (int i = 0; i < entity->nmaterials; i++)
            video_set_material(sbox->world_shader, entity->materials[i], i);
        video_draw_mesh(entity->mesh);
    }

    SDL_GL_SwapWindow(sbox->window);
}

void shutdown(sbox_t* sbox) {
    info(sbox, "shutting down...");

    int n = 0;
    shader_t* shader = sbox->shaders;
    while (shader) {
        shader_t* next = shader->next;
        shader_free(sbox, shader);
        shader = next;
        n++;
    }

    info(sbox, "released %d shader(s)", n);

    n = 0;
    mesh_t* mesh = sbox->meshes;
    while (mesh) {
        mesh_t* next = mesh->next;
        mesh_free(sbox, mesh);
        mesh = next;
        n++;
    }

    info(sbox, "released %d meshes(s)", n);

    n = 0;
    texture_t* texture = sbox->textures;
    while (texture) {
        texture_t* next = texture->next;
        texture_free(sbox, texture);
        texture = next;
        n++;
    }

    info(sbox, "released %d texture(s)", n);

    n = 0;
    material_t* material = sbox->materials;
    while (material) {
        material_t* next = material->next;
        material_free(sbox, material);
        material = next;
        n++;
    }

    info(sbox, "released %d material(s)", n);

    SDL_GL_DeleteContext(sbox->gl_context);
    SDL_DestroyWindow(sbox->window);

    sbox_free(sbox);
}
