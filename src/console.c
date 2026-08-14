#include "console.h"
#include "quark.h"

void con_init(quark_t* quark, console_t* con) {
    con->input[0] = '\0';
    for (int i = 0; i < CON_MAX_HISTORY; i++)
        con->history[i] = NULL;
    con->history_len = 0;
}

void con_free(quark_t* quark, console_t* con) {
    free(con->input);
    for (int i = 0; i < con->history_len; i++)
        free(con->history[i]);
}

void con_open(quark_t* quark, console_t* con) {
    cvar_set(quark, "console", "1");
    SDL_StartTextInput();
}

void con_close(quark_t* quark, console_t* con) {
    cvar_set(quark, "console", "0");
    SDL_StopTextInput();
}

void con_submit(quark_t* quark, console_t* con) {
    info(quark, "> %s", con->input);
    char* stream = malloc(strlen(con->input) + 1);
    strcpy(stream, con->input);

    const char* delim = " ";
    char* name = strtok(stream, delim);
    if (!name) {
        con->input[0] = '\0';
        return;
    }
    
    const char* args[16];
    size_t argc = 0;

    char* arg;
    while ((arg = strtok(NULL, delim))) {
        args[argc++] = arg;
    }

    cvar_t* cvar = NULL;
    if ((cvar = cvar_find(quark, name))) {
        if (argc == 1) {
            cvar_set(quark, name, args[0]);
        } else {
            info(quark, "%s = %s", cvar->name, cvar->string);
        }

        con->input[0] = '\0';
        return;
    }

    cmd_t* cmd = NULL;
    if ((cmd = cmd_find(quark, name))) {
        cmd_run(quark, name, args, argc);
        con->input[0] = '\0';
        return;
    }

    info(quark, "command or cvar not found: %s", name);
    con->input[0] = '\0';
}

void con_add_history(quark_t* quark, console_t* con, const char* text) {
    char* owned = malloc(strlen(text) + 1);
    strcpy(owned, text);
    con->history[con->history_len++] = owned;
}
