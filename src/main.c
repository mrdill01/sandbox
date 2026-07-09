#include "sbox.h"
#include "render.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "../include/gl.h"

int init(sbox_t* sbox);
void tick(sbox_t* sbox);
void shutdown(sbox_t* sbox);

int main(int argc, char* argv[]) {
    sbox_t sbox;
    sbox_init(&sbox);

    if (init(&sbox) == -1)
        return EXIT_FAILURE;

    map_load(&sbox, &sbox.map);

    while (sbox.running) {
        tick(&sbox);
        r_render(&sbox, &sbox.renderer);
    }

    shutdown(&sbox);

    return EXIT_SUCCESS;
}

int init(sbox_t* sbox) {
    info(sbox, "%s", SANDBOX_VERSION);

    time_t current_time = time(NULL);
	struct tm* local_time = localtime(&current_time);
	char* time_string = asctime(local_time);
	time_string[strlen(time_string) - 1] = '\0';
    info(sbox, "current date and time: %s", time_string);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        error(sbox, "failed to initialize SDL: %s", SDL_GetError());
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    sbox->window = SDL_CreateWindow("sandbox",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        r_width.value,
        r_height.value,
        SDL_WINDOW_OPENGL);

    if (!sbox->window) {
        error(sbox, "failed to open window: %s", SDL_GetError());
        return -1;
    }

    sbox->gl_context = SDL_GL_CreateContext(sbox->window);

    if (!sbox->gl_context) {
        error(sbox, "failed to setup OpenGL context: %s", SDL_GetError());
        return -1;
    }

    if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress)) {
        error(sbox, "failed to initialize GLAD");
        return -1;
    }

    r_init(sbox, &sbox->renderer);

    info(sbox, "GPU: %s", glGetString(GL_RENDERER));
    info(sbox, "vendor: %s", glGetString(GL_VENDOR));
    info(sbox, "version: %s", glGetString(GL_VERSION));
    return 0;
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
                r_on_resize(sbox, &sbox->renderer, e.window.data1, e.window.data2);
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

    if (sbox->keys[SDL_SCANCODE_ESCAPE]) {
        sbox->running = false;
    }
}

void shutdown(sbox_t* sbox) {
    info(sbox, "shutting down...");

    r_free(sbox, &sbox->renderer);
    SDL_GL_DeleteContext(sbox->gl_context);
    SDL_DestroyWindow(sbox->window);

    info(sbox, "shutdown complete. exiting...");
    sbox_free(sbox);
}
