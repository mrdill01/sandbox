#include "command.h"
#include "../shared/quark.h"
#include "console.h"
#include "../shared/net.h"

cmd_t help = {"help", "[cmd/cvar]", "Shows a help message for the console.", false};
cmd_t cmdlist = {"cmdlist", "", "Prints all commands to the console.", false};
cmd_t cvarlist = {"cvarlist", "", "Prints all cvars to the console.", false};
cmd_t reset = {"reset", "", "Resets a cvar to its default value.", false};
cmd_t clear = {"clear", "", "Clears the console history.", false};
cmd_t teleport = {"teleport", "<x> <y> <z>", "Teleports to a position.", true};
cmd_t bot = {"bot", "<spawn|kickall>", "Adds or removes bots from the game", true};
cmd_t host = {"host", "", "Hosts a new game.", false};
cmd_t connect_ = {"connect", "<ip> <port>", "Connects the client to a server.", false};
cmd_t disconnect = {"disconnect", "", "Disconnects from the server.", false};
cmd_t quit = {"quit", "", "Quits the game.", false};

void cmd_init(quark_t* quark) {
    cmd_register(quark, &help);
    cmd_register(quark, &cmdlist);
    cmd_register(quark, &cvarlist);
    cmd_register(quark, &reset);
    cmd_register(quark, &clear);
    cmd_register(quark, &teleport);
    cmd_register(quark, &bot);
    cmd_register(quark, &host);
    cmd_register(quark, &connect_);
    cmd_register(quark, &disconnect);
    cmd_register(quark, &quit);
}

void cmd_register(quark_t* quark, cmd_t* cmd) {
    info(quark, "register command %s", cmd->name);
    cmd->next = quark->cmds;
    quark->cmds = cmd;
}

cmd_t* cmd_find(quark_t* quark, const char* name) {
    cmd_t* cmd = quark->cmds;
    while (cmd) {
        if (strcmp(cmd->name, name) == 0)
            return cmd;
        cmd = cmd->next;
    }

    return NULL;
}

void cmd_run(quark_t* quark, const char* name, const char** args, int argc) {
    cmd_t* cmd = cmd_find(quark, name);
    if (!cmd) {
        error(quark, "command not found: %s", name);
        return;
    }

    if (cmd->is_cheat && !sv_cheats.value) {
        error(quark, "the %s command requires sv_cheats to be set to 1", cmd->name);
        return;
    }

    if (strcmp(cmd->name, "help") == 0) {
        if (argc == 1) {
            cvar_t* cvar = cvar_find(quark, args[0]);
            if (cvar) {
                info(quark, "%s\n(default value: %s)", cvar->desc, cvar->init);
                return;
            }

            cmd_t* cmd = cmd_find(quark, args[0]);
            if (cmd) {
                info(quark, cmd->desc);
                return;
            }

            error(quark, "command or cvar not found: %s", args[0]);
            return;
        }

        info(quark,
            "The console is used to run commands or set the values of cvars.");
        info(quark,
            "Type 'cvarlist' for a list of cvars and 'cmdlist' for a list of commands.");
        info(quark,
            "Press escape or F1 to close the console. Press enter to submit input.");
        return;
    }

    if (strcmp(cmd->name, "cmdlist") == 0) {
        cmd_t* cmd = quark->cmds;
        while (cmd) {
            info(quark, "%20s %16s %s", cmd->name, cmd->usage, cmd->desc);
            cmd = cmd->next;
        }
        return;
    }

    if (strcmp(cmd->name, "cvarlist") == 0) {
        cvar_t* cvar = quark->cvars;
        while (cvar) {
            info(quark, "%20s %s %s", cvar->name, cvar->string, cvar->desc);
            cvar = cvar->next;
        }
        return;
    }

    if (strcmp(cmd->name, "reset") == 0) {
        if (argc != 1) {
            cmd_show_usage(quark, cmd->name);
            return;
        }

        cvar_t* cvar = cvar_find(quark, args[0]);
        if (!cvar) {
            error(quark, "cvar not found: %s", args[0]);
            return;
        }
        cvar_set(quark, args[0], cvar->init);
        info(quark, "reset cvar %s", cvar->name);
    }

    if (strcmp(cmd->name, "clear") == 0) {
        quark->console.history_len = 0;
        return;
    }

    if (strcmp(cmd->name, "teleport") == 0) {
        if (argc != 3) {
            cmd_show_usage(quark, cmd->name);
            return;
        }

        float x = atof(args[0]);
        float y = atof(args[1]);
        float z = atof(args[2]);

        player_teleport(quark, quark->player, (vec3){x, y, z});
        return;
    }

    if (strcmp(cmd->name, "bot") == 0) {
        if (argc != 1) {
            cmd_show_usage(quark, cmd->name);
            return;
        }

        if (strcmp(args[0], "spawn") == 0) {
            player_t* bot = gm_spawn_player(quark, 1, true);
            player_teleport(quark, bot, quark->player->look_trace.point);
        
        } else if (strcmp(args[0], "kickall") == 0) {
            for (int i = 0; i < NET_MAX_PLAYERS; i++) {
                player_t* bot = quark->players[i];
                if (!bot) continue;
                if (bot->is_bot) {
                    player_free(quark, bot);
                    quark->players[i] = NULL;
                }
            }
        }
    }

    if (strcmp(cmd->name, "host") == 0) {
        sv_start(quark, &quark->server, NET_PORT);
        return;
    }

    if (strcmp(cmd->name, "connect") == 0) {
        if (argc != 2) {
            cmd_show_usage(quark, cmd->name);
            return;
        }

        cl_connect(quark, &quark->client, args[0], atoi(args[1]));
        
        quark->ui_state = UI_STATE_LOADING;
        ui_render(quark, &quark->renderer.ui, &quark->renderer);
        SDL_GL_SwapWindow(quark->window);

        map_load(quark, &quark->map);
        return;
    }

    if (strcmp(cmd->name, "disconnect") == 0) {
        cl_disconnect(quark, &quark->client);
        sv_stop(quark, &quark->server);
        
        map_free(quark, &quark->map);
        quark->player = NULL;
        for (int i = 0; i < NET_MAX_PLAYERS; i++) {
            if (!quark->players[i]) continue;
            player_free(quark, quark->players[i]);
		    quark->players[i] = NULL;
        }
        
        quark->ui_state = UI_STATE_MAIN_MENU;
        return;
    }

    if (strcmp(cmd->name, "quit") == 0) {
        quark->running = false;
        return;
    }
}

void cmd_show_usage(quark_t* quark, const char* name) {
    cmd_t* cmd = cmd_find(quark, name);
    if (!cmd) {
        error(quark, "command not found: %s", name);
        return;
    }

    info(quark, "usage: %s %s", cmd->name, cmd->usage);
}
