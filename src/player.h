#ifndef PLAYER_H
#define PLAYER_H

#include "math.h"
#include "physics.h"

typedef struct sbox_t sbox_t;
typedef struct camera_t camera_t;
typedef struct entlist_t entlist_t;

typedef enum {
    MOVE_WALK,
    MOVE_SPRINT,
    MOVE_CROUCH,
} move_mode_t;

typedef struct player_t {
    move_mode_t move_mode;
    vec3 position;
    vec3 velocity;
    vec3 move_input;
    float target_speed;
    bool is_grounded;
    phys_material_t ground_mat;
    float water_level;
    float last_step_time;
    bool pressed_jump;
    float height;
} player_t;

void player_init(sbox_t* sbox, player_t* player);
void player_tick(sbox_t* sbox, player_t* player, camera_t* camera, entlist_t* entlist);

void player_get_top_position(sbox_t* sbox, player_t* player, vec3 position);
void player_get_bottom_position(sbox_t* sbox, player_t* player, vec3 position);

#endif
