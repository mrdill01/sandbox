#ifndef GM_H
#define GM_H

typedef struct sbox_t sbox_t;
typedef struct player_t player_t;

typedef struct {
    float timer;
} gamemode_t;

player_t* gm_spawn_player(sbox_t* sbox);

#endif
