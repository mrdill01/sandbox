#include "sbox.h"
#include "render.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "../include/gl.h"

bool init(sbox_t* sbox);
void tick(sbox_t* sbox);
void shutdown(sbox_t* sbox);

int main(int argc, char* argv[]) {
    sbox_t sbox;
    sbox_init(&sbox);

    if (!init(&sbox))
        return EXIT_FAILURE;

    map_load(&sbox, &sbox.map);

    while (sbox.running) {
        tick(&sbox);
        r_render(&sbox, &sbox.renderer);
    }

    shutdown(&sbox);

    return EXIT_SUCCESS;
}

bool init(sbox_t* sbox) {
    info(sbox, "initializing SDL...");

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        error(sbox, "failed to initialize SDL: %s", SDL_GetError());
        return false;
    }

    info(sbox, "SDL initialized!");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, R_GL_MAJ);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, R_GL_MIN);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    info(sbox, "creating window...");

    sbox->window = SDL_CreateWindow("sandbox",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        r_width.value,
        r_height.value,
        SDL_WINDOW_OPENGL);

    if (!sbox->window) {
        error(sbox, "failed to open window: %s", SDL_GetError());
        return false;
    }

    info(sbox, "window created!");
    info(sbox, "setting up OpenGL context...");

    sbox->gl_context = SDL_GL_CreateContext(sbox->window);

    if (!sbox->gl_context) {
        error(sbox, "failed to setup OpenGL context: %s", SDL_GetError());
        return false;
    }

    info(sbox, "created OpenGL context!");
    info(sbox, "loading OpenGL functions...");

    if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress)) {
        error(sbox, "failed to initialize GLAD");
        return false;
    }

    info(sbox, "OpenGL loaded!");
    info(sbox, "GPU: %s", glGetString(GL_RENDERER));
    info(sbox, "vendor: %s", glGetString(GL_VENDOR));
    info(sbox, "version: %s", glGetString(GL_VERSION));

    r_init(sbox, &sbox->renderer);
    a_init(sbox, &sbox->audio);

    return true;
}

void tick(sbox_t* sbox) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT: sbox->running = false; break;
        case SDL_WINDOWEVENT: {
            switch (e.window.event) {
            case SDL_WINDOWEVENT_RESIZED: {
                cvar_set_value(sbox, "r_width", e.window.data1);
                cvar_set_value(sbox, "r_height", e.window.data2);
                r_on_resize(sbox);
                break;
            }
            }
            break;
        }
        case SDL_KEYDOWN: {
            sbox->keys[e.key.keysym.scancode] = true;
            break;
        }
        case SDL_KEYUP: {
            sbox->keys[e.key.keysym.scancode] = false;
            break;
        }
        case SDL_MOUSEMOTION: {
            sbox->mxdt = e.motion.xrel;
            sbox->mydt = e.motion.yrel;
            break;
        }
        }
    }

    sbox_tick(sbox);

    if (sbox->keys[SDL_SCANCODE_ESCAPE])
        sbox->running = false;

    if (sbox->keys[SDL_SCANCODE_F2]) {
        sbox->keys[SDL_SCANCODE_F2] = false;
        sbox_reload_resources(sbox);
    }

    if (sbox->keys[SDL_SCANCODE_F4]) {
        sbox->keys[SDL_SCANCODE_F4] = false;
        cvar_toggle(sbox, "r_debug_draw_colliders");
    }
}

void shutdown(sbox_t* sbox) {
    info(sbox, "shutting down...");

    r_free(sbox, &sbox->renderer);
    a_free(sbox, &sbox->audio);
    SDL_GL_DeleteContext(sbox->gl_context);
    SDL_DestroyWindow(sbox->window);
    SDL_Quit();

    sbox_free(sbox);
}
