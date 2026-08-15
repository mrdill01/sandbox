#include "quark.h"
#include "render.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "../../include/gl.h"

bool init(quark_t* quark);
void tick(quark_t* quark);
void quit_game(quark_t* quark);

int main(int argc, char* argv[]) {
    quark_t quark;
    if (!init(&quark))
        return EXIT_FAILURE;

    SDL_StopTextInput();

    #ifdef QUARK_DEBUG
    cmd_run(&quark, "host", NULL, 0);
    const char* args[] = {"127.0.0.1", "25565"};
    cmd_run(&quark, "connect", args, 2);

    ui_render(&quark, &quark.renderer.ui, &quark.renderer);
    SDL_GL_SwapWindow(quark.window);
    #endif

    while (quark.running) {
        tick(&quark);
        r_render(&quark, &quark.renderer);
    }

    quit_game(&quark);
    return EXIT_SUCCESS;
}

bool init(quark_t* quark) {
    quark_init(quark);

    info(quark, "initializing SDL...");

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        error(quark, "failed to initialize SDL: %s", SDL_GetError());
        return false;
    }

    info(quark, "SDL initialized!");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, R_GL_MAJ);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, R_GL_MIN);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    info(quark, "creating window...");

    quark->window = SDL_CreateWindow("quark",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        r_width.value,
        r_height.value,
        SDL_WINDOW_OPENGL);

    if (!quark->window) {
        error(quark, "failed to open window: %s", SDL_GetError());
        return false;
    }

    info(quark, "window created!");
    info(quark, "setting up OpenGL context...");

    quark->gl_context = SDL_GL_CreateContext(quark->window);

    if (!quark->gl_context) {
        error(quark, "failed to setup OpenGL context: %s", SDL_GetError());
        return false;
    }

    info(quark, "created OpenGL context!");
    info(quark, "loading OpenGL functions...");

    if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress)) {
        error(quark, "failed to initialize GLAD");
        return false;
    }

    info(quark, "OpenGL loaded!");
    info(quark, "GPU: %s", glGetString(GL_RENDERER));
    info(quark, "vendor: %s", glGetString(GL_VENDOR));
    info(quark, "version: %s", glGetString(GL_VERSION));

    r_init(quark, &quark->renderer);
    a_init(quark, &quark->audio);
    map_init(quark, &quark->map);
    return true;
}

void tick(quark_t* quark) {
    for (int i = 0; i < NUM_BUTTONS; i++)
        quark->prev_buttons[i] = quark->buttons[i];
    quark->mxdt = 0.0f;
    quark->mydt = 0.0f;

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT: quark->running = false; break;
        case SDL_WINDOWEVENT: {
            switch (e.window.event) {
            case SDL_WINDOWEVENT_RESIZED: {
                cvar_set_value(quark, "r_width", e.window.data1);
                cvar_set_value(quark, "r_height", e.window.data2);
                r_on_resize(quark);
                break;
            }
            }
            break;
        }
        case SDL_KEYDOWN: {
            quark->keys[e.key.keysym.scancode] = true;
            break;
        }
        case SDL_KEYUP: {
            quark->keys[e.key.keysym.scancode] = false;
            break;
        }
        case SDL_MOUSEMOTION: {
            quark->mx = e.motion.x;
            quark->my = e.motion.y;
            quark->mxdt = e.motion.xrel;
            quark->mydt = e.motion.yrel;
            break;
        }
        case SDL_MOUSEBUTTONDOWN: {
            quark->buttons[e.button.button] = true;
            break;
        }
        case SDL_MOUSEBUTTONUP: {
            quark->buttons[e.button.button] = false;
            break;
        }
        case SDL_TEXTINPUT: {
            strcat(quark->console.input, e.text.text);
            break;
        }
        }
    }

    quark_tick(quark);

    if (quark->keys[SDL_SCANCODE_ESCAPE]) {
        quark->keys[SDL_SCANCODE_ESCAPE] = false;
        if (console.value) con_close(quark, &quark->console);
        else if (quark->ui_state == UI_STATE_IN_GAME ||
            quark->ui_state == UI_STATE_DEAD) quark->ui_state = UI_STATE_PAUSE_MENU;
        else quark->ui_state = UI_STATE_IN_GAME;
    }

    if (quark->keys[SDL_SCANCODE_F1]) {
        quark->keys[SDL_SCANCODE_F1] = false;
        if (console.value)
            con_close(quark, &quark->console);
        else
            con_open(quark, &quark->console);
    }

    if (console.value) {
        if (quark->keys[SDL_SCANCODE_RETURN]) {
            quark->keys[SDL_SCANCODE_RETURN] = false;
            con_submit(quark, &quark->console);
        }

        if (quark->keys[SDL_SCANCODE_UP]) {
            quark->keys[SDL_SCANCODE_UP] = false;
            if (quark->console.scroll > -353)
                quark->console.scroll -= 1;
        }

        if (quark->keys[SDL_SCANCODE_DOWN]) {
            quark->keys[SDL_SCANCODE_DOWN] = false;
            if (quark->console.scroll < quark->console.history_len - 1 - 20)
                quark->console.scroll += 1;
        }

        if (quark->keys[SDL_SCANCODE_BACKSPACE]) {
            quark->keys[SDL_SCANCODE_BACKSPACE] = false;
            quark->console.input[strlen(quark->console.input) - 1] = '\0';
        }
    }

    if (quark->keys[SDL_SCANCODE_F2]) {
        quark->keys[SDL_SCANCODE_F2] = false;
        quark_reload_resources(quark);
    }

    if (quark->keys[SDL_SCANCODE_F3]) {
        quark->keys[SDL_SCANCODE_F3] = false;
        cvar_toggle(quark, "r_debug_menu");
    }

    if (quark->keys[SDL_SCANCODE_F4]) {
        quark->keys[SDL_SCANCODE_F4] = false;
        cvar_toggle(quark, "r_debug_colliders");
    }

    if (quark->keys[SDL_SCANCODE_F5]) {
        quark->keys[SDL_SCANCODE_F5] = false;
        if (r_debug_buffer.value == 0) cvar_set(quark, "r_debug_buffer", "1");
        else if (r_debug_buffer.value == 1) cvar_set(quark, "r_debug_buffer", "2");
        else if (r_debug_buffer.value == 2) cvar_set(quark, "r_debug_buffer", "3");
        else if (r_debug_buffer.value == 3) cvar_set(quark, "r_debug_buffer", "4");
        else if (r_debug_buffer.value == 4) cvar_set(quark, "r_debug_buffer", "5");
        else if (r_debug_buffer.value == 5) cvar_set(quark, "r_debug_buffer", "0");
    }

    if (quark->keys[SDL_SCANCODE_F11] ||
        (quark->keys[SDL_SCANCODE_LALT] && quark->keys[SDL_SCANCODE_RETURN]))
    {
        quark->keys[SDL_SCANCODE_F11] = false;
        quark->keys[SDL_SCANCODE_RETURN] = false;
        cvar_toggle(quark, "r_fullscreen");
    }
}

void quit_game(quark_t* quark) {
    info(quark, "shutting down...");

    r_free(quark, &quark->renderer);
    a_free(quark, &quark->audio);
    for (int i = 0; i < NET_MAX_PLAYERS; i++) {
        if (!quark->players[i]) continue;
        player_free(quark, quark->players[i]);
    }
    
    SDL_GL_DeleteContext(quark->gl_context);
    SDL_DestroyWindow(quark->window);
    SDL_Quit();

    quark_free(quark);
}
