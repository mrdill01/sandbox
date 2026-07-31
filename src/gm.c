#include "gm.h"
#include "sbox.h"
#include "player.h"

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
