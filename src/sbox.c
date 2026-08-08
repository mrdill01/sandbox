#include "sbox.h"
#include "net.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>

#define MAX_MSG_LEN 2048

cvar_t r_width = {"r_width", "960.0f", true, "Renderer width."};
cvar_t r_height = {"r_height", "540.0f", true, "Renderer height."};
cvar_t r_scale = {"r_scale", "0.35f", true, "Resolution scaling."};
cvar_t r_fullscreen = {"r_fullscreen", "0", true, "Fullscreen."};
cvar_t r_vsync = {"r_vsync", "0", true, "Vertical sync."};
cvar_t r_fov = {"r_fov", "75.0f", true, "Field-of-view."};
cvar_t r_shadows = {"r_shadows", "1", true, "Enable shadows."};
cvar_t r_shadow_res = {"r_shadow_res", "1024.0", true, "Shadow resolution."};
cvar_t r_third_person = {"r_third_person", "0", true, "Enable third-person camera."};
cvar_t r_viewmodel = {"r_viewmodel", "1", true, "Enables or disables rendering of the viewmodel."};
cvar_t r_debug_menu = {"r_debug_menu", "1", true, "Debug menu."};
cvar_t r_debug_draw_colliders = {"r_debug_draw_colliders", "0", true, "Draw colliders."};
cvar_t r_debug_draw_bullets = {"r_debug_draw_bullets", "1", true, "Draw bullet traces."};
cvar_t r_debug_buffer = {"r_debug_buffer", "0", true,
	"0 = Disabled, 1 = Position, 2 = Albedo, 3 = Roughness, 4 = Normals, 5 = Depth."};
cvar_t a_device = {"a_device", "(null)", true, "Audio output device (default (null))."};
cvar_t a_volume = {"a_volume", "0.2f", true, "Audio volume."};
cvar_t m_sens = {"m_sens", "5.0f", true, "Mouse sensitivity."};
cvar_t console = {"console", "0", true, "Show the developer console."};
cvar_t noclip = {"noclip", "0", true, "Enables flight / disables collision."};
cvar_t sv_timescale = {"sv_timescale", "1.0f", true, "Set to values less than 1.0 for slow-motion."};
cvar_t sv_respawn_time = {"sv_respawn_time", "3.0f", true, "How long for players to respawn."};
cvar_t sv_destruction = {"sv_destruction", "0", true,
	"Allow the map to be destroyed by explosions."};
cvar_t sv_random_seed = {"sv_random_seed", "12345", true, "Random seed value."};
cvar_t cl_name = {"cl_name", "Player", true, "Display name."};
cvar_t edit_mode = {"edit_mode", "0", true, "Enable edit mode."};
cvar_t edit_snap_size = {"edit_snap_size", "0.2f", true, "Edit mode snap size."};

void sbox_init(sbox_t* sbox) {
	con_init(sbox, &sbox->console);
	info(sbox, "%s", SBOX_VERSION);

	#ifdef SBOX_DEBUG
	info(sbox, "DEBUG BUILD (undefine SBOX_DEBUG for release)");
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
    cvar_register(sbox, &r_shadows, NULL);
    cvar_register(sbox, &r_shadow_res, NULL);
    cvar_register(sbox, &r_third_person, NULL);
    cvar_register(sbox, &r_viewmodel, NULL);
    cvar_register(sbox, &r_debug_menu, NULL);
    cvar_register(sbox, &r_debug_draw_colliders, NULL);
    cvar_register(sbox, &r_debug_buffer, NULL);
    cvar_register(sbox, &a_device, NULL);
    cvar_register(sbox, &a_volume, NULL);
    cvar_register(sbox, &m_sens, NULL);
    cvar_register(sbox, &console, NULL);
    cvar_register(sbox, &noclip, NULL);
    cvar_register(sbox, &sv_timescale, NULL);
    cvar_register(sbox, &sv_respawn_time, NULL);
    cvar_register(sbox, &sv_destruction, NULL);
    cvar_register(sbox, &sv_random_seed, NULL);
    cvar_register(sbox, &cl_name, NULL);
    cvar_register(sbox, &edit_mode, NULL);
    cvar_register(sbox, &edit_snap_size, NULL);

	sbox->cmds = NULL;
	cmd_init(sbox);

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

	net_init(sbox);
	sv_init(sbox, &sbox->server);
	cl_init(sbox, &sbox->client);

	sbox->ui_state = UI_STATE_MAIN_MENU;

	for (int i = 0; i < MAX_PLAYERS; i++)
		sbox->players[i] = NULL;
}

void sbox_free(sbox_t* sbox) {
    map_free(sbox, &sbox->map);
	con_free(sbox, &sbox->console);
	cl_disconnect(sbox, &sbox->client);
	sv_stop(sbox, &sbox->server);
	net_free(sbox);
}

void sbox_tick(sbox_t* sbox) {
	sbox->last = sbox->now;
	sbox->now = SDL_GetPerformanceCounter();
   	sbox->dt = (sbox->now - sbox->last) / (double)SDL_GetPerformanceFrequency() * sv_timescale.value;
	sbox->time += sbox->dt;

	sv_tick(sbox, &sbox->server);
	cl_tick(sbox, &sbox->client);

	a_tick(sbox, &sbox->audio, sbox->player, &sbox->renderer.camera);
	
	for (int i = 0; i < MAX_PLAYERS; i++) {
		player_t* player = sbox->players[i];
		if (!player) continue;
		player_input(sbox, sbox->player);
		player_tick(sbox, sbox->players[i], &sbox->renderer.camera, &sbox->map.entlist);
	}

	map_tick(sbox, &sbox->map);
	r_tick(sbox, &sbox->renderer);
}

void sbox_reload_resources(sbox_t* sbox) {
	info(sbox, "reloading resources...");

	sbox->renderer.gbuffer_shader = shader_load(sbox,
        "gbuffer", "res/shaders/gbuffer.vs", "res/shaders/gbuffer.fs");

	sbox->renderer.screen_shader = shader_load(sbox,
        "screen", "res/shaders/screen.vs", "res/shaders/screen.fs");

	sbox->renderer.forward_shader = shader_load(sbox,
        "forward", "res/shaders/forward.vs", "res/shaders/forward.fs");

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
	size_t len = vsnprintf(buffer, MAX_MSG_LEN, msg, args);
	buffer[len] = '\0';
	va_end(args);

	char text[MAX_MSG_LEN + 32];
	sprintf(text, "%s", buffer);

	puts(text);
	con_add_history(sbox, &sbox->console, text);
}

void error(sbox_t* sbox, const char* msg, ...) {
    char buffer[MAX_MSG_LEN];
	va_list args;
	va_start(args, msg);
	size_t len = vsnprintf(buffer, MAX_MSG_LEN, msg, args);
	buffer[len] = '\0';
	va_end(args);

	char text[MAX_MSG_LEN + 32];
	sprintf(text, "error: %s", buffer);

	puts(text);
	con_add_history(sbox, &sbox->console, text);

	#ifdef SBOX_DEBUG
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
