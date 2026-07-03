#include "core.h"
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
    mesh_t* quad_mesh = mesh_load(&sbox, "res/meshes/quad.obj");
    mesh_t* crate_mesh = mesh_load(&sbox, "res/meshes/crate.obj");
    mesh_t* floor_mesh = mesh_load(&sbox, "res/meshes/floor.obj");
    mesh_t* barrel_mesh = mesh_load(&sbox, "res/meshes/barrel.obj");
    mesh_t* chainlink_fence_mesh = mesh_load(&sbox, "res/meshes/chainlink_fence.obj");
    mesh_t* tommy_gun_mesh = mesh_load(&sbox, "res/meshes/tommy_gun.obj");
    
    material_t* crate = material_load(&sbox,
        "res/textures/crate.png",
        "res/textures/crate_r.png",
        "res/textures/crate_n.png",
        1, 1, false);

    material_t* chainlink = material_load(&sbox,
        "res/textures/chainlink.png",
        "res/textures/chainlink_r.png",
        "res/textures/chainlink_n.png",
        20, 20, true);

    material_t* metal = material_load(&sbox,
        "res/textures/metal.png",
        "res/textures/metal_r.png",
        "res/textures/metal_n.png",
        1, 1, false);

    material_t* barrel = material_load(&sbox,
        "res/textures/barrel.png",
        "res/textures/barrel_r.png",
        "res/textures/barrel_n.png",
        1, 1, false);

    material_t* wood = material_load(&sbox,
        "res/textures/wood.png",
        "res/textures/wood_r.png",
        "res/textures/wood_n.png",
        1, 1, false);

    ent_t floor_entity = {0};
    floor_entity.name = "floor";
    vec3 pos3 = {0, -0.5, 0};
    glm_vec3_copy(pos3, floor_entity.position);
    floor_entity.mesh = floor_mesh;
    floor_entity.nmaterials = 0;
    floor_entity.materials[floor_entity.nmaterials++] = metal;
    floor_entity.dist_to_camera = 0.0f;
    add_ent(&sbox, floor_entity);

    ent_t crate_entity = {0};
    crate_entity.name = "crate";
    vec3 pos = {0, 0, 0};
    glm_vec3_copy(pos, crate_entity.position);
    crate_entity.mesh = crate_mesh;
    crate_entity.nmaterials = 0;
    crate_entity.materials[crate_entity.nmaterials++] = crate;
    crate_entity.dist_to_camera = 0.0f;
    add_ent(&sbox, crate_entity);

    ent_t crate2_entity = {0};
    crate2_entity.name = "crate";
    vec3 pos5 = {-1, 0, 0.1};
    glm_vec3_copy(pos5, crate2_entity.position);
    crate2_entity.mesh = crate_mesh;
    crate2_entity.nmaterials = 0;
    crate2_entity.materials[crate2_entity.nmaterials++] = crate;
    crate2_entity.dist_to_camera = 0.0f;
    add_ent(&sbox, crate2_entity);

    ent_t barrel_entity = {0};
    barrel_entity.name = "barrel";
    vec3 pos4 = {1.5, 0, 0};
    glm_vec3_copy(pos4, barrel_entity.position);
    barrel_entity.mesh = barrel_mesh;
    barrel_entity.nmaterials = 0;
    barrel_entity.materials[barrel_entity.nmaterials++] = barrel;
    barrel_entity.dist_to_camera = 0.0f;
    add_ent(&sbox, barrel_entity);

    ent_t chainlink_fence_entity = {0};
    chainlink_fence_entity.name = "chainlink fence";
    vec3 pos2 = {0, -0.5, 0.8};
    glm_vec3_copy(pos2, chainlink_fence_entity.position);
    chainlink_fence_entity.mesh = chainlink_fence_mesh;
    chainlink_fence_entity.nmaterials = 0;
    chainlink_fence_entity.materials[chainlink_fence_entity.nmaterials++] = wood;
    chainlink_fence_entity.materials[chainlink_fence_entity.nmaterials++] = chainlink;
    chainlink_fence_entity.dist_to_camera = 0.0f;
    add_ent(&sbox, chainlink_fence_entity);

    ent_t tommy_gun_entity = {0};
    tommy_gun_entity.name = "tommy gun";
    vec3 pos6 = {1.5f, 1.5f, 0.0f};
    glm_vec3_copy(pos6, tommy_gun_entity.position);
    tommy_gun_entity.mesh = tommy_gun_mesh;
    tommy_gun_entity.nmaterials = 0;
    tommy_gun_entity.materials[tommy_gun_entity.nmaterials++] = metal;
    tommy_gun_entity.dist_to_camera = 0.0f;
    add_ent(&sbox, tommy_gun_entity);
    
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

    time_t current_time = time(NULL);
	struct tm* local_time = localtime(&current_time);
	char* time_string = asctime(local_time);
	time_string[strlen(time_string) - 1] = '\0';
    info(sbox, "current date & time: %s", time_string);

    info(sbox, "opengl gpu: %s", glGetString(GL_RENDERER));
    info(sbox, "opengl vendor: %s", glGetString(GL_VENDOR));
    info(sbox, "opengl version: %s", glGetString(GL_VERSION));

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

    if (sbox->keys[SDL_SCANCODE_W]) {
        sbox->camera.position[2] -= 1 * sbox->dt;
    }

    if (sbox->keys[SDL_SCANCODE_S]) {
        sbox->camera.position[2] += 1 * sbox->dt;
    }

    if (sbox->keys[SDL_SCANCODE_A]) {
        sbox->camera.position[0] -= 1 * sbox->dt;
    }

    if (sbox->keys[SDL_SCANCODE_D]) {
        sbox->camera.position[0] += 1 * sbox->dt;
    }

    if (sbox->keys[SDL_SCANCODE_G]) {
        camera_add_pitch(&sbox->camera, 30.0f);
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
    glm_perspective(rad(sbox->camera.fov),
        (float)sbox->cfg.v_width / sbox->cfg.v_height,
        0.001f,
        100.0f,
        projection);
    shader_set_mat4(sbox->world_shader, "projection", projection);

    mat4 view;
    vec3 dir;
    vec3 forward = {0.0f, 0.0f, -1.0f};
    vec3 up = {0.0f, 1.0f, 0.0f};
    glm_vec3_add(sbox->camera.position, forward, dir);
    glm_lookat(sbox->camera.position, dir, up, view);
    shader_set_mat4(sbox->world_shader, "view", view);

    shader_set_vec3(sbox->world_shader, "view_pos", sbox->camera.position);

    vec3 light_position = {1.5f, 1.5f, -1.5f};
    shader_set_vec3(sbox->world_shader, "light.position", light_position);
    
    vec3 light_color = {4.0f, 4.0f, 4.0f};
    shader_set_vec3(sbox->world_shader, "light.color", light_color);

    for (int i = 0; i < sbox->entlist.len; i++) {
        ent_t* ent = &sbox->entlist.ents[i];

        mat4 model;
        glm_mat4_identity(model);
        glm_translate(model, ent->position);
        shader_set_mat4(sbox->world_shader, "model", model);

        for (int i = 0; i < ent->nmaterials; i++)
            video_set_material(sbox->world_shader, ent->materials[i], i);
        video_draw_mesh(ent->mesh);
    }

    SDL_GL_SwapWindow(sbox->window);
}

void shutdown(sbox_t* sbox) {
    info(sbox, "shutting down...");

    shader_t* shader = sbox->shaders;
    while (shader) {
        shader_t* next = shader->next;
        shader_free(sbox, shader);
        shader = next;
    }

    mesh_t* mesh = sbox->meshes;
    while (mesh) {
        mesh_t* next = mesh->next;
        mesh_free(sbox, mesh);
        mesh = next;
    }

    texture_t* texture = sbox->textures;
    while (texture) {
        texture_t* next = texture->next;
        texture_free(sbox, texture);
        texture = next;
    }

    material_t* material = sbox->materials;
    while (material) {
        material_t* next = material->next;
        material_free(sbox, material);
        material = next;
    }

    SDL_GL_DeleteContext(sbox->gl_context);
    SDL_DestroyWindow(sbox->window);

    info(sbox, "cleanup complete! exiting...");
}
