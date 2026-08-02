#include "entity.h"
#include "sbox.h"

void entity_init_common(
	sbox_t* sbox, const char* name, entity_type_t type, vec3 position, entity_t** out)
{
	if (!out) return;

    entity_t* entity = malloc(sizeof(entity_t));
	entity->id = -1;
	entity->name = malloc(strlen(name) + 1);
	strcpy(entity->name, name);
	entity->type = type;
	glm_vec3_copy(position, entity->position);
    glm_quat_identity(entity->rotation);
	glm_vec3_copy((vec3){1.0f, 1.0f, 1.0f}, entity->scale);
	entity->local_bbox = (bbox_t){0};
	entity->world_bbox = (bbox_t){0};
	entity->spawn_time = sbox->time;

	*out = entity;
}

void entity_init_prop(sbox_t* sbox,
	const char* name, float x, float y, float z, mesh_t* mesh, entity_t** out)
{
    entity_t* entity = NULL;
	entity_init_common(sbox, name, ENTITY_MESH, (vec3){x, y, z}, &entity);
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

void entity_init_vehicle(sbox_t* sbox,
	const char* name, float x, float y, float z, mesh_t* mesh, entity_t** out)
{
    entity_t* entity = NULL;
	entity_init_common(sbox, name, ENTITY_VEHICLE, (vec3){x, y, z}, &entity);
    entity->data.vehicle.mesh = mesh;
    for (int i = 0; i < MAX_MATERIALS; i++) {
        entity->data.vehicle.materials[i] = NULL;
	}

    *out = entity;
}

void entity_init_sun_light(sbox_t* sbox,
    const char* name,
    float x, float y, float z,
    vec3 dir, vec3 color, entity_t** out)
{
	if (!out) return;

	entity_t* entity = NULL;
	entity_init_common(sbox, name, ENTITY_SUN_LIGHT, (vec3){x, y, z}, &entity);
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
	entity_init_common(sbox, name, ENTITY_POINT_LIGHT, (vec3){x, y, z}, &entity);
	glm_vec3_copy(color, entity->data.point_light.color);

	*out = entity;
}

void entity_free(sbox_t* sbox, entity_t* entity) {
	if (!entity) return;
	free(entity->name);
    free(entity);
}

mesh_t* entity_get_mesh(sbox_t* sbox, entity_t* entity) {
	switch (entity->type) {
	case ENTITY_MESH: return entity->data.prop.mesh;
	case ENTITY_PROJECTILE: return entity->data.projectile.mesh;
	default: return NULL;
	}
}

void entity_get_materials(sbox_t* sbox,
	entity_t* entity,
	material_t** materials,
	size_t* nmaterials)
{
	switch (entity->type) {
	case ENTITY_MESH: {
		memcpy(materials, entity->data.prop.materials,
			sizeof(material_t*) * MAX_MATERIALS);
		if (nmaterials)
			*nmaterials = entity->data.prop.mesh->nmaterials;
		break;
	}
	case ENTITY_PROJECTILE: {
		memcpy(materials, entity->data.projectile.materials,
			sizeof(material_t*) * MAX_MATERIALS);
		if (nmaterials)
			*nmaterials = entity->data.projectile.mesh->nmaterials;
		break;
	}
	default: break;
	}
}

bool entity_get_drawcall(sbox_t* sbox, entity_t* entity, drawcall_t* drawcall) {
	if (!entity || !drawcall)
		return false;
	
	if (entity->type != ENTITY_MESH &&
		entity->type != ENTITY_DROPPED_ITEM &&
		entity->type != ENTITY_PROJECTILE)
		return false;

	if (entity->type == ENTITY_MESH && !entity->data.prop.is_visible)
		return false;
	
    if (entity->type == ENTITY_MESH && entity->data.prop.is_viewmodel)
		return false;

	drawcall->entity = malloc(strlen(entity->name));
	strcpy(drawcall->entity, entity->name);

	drawcall->mesh = entity_get_mesh(sbox, entity);

	material_t* materials[4] = {0};
	size_t nmaterials = 0;
	entity_get_materials(sbox, entity, materials, &nmaterials);
	memcpy(drawcall->materials, materials, sizeof(material_t*) * MAX_MATERIALS);

	int n = 0;
	for (int i = 0; i < MAX_MATERIALS; i++)
		if (materials[i])
			n++;

	/*if (n != nmaterials) {
		error(sbox, "entity %s doesn't have the proper number of materials (have %d, need %d)",
			entity->name, n, nmaterials);
		return false;
	}*/

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
	for (size_t i = 0; i < entlist->len; i++) {
		entity_t* entity = entlist->ents[i];
		if (!entity) continue;
		entity_free(sbox, entity);
	}

	info(sbox, "freed %d entities", entlist->len);
	free(entlist->ents);
}

static void compute_bounding_box(sbox_t* sbox, entity_t* entity) {
	mesh_t* mesh = entity_get_mesh(sbox, entity);
	if (!mesh) return;
	
	entity->local_bbox = mesh->bbox;

	mat4 rotation;
	glm_quat_rotate(GLM_MAT4_IDENTITY, entity->rotation, rotation);
	entity->world_bbox = entity->local_bbox;
	entity->world_bbox = bbox_rotate(&entity->world_bbox, rotation);
	entity->world_bbox = bbox_translate(&entity->world_bbox, entity->position);
	entity->world_bbox = bbox_scale(&entity->world_bbox, entity->scale);
}

void entlist_tick(sbox_t* sbox, entlist_t* entlist) {
	for (size_t i = 0; i < entlist->len; i++) {
		entity_t* entity = entlist->ents[i];
		if (!entity) continue;
		compute_bounding_box(sbox, entity);

		switch (entity->type) {
		case ENTITY_MESH: {
			entity_mesh_t* prop = &entity->data.prop;
			if (prop->is_pickup) {
    			glm_quat(entity->rotation, rad(sbox->time * ITEM_SPIN_RATE), 0.0f, 1.0f, 0.0f);
			}
			break;
		}
		case ENTITY_PROJECTILE: {
			entity_tick_projectile(sbox, entity, &entity->data.projectile);
			break;
		}
		case ENTITY_EXPLOSION: {
			entity_tick_explosion(sbox, entity, &entity->data.explosion);
			break;
		}
		default: break;
		}
	}
}

void entlist_add(sbox_t* sbox, entlist_t* entlist, entity_t* entity) {
	if (!entity) return;
	int slot = -1;

	for (int i = 0; i < entlist->len; i++) {
		entity_t* entity = entlist->ents[i];
		if (!entity) {
			slot = i;
			break;
		}
	}

	if (slot == -1) {
		entlist->ents = realloc(entlist->ents, sizeof(entity_t*) * (entlist->len + 1));
		entlist->ents[entlist->len++] = entity;
		slot = entlist->len - 1;
	} else {
		entlist->ents[slot] = entity;
	}

	entity->id = slot;
}

void entlist_remove(sbox_t* sbox, entlist_t* entlist, entity_t* entity) {
	if (!entity || entity->id == -1) return;
	entlist->ents[entity->id] = NULL;
	entity_free(sbox, entity);
}

entity_t* entlist_find_by_name(sbox_t* sbox, entlist_t* entlist, const char* name) {
	for (size_t i = 0; i < entlist->len; i++) {
		entity_t* entity = entlist->ents[i];
		if (!entity) continue;

		if (strcmp(entity->name, name) == 0)
			return entity;
	}
	return NULL;
}
