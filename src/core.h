#ifndef CORE_H
#define CORE_H

#include "math.h"
#include "video.h"
#include "ent.h"

#include <stdbool.h>
#include <assert.h>

#include <SDL2/SDL.h>

#define NUM_KEYS 512

typedef struct {
    int v_width;
    int v_height;
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
    shader_t* world_shader;

    shader_t* shaders;
    mesh_t* meshes;
    texture_t* textures;
    material_t* materials;

    camera_t camera;
    entlist_t entlist;
} sbox_t;

void sbox_init(sbox_t* sbox);
void sbox_tick(sbox_t* sbox);

void add_ent(sbox_t* sbox, ent_t ent);

void info(sbox_t* sbox, const char* msg, ...);
void error(sbox_t* sbox, const char* msg, ...);
char* load_file(sbox_t* sbox, const char* path);

#endif
