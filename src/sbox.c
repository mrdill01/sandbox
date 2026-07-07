#include "sbox.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>

#define MAX_MSG_LEN 2048

void sbox_init(sbox_t* sbox) {
    sbox->cfg.v_width = 960;
    sbox->cfg.v_height = 540;
    sbox->cfg.v_fov = 75.0f;
    sbox->cfg.m_sens = 1.0f;

	sbox->running = true;
	sbox->now = SDL_GetPerformanceCounter();
	sbox->last = 0;
	sbox->dt = 0.0;
	sbox->time = 0.0;

	for (int i = 0; i < NUM_KEYS; i++)
		sbox->keys[i] = false;
	sbox->mxdt = 0.0f;
	sbox->mydt = 0.0f;

	sbox->window = NULL;
	sbox->gl_context = NULL;

	sbox->shaders = NULL;
	sbox->meshes = NULL;
	sbox->textures = NULL;
	sbox->materials = NULL;

    camera_init(&sbox->camera);
	entlist_init(sbox, &sbox->entlist);
    player_init(&sbox->player);
}

void sbox_free(sbox_t* sbox) {
	entlist_free(sbox, &sbox->entlist);
}

void sbox_tick(sbox_t* sbox) {
	sbox->last = sbox->now;
	sbox->now = SDL_GetPerformanceCounter();
   	sbox->dt = (sbox->now - sbox->last) / (double)SDL_GetPerformanceFrequency();
	sbox->time += sbox->dt;

	entlist_tick(sbox, &sbox->entlist);
    player_tick(sbox, &sbox->player, &sbox->camera);
}

void sbox_load_map(sbox_t* sbox) {
	//mesh_t* quad_mesh = mesh_load(sbox, "res/meshes/quad.obj");
    mesh_t* crate_mesh = mesh_load(sbox, "res/meshes/crate.obj");
    mesh_t* floor_mesh = mesh_load(sbox, "res/meshes/floor.obj");
    mesh_t* wall_mesh = mesh_load(sbox, "res/meshes/wall.obj");
    mesh_t* barrel_mesh = mesh_load(sbox, "res/meshes/barrel.obj");
    mesh_t* chainlink_fence_mesh = mesh_load(sbox, "res/meshes/chainlink_fence.obj");
    mesh_t* tommy_gun_mesh = mesh_load(sbox, "res/meshes/tommy_gun.obj");
    
    material_t* crate = material_load(sbox,
        "res/textures/crate.png",
        "res/textures/crate_r.png",
        "res/textures/crate_n.png",
        1, 1, false);

    material_t* chainlink = material_load(sbox,
        "res/textures/chainlink.png",
        "res/textures/chainlink_r.png",
        "res/textures/chainlink_n.png",
        20, 20, true);

    material_t* metal = material_load(sbox,
        "res/textures/metal.png",
        "res/textures/metal_r.png",
        "res/textures/metal_n.png",
        1, 1, false);

    material_t* barrel = material_load(sbox,
        "res/textures/barrel.png",
        "res/textures/barrel_r.png",
        "res/textures/barrel_n.png",
        1, 1, false);

    material_t* wood = material_load(sbox,
        "res/textures/wood.png",
        "res/textures/wood_r.png",
        "res/textures/wood_n.png",
        1, 1, false);

    material_t* brick = material_load(sbox,
        "res/textures/brick.png",
        "res/textures/brick_r.png",
        "res/textures/brick_n.png",
        8, 8, false);

    entity_t* entity;
    entity_init(sbox, "floor", 0.0f, -0.5f, 0.0f, &entity);
    entity->mesh = floor_mesh;
    entity->materials[entity->nmaterials++] = metal;
    entlist_add(sbox, &sbox->entlist, entity);

    entity_init(sbox, "wall", 0.0f, -0.5f, 4.0f, &entity);
    entity->mesh = wall_mesh;
    entity->materials[entity->nmaterials++] = brick;
    entlist_add(sbox, &sbox->entlist, entity);

    entity_init(sbox, "crate", 0.0f, 0.0f, 0.0f, &entity);
    entity->mesh = crate_mesh;
    entity->materials[entity->nmaterials++] = crate;
    entlist_add(sbox, &sbox->entlist, entity);

    entity_init(sbox, "crate(2)", -1.0f, 0.0f, 0.1f, &entity);
    entity->mesh = crate_mesh;
    entity->materials[entity->nmaterials++] = crate;
    entlist_add(sbox, &sbox->entlist, entity);

    entity_init(sbox, "crate(3)", -1.0f, 0.0f, -0.9f, &entity);
    entity->mesh = crate_mesh;
    entity->materials[entity->nmaterials++] = crate;
    entlist_add(sbox, &sbox->entlist, entity);

    entity_init(sbox, "crate(4)", -0.8f, 1.0f, -0.5f, &entity);
    entity->mesh = crate_mesh;
    entity->materials[entity->nmaterials++] = crate;
    entlist_add(sbox, &sbox->entlist, entity);

    entity_init(sbox, "barrel", 1.5f, 0.0f, 0.0f, &entity);
    entity->mesh = barrel_mesh;
    entity->materials[entity->nmaterials++] = barrel;
    entlist_add(sbox, &sbox->entlist, entity);

    entity_init(sbox, "chainlink fence", 0.0f, -0.5f, 0.8f, &entity);
    entity->mesh = chainlink_fence_mesh;
    entity->materials[entity->nmaterials++] = wood;
    entity->materials[entity->nmaterials++] = chainlink;
    entlist_add(sbox, &sbox->entlist, entity);

    entity_init(sbox, "tommy gun", 1.5f, 0.7f, 0.0f, &entity);
    entity->mesh = tommy_gun_mesh;
    entity->materials[entity->nmaterials++] = metal;
    entity->materials[entity->nmaterials++] = wood;
    entlist_add(sbox, &sbox->entlist, entity);
}

void info(sbox_t* sbox, const char* msg, ...) {
    char buffer[MAX_MSG_LEN];
	va_list args;
	va_start(args, msg);
	vsnprintf(buffer, MAX_MSG_LEN, msg, args);
	va_end(args);

    printf("%s\n", buffer);
}

void error(sbox_t* sbox, const char* msg, ...) {
    char buffer[MAX_MSG_LEN];
	va_list args;
	va_start(args, msg);
	vsnprintf(buffer, MAX_MSG_LEN, msg, args);
	va_end(args);

    printf("error: %s\n", buffer);
}

char* load_file(sbox_t* sbox, const char* path) {
	FILE* fp = fopen(path, "r");
	if (!fp) error(sbox, "failed to read '%s'", path);
	
	fseek(fp, 0, SEEK_END);
	size_t len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
    char* text = malloc(len + 1);
	fread(text, 1, len + 1, fp);
	fclose(fp);
	text[len] = '\0';
	return text;
}
