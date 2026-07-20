#include "entity.h"
#include "sbox.h"

static void init_common(
	const char* name, entity_type_t type, float x, float y, float z, entity_t** out)
{
	if (!out) return;

    entity_t* entity = malloc(sizeof(entity_t));
	entity->name = name;
	entity->type = type;
	glm_vec3_copy((vec3){x, y, z}, entity->position);
    glm_quat_copy(GLM_QUAT_IDENTITY, entity->rotation);
	glm_vec3_copy((vec3){1.0f, 1.0f, 1.0f}, entity->scale);
	entity->bbox = (bbox_t){0};

	*out = entity;
}

void entity_init_prop(sbox_t* sbox,
	const char* name, float x, float y, float z, mesh_t* mesh, entity_t** out)
{
    entity_t* entity = NULL;
	init_common(name, ENTITY_PROP, x, y, z, &entity);
    entity->data.prop.mesh = mesh;
    for (int i = 0; i < MAX_MATERIALS; i++) {
        entity->data.prop.materials[i] = NULL;
	}
	entity->data.prop.is_visible = true;
	entity->data.prop.is_viewmodel = false;
	entity->data.prop.is_pickup = false;
	entity->data.prop.enable_collision = true;

    *out = entity;
}

void entity_init_sun_light(sbox_t* sbox,
    const char* name,
    float x, float y, float z,
    vec3 dir, vec3 color, entity_t** out)
{
	if (!out) return;

	entity_t* entity = NULL;
	init_common(name, ENTITY_SUN_LIGHT, x, y, z, &entity);
	glm_vec3_copy(dir, entity->data.sun_light.direction);
	glm_vec3_norm(entity->data.sun_light.direction);
	glm_vec3_copy(color, entity->data.sun_light.color);
	glm_mat4_identity(entity->data.sun_light.matrix);

	*out = entity;
}

void entity_init_point_light(sbox_t* sbox,
    const char* name, float x, float y, float z, vec3 color, entity_t** out)
{
	if (!out) return;

	entity_t* entity = NULL;
	init_common(name, ENTITY_POINT_LIGHT, x, y, z, &entity);
	glm_vec3_copy(color, entity->data.point_light.color);

	*out = entity;
}

static void compute_bounding_box(entity_t* entity) {
	if (entity->type != ENTITY_PROP) return;
	
	mat4 rotation;
	glm_quat_rotate(GLM_MAT4_IDENTITY, entity->rotation, rotation);
	entity->bbox = bbox_rotate(&entity->data.prop.mesh->bbox, rotation);
	entity->bbox = bbox_translate(&entity->bbox, entity->position);
}

void entity_free(sbox_t* sbox, entity_t* entity) {
	if (!entity) return;
    free(entity);
}

void entity_prop_set_material(sbox_t* sbox,
    entity_t* entity,
    material_t* material,
    int slot)
{
	if (!entity) return;
	
	if (slot >= MAX_MATERIALS) {
		error(sbox, "material limit per entity reached (%d)", MAX_MATERIALS);
		return;
	}

    entity->data.prop.materials[slot] = material;
}

void entlist_init(sbox_t* sbox, entlist_t* entlist) {
	entlist->len = 0;
	entlist->ents = NULL;
}

void entlist_free(sbox_t* sbox, entlist_t* entlist) {
	for (size_t i = 0; i < entlist->len; i++)
		entity_free(sbox, entlist->ents[i]);
	info(sbox, "freed %d entities", entlist->len);
	free(entlist->ents);
}

void entlist_tick(sbox_t* sbox, entlist_t* entlist) {
	for (size_t i = 0; i < entlist->len; i++) {
		entity_t* entity = entlist->ents[i];
		if (!entity) continue;
		compute_bounding_box(entity);

		switch (entity->type) {
		case ENTITY_PROP: {
			entity_prop_t* prop = &entity->data.prop;
			if (prop->is_pickup) {
    			glm_quat(entity->rotation, rad(sbox->time * PICKUP_SPIN_RATE), 0.0f, 1.0f, 0.0f);
			}
			break;
		}
		default: break;
		}
	}
}

void entlist_add(sbox_t* sbox, entlist_t* entlist, entity_t* entity) {
	if (!entity) return;
	info(sbox, "add entity %s", entity->name);
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
