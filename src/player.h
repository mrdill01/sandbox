#ifndef PLAYER_H
#define PLAYER_H

#include "math.h"

typedef struct sbox_t sbox_t;
typedef struct camera_t camera_t;

typedef enum {
    MOVE_WALK,
    MOVE_SPRINT,
    MOVE_CROUCH,
} move_mode_t;

typedef struct {
    vec3 position;
    move_mode_t move_mode;
    vec3 wishdir;
    vec3 wishvel;
    vec3 velocity;
    float wishspeed;
    float camera_height;
} player_t;

void player_init(sbox_t* sbox, player_t* player);
void player_tick(sbox_t* sbox, player_t* player, camera_t* camera);

#endif
