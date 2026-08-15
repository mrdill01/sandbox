#include "quark.h"
#include "../shared/net.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>

#define MAX_MSG_LEN 2048

cvar_t sv_cheats = {"sv_cheats", "1", true, true, "Enables cheat commands/cvars."};
cvar_t sv_round_time = {"sv_round_time", "600", true, false,
	"Determines how long a round lasts for."};
cvar_t sv_timescale = {"sv_timescale", "1.0f", true, true,
	"Set to values less than 1.0 for slow-motion."};
cvar_t sv_respawn_time = {"sv_respawn_time", "3.0f", true, false, "How long for players to respawn."};
cvar_t sv_destruction = {"sv_destruction", "1", false, true,
	"Allow the map to be destroyed by explosions."};
cvar_t sv_random_seed = {"sv_random_seed", "12345", true, false, "Random seed value."};
cvar_t r_width = {"r_width", "960.0f", true, false, "Renderer width."};
cvar_t r_height = {"r_height", "540.0f", true, false, "Renderer height."};
cvar_t r_scale = {"r_scale", "0.45f", true, false, "Resolution scaling."};
cvar_t r_fullscreen = {"r_fullscreen", "0", true, false, "Fullscreen."};
cvar_t r_vsync = {"r_vsync", "0", true, false, "Vertical sync."};
cvar_t r_fov = {"r_fov", "100.0f", true, false, "Field-of-view."};
cvar_t r_shadows = {"r_shadows", "1", true, false, "Enable shadows."};
cvar_t r_shadow_res = {"r_shadow_res", "1024.0", true, false, "Shadow resolution."};
cvar_t r_third_person = {"r_third_person", "0", true, false, "Enable third-person camera."};
cvar_t r_viewmodel = {"r_viewmodel", "1", true, false,
	"Enables or disables rendering of the viewmodel."};
cvar_t r_hud = {"r_hud", "1", true, true, "Enables or disables rendering of the heads-up-display."};
cvar_t r_debug_menu = {"r_debug_menu", "1", true, false, "Debug menu."};
cvar_t r_debug_colliders = {"r_debug_colliders", "0", true, true,
	"Draw colliders."};
cvar_t r_debug_bullets = {"r_debug_bullets", "0", true, true,
	"Draw bullet traces."};
cvar_t r_debug_players = {"r_debug_players", "1", true, true,
	"Draw player bounding boxes."};
cvar_t r_debug_buffer = {"r_debug_buffer", "0", true, false,
	"0 = Disabled, 1 = Position, 2 = Albedo, 3 = Roughness, 4 = Normals, 5 = Depth."};
cvar_t a_device = {"a_device", "(null)", true, false, "Audio output device (default (null))."};
cvar_t a_volume = {"a_volume", "0.2f", true, false, "Audio volume."};
cvar_t m_sens = {"m_sens", "10.0f", true, false, "Mouse sensitivity."};
cvar_t console = {"console", "0", true, false, "Show the developer console."};
cvar_t profiler = {"profiler", "0", true, false, "Shows the performance profiler."};
cvar_t noclip = {"noclip", "0", true, true, "Enables flight / disables collision."};
cvar_t cl_name = {"cl_name", "Player", true, false, "Display name."};
cvar_t edit_mode = {"edit_mode", "0", true, true, "Enable edit mode."};
cvar_t edit_snap_size = {"edit_snap_size", "0.2f", true, false, "Edit mode snap size."};

void quark_init(quark_t* quark) {
	con_init(quark, &quark->console);
	info(quark, "%s", QUARK_VERSION);

	#ifdef QUARK_DEBUG
	info(quark, "DEBUG BUILD (undefine SBOX_DEBUG for release)");
	#endif

    time_t current_time = time(NULL);
	struct tm* local_time = localtime(&current_time);
	char* time_string = asctime(local_time);
	time_string[strlen(time_string) - 1] = '\0';
    info(quark, "current date and time: %s", time_string);
	
    quark->cvars = NULL;
	cvar_register(quark, &sv_cheats, NULL);
    cvar_register(quark, &sv_timescale, NULL);
    cvar_register(quark, &sv_respawn_time, NULL);
    cvar_register(quark, &sv_destruction, NULL);
    cvar_register(quark, &sv_random_seed, NULL);
    cvar_register(quark, &cl_name, NULL);
    cvar_register(quark, &r_width, NULL);
    cvar_register(quark, &r_height, NULL);
    cvar_register(quark, &r_scale, NULL);
    cvar_register(quark, &r_fullscreen, r_on_toggle_fullscreen);
    cvar_register(quark, &r_vsync, NULL);
    cvar_register(quark, &r_fov, NULL);
    cvar_register(quark, &r_shadows, NULL);
    cvar_register(quark, &r_shadow_res, NULL);
    cvar_register(quark, &r_third_person, NULL);
    cvar_register(quark, &r_viewmodel, NULL);
    cvar_register(quark, &r_hud, NULL);
    cvar_register(quark, &r_debug_menu, NULL);
    cvar_register(quark, &r_debug_colliders, NULL);
    cvar_register(quark, &r_debug_bullets, NULL);
    cvar_register(quark, &r_debug_players, NULL);
    cvar_register(quark, &r_debug_buffer, NULL);
    cvar_register(quark, &a_device, NULL);
    cvar_register(quark, &a_volume, NULL);
    cvar_register(quark, &m_sens, NULL);
    cvar_register(quark, &console, NULL);
    cvar_register(quark, &profiler, NULL);
    cvar_register(quark, &noclip, NULL);
    cvar_register(quark, &edit_mode, NULL);
    cvar_register(quark, &edit_snap_size, NULL);

	quark->cmds = NULL;
	cmd_init(quark);

	cfg_write(quark, DEFAULT_CFG_PATH);

	quark->running = true;
	quark->now = SDL_GetPerformanceCounter();
	quark->last = 0;
	quark->dt = 0.0;
	quark->time = 0.0;

	prof_init(quark, &quark->prof);

	for (int i = 0; i < NUM_KEYS; i++)
		quark->keys[i] = false;
	
	for (int i = 0; i < NUM_BUTTONS; i++) {
		quark->buttons[i] = false;
		quark->prev_buttons[i] = false;
	}

	quark->mx = 0.0f;
	quark->my = 0.0f;
	quark->mxdt = 0.0f;
	quark->mydt = 0.0f;

	quark->window = NULL;
	quark->gl_context = NULL;
	quark->shaders = NULL;
	quark->meshes = NULL;
	quark->textures = NULL;
	quark->materials = NULL;

	net_init(quark);
	sv_init(quark, &quark->server);
	cl_init(quark, &quark->client);

	quark->ui_state = UI_STATE_MAIN_MENU;
	gm_start(quark, &quark->gm, GAME_MODE_CTF);

	for (int i = 0; i < NET_MAX_PLAYERS; i++)
		quark->players[i] = NULL;
}

void quark_free(quark_t* quark) {
	cl_disconnect(quark, &quark->client);
	sv_stop(quark, &quark->server);
    map_free(quark, &quark->map);
	con_free(quark, &quark->console);
	net_free(quark);
}

void quark_tick(quark_t* quark) {
	quark->last = quark->now;
	quark->now = SDL_GetPerformanceCounter();
   	quark->dt = (quark->now - quark->last) / (double)SDL_GetPerformanceFrequency() * sv_timescale.value;
	quark->time += quark->dt;

	sv_tick(quark, &quark->server);
	cl_tick(quark, &quark->client);
	gm_tick(quark, &quark->gm);

	a_tick(quark, &quark->audio, quark->player, &quark->renderer.camera);
	
	for (int i = 0; i < NET_MAX_PLAYERS; i++) {
		player_t* player = quark->players[i];
		if (!player) continue;
		player_input(quark, player);
		player_tick(quark, quark->players[i], &quark->renderer.camera, &quark->map.entlist);
	}

	map_tick(quark, &quark->map);

	prof_tick(quark, &quark->prof);
	r_tick(quark, &quark->renderer);
}

void quark_reload_resources(quark_t* quark) {
	info(quark, "reloading resources...");

	quark->renderer.gbuffer_shader = shader_load(quark,
        "gbuffer", "res/shaders/gbuffer.vs", "res/shaders/gbuffer.fs");

	quark->renderer.screen_shader = shader_load(quark,
        "screen", "res/shaders/screen.vs", "res/shaders/screen.fs");

	quark->renderer.forward_shader = shader_load(quark,
        "forward", "res/shaders/forward.vs", "res/shaders/forward.fs");

	quark->renderer.skybox_shader = shader_load(quark,
        "skybox", "res/shaders/skybox.vs", "res/shaders/skybox.fs");
	
	map_load(quark, &quark->map);

	info(quark, "resources reloaded!");
}

void info(quark_t* quark, const char* msg, ...) {
    char buffer[MAX_MSG_LEN];
	va_list args;
	va_start(args, msg);
	size_t len = vsnprintf(buffer, MAX_MSG_LEN, msg, args);
	buffer[len] = '\0';
	va_end(args);

	char text[MAX_MSG_LEN + 32];
	sprintf(text, "%s", buffer);

	puts(text);
	con_add_history(quark, &quark->console, text);
}

void error(quark_t* quark, const char* msg, ...) {
    char buffer[MAX_MSG_LEN];
	va_list args;
	va_start(args, msg);
	size_t len = vsnprintf(buffer, MAX_MSG_LEN, msg, args);
	buffer[len] = '\0';
	va_end(args);

	char text[MAX_MSG_LEN + 32];
	sprintf(text, "error: %s", buffer);

	puts(text);
	con_add_history(quark, &quark->console, text);

	#ifdef QUARK_DEBUG
	exit(EXIT_FAILURE);
	#endif
}

char* load_file(quark_t* quark, const char* path) {
	FILE* fp = fopen(path, "r");
	if (!fp) error(quark, "failed to open %s for reading", path);
	
	fseek(fp, 0, SEEK_END);
	size_t len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
    char* text = malloc(len + 1);
	fread(text, 1, len + 1, fp);
	fclose(fp);
	text[len] = '\0';
	return text;
}

void clear_file(quark_t* quark, const char* path) {
	FILE* fp = fopen(path, "w");
	if (!fp) error(quark, "failed to clear file %s", path);
	fclose(fp);
}
