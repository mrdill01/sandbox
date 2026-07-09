#ifndef CORE_H
#define CORE_H

#include "math.h"
#include "render.h"
#include "entity.h"
#include "player.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include <SDL2/SDL.h>

#define SANDBOX_VERSION "sbox 1.0"

#define NUM_KEYS 512

typedef struct {
    int r_width;
    int r_height;
    float r_scale;
    float r_fov;
    float m_sens;
} config_t;

typedef struct sbox_t {
    config_t cfg;
    
    bool running;
    uint64_t now;
    uint64_t last;
    double dt;
    double time;

    bool keys[NUM_KEYS];
    float mxdt;
    float mydt;

    SDL_Window* window;
    SDL_GLContext* gl_context;
    renderer_t renderer;

    shader_t* shaders;
    mesh_t* meshes;
    texture_t* textures;
    material_t* materials;

    entlist_t entlist;
    player_t player;
} sbox_t;

void sbox_init(sbox_t* sbox);
void sbox_free(sbox_t* sbox);
void sbox_tick(sbox_t* sbox);

void sbox_load_map(sbox_t* sbox);

void info(sbox_t* sbox, const char* msg, ...);
void error(sbox_t* sbox, const char* msg, ...);
char* load_file(sbox_t* sbox, const char* path);

#endif
