#include "entity.h"
#include "sbox.h"

void entity_init(sbox_t* sbox, const char* name, float x, float y, float z, entity_t** out) {
    if (!out) return;

    entity_t* entity = malloc(sizeof(entity_t));
    entity->name = name;

    glm_vec3_copy((vec3){x, y, z}, entity->position);
    glm_quat_copy(GLM_QUAT_IDENTITY, entity->rotation);

    entity->mesh = NULL;
	entity->nmaterials = 0;
    for (int i = 0; i < MAX_MATERIALS; i++)
        entity->materials[i] = NULL;

    entity->dist_to_camera = 0.0f;
	entity->is_viewmodel = false;

    *out = entity;
}

void entity_free(sbox_t* sbox, entity_t* entity) {
	if (!entity) return;
    free(entity);
}

void entlist_init(sbox_t* sbox, entlist_t* entlist) {
	entlist->len = 0;
	entlist->ents = NULL;
}

void entlist_free(sbox_t* sbox, entlist_t* entlist) {
	for (size_t i = 0; i < entlist->len; i++)
		entity_free(sbox, entlist->ents[i]);
	free(entlist->ents);
}

static int comp_ent_distance(const void* a_ptr, const void* b_ptr) {
	entity_t* a = *(entity_t**)a_ptr;
	entity_t* b = *(entity_t**)b_ptr;
	if (a->dist_to_camera == -1.0f) return -1;
	if (b->dist_to_camera == -1.0f) return 1;
	if (a->dist_to_camera < b->dist_to_camera) return 1;
	return -1;
}

void entlist_tick(sbox_t* sbox, entlist_t* entlist) {
	for (size_t i = 0; i < entlist->len; i++) {
		entity_t* ent = entlist->ents[i];
		vec3 tmp;

		vec3 center;
		bbox_get_center(&ent->mesh->bbox, center);

		glm_vec3_sub(center, sbox->renderer.camera.position, tmp);
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

	qsort(entlist->ents, entlist->len, sizeof(entity_t*), comp_ent_distance);
}

void entlist_add(sbox_t* sbox, entlist_t* entlist, entity_t* entity) {
	if (!entity) return;
	info(sbox, "entlist: add %s", entity->name);
	sbox->entlist.ents = realloc(sbox->entlist.ents, sizeof(entity_t*) * (sbox->entlist.len + 1));
	sbox->entlist.ents[sbox->entlist.len++] = entity;
}

entity_t* entlist_find_by_name(sbox_t* sbox, entlist_t* entlist, const char* name) {
	for (size_t i = 0; i < entlist->len; i++) {
		entity_t* entity = entlist->ents[i];
		if (strcmp(entity->name, name) == 0)
			return entity;
	}
	return NULL;
}
