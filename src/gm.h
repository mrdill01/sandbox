#ifndef GM_H
#define GM_H

#include <stdbool.h>

typedef struct sbox_t sbox_t;
typedef struct player_t player_t;

typedef enum {
    GAME_MODE_CTF,
} game_mode_type_t;

typedef enum {
    GAME_STATE_WAITING,
    GAME_STATE_WARMUP,
    GAME_STATE_LIVE,
} game_state_t;

typedef struct {
    game_mode_type_t type;
    game_state_t state;
    const char* state_name;
    float timer;
} game_mode_t;

void gm_start(sbox_t* sbox, game_mode_t* gm, game_mode_type_t gm_type);
void gm_tick(sbox_t* sbox, game_mode_t* gm);
void gm_set_state(sbox_t* sbox, game_mode_t* gm, game_state_t state);
player_t* gm_spawn_player(sbox_t* sbox, bool is_bot);
void gm_respawn_player(sbox_t* sbox, player_t* player);

#endif
