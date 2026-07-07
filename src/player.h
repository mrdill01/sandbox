#ifndef PLAYER_H
#define PLAYER_H

#include "math.h"

typedef struct sbox_t sbox_t;
typedef struct camera_t camera_t;

typedef struct {
    vec3 position;
    vec3 wishdir;
    vec3 velocity;
    float speed;
    float camera_height;
} player_t;

void player_init(player_t* player);
void player_tick(sbox_t* sbox, player_t* player, camera_t* camera);

#endif
