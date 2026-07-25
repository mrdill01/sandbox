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
	entity->local_bbox = (bbox_t){0};
	entity->world_bbox = (bbox_t){0};

	*out = entity;
}

void entity_init_prop(sbox_t* sbox,
	const char* name, float x, float y, float z, mesh_t* mesh, entity_t** out)
{
    entity_t* entity = NULL;
	init_common(name, ENTITY_MESH, x, y, z, &entity);
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
	if (entity->type != ENTITY_MESH) return;
	
	entity->local_bbox = entity->data.prop.mesh->bbox;

	mat4 rotation;
	glm_quat_rotate(GLM_MAT4_IDENTITY, entity->rotation, rotation);
	entity->world_bbox = entity->local_bbox;
	entity->world_bbox = bbox_rotate(&entity->world_bbox, rotation);
	entity->world_bbox = bbox_translate(&entity->world_bbox, entity->position);
	entity->world_bbox = bbox_scale(&entity->world_bbox, entity->scale);
}

void entity_free(sbox_t* sbox, entity_t* entity) {
	if (!entity) return;
    free(entity);
}

bool entity_get_drawcall(sbox_t* sbox, entity_t* entity, drawcall_t* drawcall) {
	if (!drawcall)
		return false;
	
	if (entity->type != ENTITY_MESH && entity->type != ENTITY_DROPPED_ITEM)
		return false;

	if (entity->type == ENTITY_MESH && !entity->data.prop.is_visible)
		return false;
	
    if (entity->type == ENTITY_MESH && entity->data.prop.is_viewmodel)
		return false;

	drawcall->entity = malloc(strlen(entity->name));
	strcpy(drawcall->entity, entity->name);

	drawcall->mesh = entity->data.prop.mesh;
	memcpy(drawcall->materials, entity->data.prop.materials,
		sizeof(material_t*) * MAX_MATERIALS);

	int n = 0;
	for (int i = 0; i < MAX_MATERIALS; i++)
		if (entity->data.prop.materials[i])
			n++;

	if (n != entity->data.prop.mesh->nmaterials) {
		error(sbox, "entity %s doesn't have the proper number of materials (have %d, need %d)",
			entity->name, n, entity->data.prop.mesh->nmaterials);
		return false;
	}

	drawcall->local_bbox = entity->data.prop.mesh->bbox;

	glm_mat4_identity(drawcall->model);
	glm_translate(drawcall->model, entity->position);
	glm_quat_rotate(drawcall->model, entity->rotation, drawcall->model);
	glm_scale(drawcall->model, entity->scale);
	drawcall->world_bbox = entity->world_bbox;

	glm_vec3_copy(entity->position, drawcall->position);
	glm_quat_rotate(GLM_MAT4_IDENTITY, entity->rotation, drawcall->rotation);
	glm_vec3_copy(entity->scale, drawcall->scale);

	drawcall->dist_to_camera = 0.0f;

	drawcall->is_translucent = false;
	for (int i = 0; i < MAX_MATERIALS; i++) {
		const material_t* material = drawcall->materials[i];
		if (!material) continue;
		if (material->is_translucent) {
			drawcall->is_translucent = true;
			break;
		}
	}

	return true;
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
		case ENTITY_MESH: {
			entity_mesh_t* prop = &entity->data.prop;
			if (prop->is_pickup) {
    			glm_quat(entity->rotation, rad(sbox->time * ITEM_SPIN_RATE), 0.0f, 1.0f, 0.0f);
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
