#ifndef GM_H
#define GM_H

#include <stdbool.h>

typedef struct sbox_t sbox_t;
typedef struct player_t player_t;

typedef struct {
    float timer;
} gamemode_t;

player_t* gm_spawn_player(sbox_t* sbox, bool is_bot);
void gm_respawn_player(sbox_t* sbox, player_t* player);

#endif
