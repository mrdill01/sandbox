#include "command.h"
#include "sbox.h"
#include "console.h"
#include "net.h"

cmd_t help = {"help", "Shows a help message for the console."};
cmd_t cmdlist = {"cmdlist", "Prints all commands to the console."};
cmd_t cvarlist = {"cvarlist", "Prints all cvars to the console."};
cmd_t reset = {"reset", "Resets a cvar to its default value."};
cmd_t clear = {"clear", "Clears the console history."};
cmd_t host = {"host", "Hosts a new game."};
cmd_t connect_ = {"connect", "Connects the client to a server."};
cmd_t disconnect = {"disconnect", "Disconnects from the server."};
cmd_t quit = {"quit", "Quits the game."};

void cmd_init(sbox_t* sbox) {
    cmd_register(sbox, &help);
    cmd_register(sbox, &cmdlist);
    cmd_register(sbox, &cvarlist);
    cmd_register(sbox, &reset);
    cmd_register(sbox, &clear);
    cmd_register(sbox, &host);
    cmd_register(sbox, &connect_);
    cmd_register(sbox, &disconnect);
    cmd_register(sbox, &quit);
}

void cmd_register(sbox_t* sbox, cmd_t* cmd) {
    info(sbox, "register command %s", cmd->name);
    cmd->next = sbox->cmds;
    sbox->cmds = cmd;
}

cmd_t* cmd_find(sbox_t* sbox, const char* name) {
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

    if (strcmp(cmd->name, "help") == 0) {
        info(sbox,
            "The console is used to run commands or set the values of cvars (config variables).");
        info(sbox,
            "Type 'cvarlist' for a list of cvars and 'cmdlist' for a list of commands.");
        info(sbox,
            "Press escape or F1 to close the console. Press enter to submit input.");
        return;
    }

    if (strcmp(cmd->name, "cmdlist") == 0) {
        cmd_t* cmd = sbox->cmds;
        while (cmd) {
            info(sbox, "%32s %s", cmd->name, cmd->desc);
            cmd = cmd->next;
        }
        return;
    }

    if (strcmp(cmd->name, "cvarlist") == 0) {
        cvar_t* cvar = sbox->cvars;
        while (cvar) {
            info(sbox, "%32s %s %s", cvar->name, cvar->string, cvar->desc);
            cvar = cvar->next;
        }
        return;
    }

    if (strcmp(cmd->name, "reset") == 0) {
        cvar_t* cvar = cvar_find(sbox, args[0]);
        if (!cvar)
            return;
        cvar_set(sbox, args[0], cvar->init);
        info(sbox, "reset cvar %s", cvar->name);
    }

    if (strcmp(cmd->name, "clear") == 0) {
        sbox->console.history_len = 0;
        return;
    }

    if (strcmp(cmd->name, "host") == 0) {
        sv_start(sbox, &sbox->server, NET_PORT);
        return;
    }

    if (strcmp(cmd->name, "connect") == 0) {
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
