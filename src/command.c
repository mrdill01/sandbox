#include "command.h"
#include "sbox.h"
#include "console.h"
#include "net.h"

cmd_t host = {"host", "Hosts a new game."};
cmd_t disconnect = {"disconnect", "Disconnects from the server."};
cmd_t quit = {"quit", "Quits the game."};

void cmd_init(sbox_t* sbox) {
    cmd_register(sbox, &host);
    cmd_register(sbox, &disconnect);
    cmd_register(sbox, &quit);
}

void cmd_register(sbox_t* sbox, cmd_t* cmd) {
    info(sbox, "register command %s", cmd->name);
    cmd->next = sbox->cmds;
    sbox->cmds = cmd;
}

static cmd_t* cmd_find(sbox_t* sbox, const char* name) {
    cmd_t* cmd = sbox->cmds;
    while (cmd) {
        if (strcmp(cmd->name, name) == 0)
            return cmd;
        cmd = cmd->next;
    }

    return NULL;
}

void cmd_run(sbox_t* sbox, const char* name, const char** args, int argc) {
    cmd_t* cmd = cmd_find(sbox, name);
    if (!cmd) {
        error(sbox, "command not found: %s", name);
        return;
    }

    if (strcmp(cmd->name, "host") == 0) {
        sv_start(sbox, &sbox->server, NET_PORT);
        cl_connect(sbox, &sbox->client, "127.0.0.1", NET_PORT);

        sbox->ui_state = UI_STATE_LOADING;
        map_load(sbox, &sbox->map);

        sbox->players[0] = gm_spawn_player(sbox, false);
        sbox->player = sbox->players[0];
        sbox->player->is_me = true;
        return;
    }

    if (strcmp(cmd->name, "disconnect") == 0) {
        cl_disconnect(sbox, &sbox->client);
        sv_stop(sbox, &sbox->server);
        
        map_free(sbox, &sbox->map);
        sbox->player = NULL;
        for (int i = 0; i < MAX_PLAYERS; i++)
		    sbox->players[i] = NULL;
        
        sbox->ui_state = UI_STATE_MAIN_MENU;
        return;
    }

    if (strcmp(cmd->name, "quit") == 0) {
        sbox->running = false;
        return;
    }
}
