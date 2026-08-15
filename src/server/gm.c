#include "gm.h"
#include "../shared/quark.h"
#include "../shared/player.h"

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

player_t* gm_spawn_player(quark_t* quark, int id, bool is_bot) {
    quark->players[id] = player_new(quark, id, is_bot);
    
    info(quark, "spawning player id %d", id);
    return quark->players[id];
}

void gm_respawn_player(quark_t* quark, player_t* player) {
    if (quark->time - player->death_time < sv_respawn_time.value)
        return;
    player_respawn(quark, player);
}
