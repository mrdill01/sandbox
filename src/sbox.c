#include "sbox.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>

#define MAX_MSG_LEN 2048

cvar_t r_width = {"r_width", "960.0f", true};
cvar_t r_height = {"r_height", "540.0f", true};
cvar_t r_scale = {"r_scale", "1.0f", true};
cvar_t r_fullscreen = {"r_fullscreen", "0", true};
cvar_t r_vsync = {"r_vsync", "0", true};
cvar_t r_fov = {"r_fov", "75.0f", true};
cvar_t r_shadow_res = {"r_shadow_res", "1024.0", true};
cvar_t r_debug_draw_colliders = {"r_debug_draw_colliders", "0", true};
cvar_t a_device = {"a_device", "(null)", true};
cvar_t a_volume = {"a_volume", "0.5f", true};
cvar_t m_sens = {"m_sens", "5.0f", true};
cvar_t edit_mode = {"edit_mode", "1.0f", true};
cvar_t edit_snap_size = {"edit_snap_size", "0.2f", true};

void sbox_init(sbox_t* sbox) {
	info(sbox, "%s", SANDBOX_VERSION);

	#ifdef SANDBOX_DEBUG
	info(sbox, "DEBUG BUILD (undefine SANDBOX_DEBUG for release)");
	#endif

    time_t current_time = time(NULL);
	struct tm* local_time = localtime(&current_time);
	char* time_string = asctime(local_time);
	time_string[strlen(time_string) - 1] = '\0';
    info(sbox, "current date and time: %s", time_string);
	
    sbox->cvars = NULL;
    cvar_register(sbox, &r_width, NULL);
    cvar_register(sbox, &r_height, NULL);
    cvar_register(sbox, &r_scale, NULL);
    cvar_register(sbox, &r_fullscreen, r_on_toggle_fullscreen);
    cvar_register(sbox, &r_vsync, NULL);
    cvar_register(sbox, &r_fov, NULL);
    cvar_register(sbox, &r_shadow_res, NULL);
    cvar_register(sbox, &r_debug_draw_colliders, NULL);
    cvar_register(sbox, &a_device, NULL);
    cvar_register(sbox, &a_volume, NULL);
    cvar_register(sbox, &m_sens, NULL);
    cvar_register(sbox, &edit_mode, NULL);
    cvar_register(sbox, &edit_snap_size, NULL);

	cfg_write(sbox, DEFAULT_CFG_PATH);

	sbox->running = true;
	sbox->now = SDL_GetPerformanceCounter();
	sbox->last = 0;
	sbox->dt = 0.0;
	sbox->time = 0.0;

	for (int i = 0; i < NUM_KEYS; i++)
		sbox->keys[i] = false;
	for (int i = 0; i < NUM_BUTTONS; i++) {
		sbox->buttons[i] = false;
		sbox->prev_buttons[i] = false;
	}
	sbox->mx = 0.0f;
	sbox->my = 0.0f;
	sbox->mxdt = 0.0f;
	sbox->mydt = 0.0f;

	sbox->window = NULL;
	sbox->gl_context = NULL;
	sbox->shaders = NULL;
	sbox->meshes = NULL;
	sbox->textures = NULL;
	sbox->materials = NULL;

	sbox->ui_state = UI_STATE_IN_GAME;

    player_init(sbox, &sbox->player);
}

void sbox_free(sbox_t* sbox) {
    map_free(sbox, &sbox->map);
}

void sbox_tick(sbox_t* sbox) {
	sbox->last = sbox->now;
	sbox->now = SDL_GetPerformanceCounter();
   	sbox->dt = (sbox->now - sbox->last) / (double)SDL_GetPerformanceFrequency();
	sbox->time += sbox->dt;

	a_tick(sbox, &sbox->audio, &sbox->player, &sbox->renderer.camera);
    player_tick(sbox, &sbox->player, &sbox->renderer.camera, &sbox->map.entlist);
    map_tick(sbox, &sbox->map);
	r_tick(sbox, &sbox->renderer);
}

void sbox_reload_resources(sbox_t* sbox) {
	info(sbox, "reloading resources...");

	free(sbox->renderer.screen_shader);
	sbox->renderer.screen_shader = shader_load(sbox,
        "screen", "res/shaders/screen.vs", "res/shaders/screen.fs");

	free(sbox->renderer.screen_shader);
	sbox->renderer.forward_shader = shader_load(sbox,
        "forward", "res/shaders/forward.vs", "res/shaders/forward.fs");

	free(sbox->renderer.skybox_shader);
	sbox->renderer.skybox_shader = shader_load(sbox,
        "skybox", "res/shaders/skybox.vs", "res/shaders/skybox.fs");
	
	map_free(sbox, &sbox->map);
	map_load(sbox, &sbox->map);

	info(sbox, "resources reloaded!");
}

void info(sbox_t* sbox, const char* msg, ...) {
    char buffer[MAX_MSG_LEN];
	va_list args;
	va_start(args, msg);
	vsnprintf(buffer, MAX_MSG_LEN, msg, args);
	va_end(args);

    printf("%s\n", buffer);
}

void error(sbox_t* sbox, const char* msg, ...) {
    char buffer[MAX_MSG_LEN];
	va_list args;
	va_start(args, msg);
	vsnprintf(buffer, MAX_MSG_LEN, msg, args);
	va_end(args);

    printf("error: %s\n", buffer);

	#ifdef SANDBOX_DEBUG
	exit(EXIT_FAILURE);
	#endif
}

char* load_file(sbox_t* sbox, const char* path) {
	FILE* fp = fopen(path, "r");
	if (!fp) error(sbox, "failed to open %s for reading", path);
	
	fseek(fp, 0, SEEK_END);
	size_t len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
    char* text = malloc(len + 1);
	fread(text, 1, len + 1, fp);
	fclose(fp);
	text[len] = '\0';
	return text;
}

void clear_file(sbox_t* sbox, const char* path) {
	FILE* fp = fopen(path, "w");
	if (!fp) error(sbox, "failed to clear file %s", path);
	fclose(fp);
}
