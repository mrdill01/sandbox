#include "core.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>

#define MAX_MSG_LEN 2048

void sbox_init(sbox_t* sbox) {
    sbox->cfg.v_width = 960;
    sbox->cfg.v_height = 540;

	sbox->running = true;
	sbox->now = SDL_GetPerformanceCounter();
	sbox->last = 0;
	sbox->dt = 0.0;
	sbox->time = 0.0;

	for (int i = 0; i < NUM_KEYS; i++)
		sbox->keys[i] = false;
	sbox->mxdt = 0.0f;
	sbox->mydt = 0.0f;

	sbox->window = NULL;
	sbox->gl_context = NULL;

	sbox->shaders = NULL;
	sbox->meshes = NULL;
	sbox->textures = NULL;
	sbox->materials = NULL;

    camera_init(&sbox->camera);
	sbox->entlist.ents = malloc(sizeof(ent_t) * sbox->entlist.len);
	sbox->entlist.len = 0;
}

static int comp_ent_distance(const void* a_ptr, const void* b_ptr) {
	ent_t* a = (ent_t*)a_ptr;
	ent_t* b = (ent_t*)b_ptr;
	if (a->dist_to_camera == -1.0f) return -1;
	if (b->dist_to_camera == -1.0f) return 1;
	if (a->dist_to_camera < b->dist_to_camera) return 1;
	return -1;
}

void sbox_tick(sbox_t* sbox) {
	sbox->last = sbox->now;
	sbox->now = SDL_GetPerformanceCounter();
   	sbox->dt = (sbox->now - sbox->last) / (double)SDL_GetPerformanceFrequency();
	sbox->time += sbox->dt;

	for (size_t i = 0; i < sbox->entlist.len; i++) {
		ent_t* ent = &sbox->entlist.ents[i];
		vec3 tmp;

		vec3 center;
		bbox_get_center(&ent->mesh->bbox, center);

		glm_vec3_sub(center, sbox->camera.position, tmp);
		ent->dist_to_camera = glm_vec3_norm2(tmp);

		bool is_translucent = true;
		for (int i = 0; i < ent->nmaterials; i++) {
			if (ent->materials[i]->is_translucent) {
				is_translucent = true;
				break;
			}
		}

		if (is_translucent)
			ent->dist_to_camera = -1.0f;
	}

	qsort(sbox->entlist.ents, sbox->entlist.len, sizeof(ent_t), comp_ent_distance);
}

void add_ent(sbox_t* sbox, ent_t ent) {
	info(sbox, "add ent '%s'", ent.name);
	sbox->entlist.ents = realloc(sbox->entlist.ents, sizeof(ent_t) * (sbox->entlist.len + 1));
	sbox->entlist.ents[sbox->entlist.len++] = ent;
}

void info(sbox_t* sbox, const char* msg, ...) {
    char buffer[MAX_MSG_LEN];
	va_list args;
	va_start(args, msg);
	vsnprintf(buffer, MAX_MSG_LEN, msg, args);
	va_end(args);

    printf("[%gs] %s\n", sbox->time, buffer);
}

void error(sbox_t* sbox, const char* msg, ...) {
    char buffer[MAX_MSG_LEN];
	va_list args;
	va_start(args, msg);
	vsnprintf(buffer, MAX_MSG_LEN, msg, args);
	va_end(args);

    printf("[%gs] error: %s\n", sbox->time, buffer);
}

char* load_file(sbox_t* sbox, const char* path) {
	FILE* fp = fopen(path, "r");
	if (!fp) error(sbox, "failed to read '%s'", path);
	
	fseek(fp, 0, SEEK_END);
	size_t len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
    char* text = malloc(len + 1);
	fread(text, 1, len + 1, fp);
	fclose(fp);
	text[len] = '\0';
	return text;
}
