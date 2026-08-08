#include "gm.h"
#include "sbox.h"
#include "player.h"

static const char* game_mode_names[] = {"Capture the Flag"};
static const char* game_state_names[] = {"Waiting for players...", "Warmup", "Playing"};

void gm_start(sbox_t* sbox, game_mode_t* gm, game_mode_type_t gm_type) {
    gm->type = gm_type;
    gm_set_state(sbox, gm, GAME_STATE_WAITING);
    gm->state_name = game_state_names[gm->state];
    gm->timer = 0.0f;
    info(sbox, "[gm] game mode set to %s", game_mode_names[gm->type]);
}

void gm_tick(sbox_t* sbox, game_mode_t* gm) {
    gm->timer += sbox->dt;
}

void gm_set_state(sbox_t* sbox, game_mode_t* gm, game_state_t state) {
    gm->state = state;
    info(sbox, "[gm] set game state to %s", game_state_names[state]);
}

player_t* gm_spawn_player(sbox_t* sbox, bool is_bot) {
    int id = -1;
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (!sbox->players[i]) {
            sbox->players[i] = player_new(sbox, i, is_bot);
            id = i;
            break;
        }
    }
    
    if (id == -1) {
        error(sbox, "the server is full");
        return NULL;
    }

    info(sbox, "spawning player #%d", id);
    return sbox->players[id];
}

void gm_respawn_player(sbox_t* sbox, player_t* player) {
    if (sbox->time - player->death_time < sv_respawn_time.value)
        return;
    player_respawn(sbox, player);
}
