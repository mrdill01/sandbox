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
    for (int i = 0; i < MAX_MATERIALS; i++) {
        entity->materials[i] = NULL;
	}
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

void entlist_tick(sbox_t* sbox, entlist_t* entlist) {
	
}

void entlist_add(sbox_t* sbox, entlist_t* entlist, entity_t* entity) {
	if (!entity) return;
	info(sbox, "entlist: add %s", entity->name);
	entlist->ents = realloc(entlist->ents, sizeof(entity_t*) * (entlist->len + 1));
	entlist->ents[entlist->len++] = entity;
}

entity_t* entlist_find_by_name(sbox_t* sbox, entlist_t* entlist, const char* name) {
	for (size_t i = 0; i < entlist->len; i++) {
		entity_t* entity = entlist->ents[i];
		if (strcmp(entity->name, name) == 0)
			return entity;
	}
	return NULL;
}
