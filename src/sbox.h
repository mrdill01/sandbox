#ifndef SBOX_H
#define SBOX_H

#include "config.h"
#include "console.h"
#include "profiler.h"
#include "server.h"
#include "client.h"
#include "render.h"
#include "audio.h"
#include "math.h"
#include "entity.h"
#include "player.h"
#include "map.h"
#include "gm.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <assert.h>

#include <SDL2/SDL.h>

#define SBOX_VERSION "sbox alpha 1.0"
#define SBOX_DEBUG
//#define SBOX_NO_AUDIO

#ifdef SBOX_DEBUG
#define unreachable(sbox) error(sbox, "unreachable code entered in %s:%d", __FILE__, __LINE__);
#else
#define unreachable(sbox) (void)sbox
#endif

#define MAX_PLAYERS 32
#define MAX_NAME_LEN 32

typedef enum {
    UI_STATE_MAIN_MENU,
    UI_STATE_LOADING,
    UI_STATE_IN_GAME,
    UI_STATE_PAUSE_MENU,
    UI_STATE_DEAD,
} ui_state_t;

#define NUM_KEYS 512
#define NUM_BUTTONS 3

typedef struct sbox_t {
    console_t console;
    cvar_t* cvars;
    cmd_t* cmds;
    
    bool running;
    uint64_t now;
    uint64_t last;
    double dt;
    double time;

    profiler_t prof;

    bool keys[NUM_KEYS];
    bool buttons[NUM_BUTTONS];
    bool prev_buttons[NUM_BUTTONS];
    float mx;
    float my;
    float mxdt;
    float mydt;

    SDL_Window* window;
    SDL_GLContext* gl_context;
    renderer_t renderer;
    shader_t* shaders;
    mesh_t* meshes;
    texture_t* textures;
    material_t* materials;

    server_t server;
    client_t client;

    audio_t audio;

    ui_state_t ui_state;
    game_mode_t gm;

    map_t map;
    player_t* players[MAX_PLAYERS];
    player_t* player;
} sbox_t;

extern cvar_t sv_cheats;
extern cvar_t sv_round_time;
extern cvar_t sv_timescale;
extern cvar_t sv_respawn_time;
extern cvar_t sv_destruction;
extern cvar_t sv_random_seed;
extern cvar_t cl_name;
extern cvar_t r_width;
extern cvar_t r_height;
extern cvar_t r_scale;
extern cvar_t r_fullscreen;
extern cvar_t r_vsync;
extern cvar_t r_fov;
extern cvar_t r_shadows;
extern cvar_t r_shadow_res;
extern cvar_t r_third_person;
extern cvar_t r_viewmodel;
extern cvar_t r_hud;
extern cvar_t r_debug_menu;
extern cvar_t r_debug_draw_colliders;
extern cvar_t r_debug_draw_bullets;
extern cvar_t r_debug_draw_players;
extern cvar_t r_debug_buffer;
extern cvar_t a_device;
extern cvar_t a_volume;
extern cvar_t m_sens;
extern cvar_t console;
extern cvar_t profiler;
extern cvar_t noclip;
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
