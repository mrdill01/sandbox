#ifndef CORE_H
#define CORE_H

#include "config.h"
#include "render.h"
#include "audio.h"
#include "math.h"
#include "entity.h"
#include "player.h"
#include "map.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <assert.h>

#include <SDL2/SDL.h>

#define SANDBOX_VERSION "sbox 1.0"
#define SANDBOX_DEBUG
//#define SANDBOX_NO_AUDIO

#ifdef SANDBOX_DEBUG
#define unreachable(sbox) error(sbox, "unreachable code entered in %s:%d", __FILE__, __LINE__);
#else
#define unreachable(sbox) (void)sbox
#endif 

#define NUM_KEYS 512

typedef struct sbox_t {
    cvar_t* cvars;
    
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

    audio_t audio;

    map_t map;
    player_t player;
} sbox_t;

extern cvar_t r_width;
extern cvar_t r_height;
extern cvar_t r_scale;
extern cvar_t r_fullscreen;
extern cvar_t r_vsync;
extern cvar_t r_fov;
extern cvar_t r_shadow_res;
extern cvar_t r_debug_draw_colliders;
extern cvar_t a_device;
extern cvar_t a_volume;
extern cvar_t m_sens;
extern cvar_t edit_mode;
extern cvar_t edit_snap_size;

void sbox_init(sbox_t* sbox);
void sbox_free(sbox_t* sbox);
void sbox_tick(sbox_t* sbox);
void sbox_reload_resources(sbox_t* sbox);

void info(sbox_t* sbox, const char* msg, ...);
void error(sbox_t* sbox, const char* msg, ...);
char* load_file(sbox_t* sbox, const char* path);
void clear_file(sbox_t* sbox, const char* path);

#endif
