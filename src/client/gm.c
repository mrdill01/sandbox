#include "gm.h"
#include "quark.h"
#include "player.h"

static const char* game_mode_names[] = {"Capture the Flag"};
static const char* game_state_names[] = {"Waiting for players...", "Warmup", "Playing"};

void gm_start(quark_t* quark, game_mode_t* gm, game_mode_type_t gm_type) {
    gm->type = gm_type;
    gm_set_state(quark, gm, GAME_STATE_WAITING);
    gm->state_name = game_state_names[gm->state];
    gm->timer = 0.0f;
    info(quark, "[gm] game mode set to %s", game_mode_names[gm->type]);
}

void gm_tick(quark_t* quark, game_mode_t* gm) {
    gm->timer += quark->dt;
}

void gm_set_state(quark_t* quark, game_mode_t* gm, game_state_t state) {
    gm->state = state;
    info(quark, "[gm] set game state to %s", game_state_names[state]);
}

player_t* gm_spawn_player(quark_t* quark, bool is_bot) {
    int id = -1;
    for (int i = 0; i < NET_MAX_PLAYERS; i++) {
        if (!quark->players[i]) {
            quark->players[i] = player_new(quark, i, is_bot);
            id = i;
            break;
        }
    }
    
    if (id == -1) {
        error(quark, "the server is full");
        return NULL;
    }

    info(quark, "spawning player #%d", id);
    return quark->players[id];
}

void gm_respawn_player(quark_t* quark, player_t* player) {
    if (quark->time - player->death_time < sv_respawn_time.value)
        return;
    player_respawn(quark, player);
}
