#include "console.h"
#include "sbox.h"

void con_init(sbox_t* sbox, console_t* con) {
    con->input[0] = '\0';
    for (int i = 0; i < CON_MAX_HISTORY; i++)
        con->history[i] = NULL;
    con->history_len = 0;
}

void con_free(sbox_t* sbox, console_t* con) {
    free(con->input);
    for (int i = 0; i < con->history_len; i++)
        free(con->history[i]);
}

void con_open(sbox_t* sbox, console_t* con) {
    cvar_set(sbox, "console", "1");
    SDL_StartTextInput();
}

void con_close(sbox_t* sbox, console_t* con) {
    cvar_set(sbox, "console", "0");
    SDL_StopTextInput();
}

void con_submit(sbox_t* sbox, console_t* con) {
    info(sbox, "> %s", con->input);
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
    if ((cvar = cvar_find(sbox, name))) {
        if (argc == 1) {
            cvar_set(sbox, name, args[0]);
        } else {
            info(sbox, "%s = %s", cvar->name, cvar->string);
        }

        con->input[0] = '\0';
        return;
    }

    cmd_t* cmd = NULL;
    if ((cmd = cmd_find(sbox, name))) {
        cmd_run(sbox, name, args, argc);
        con->input[0] = '\0';
        return;
    }

    info(sbox, "command or cvar not found: %s", name);
    con->input[0] = '\0';
}

void con_add_history(sbox_t* sbox, console_t* con, const char* text) {
    char* owned = malloc(strlen(text) + 1);
    strcpy(owned, text);
    con->history[con->history_len++] = owned;
}
