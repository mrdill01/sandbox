#include "entity.h"
#include "quark.h"

void entity_init_common(
	quark_t* quark, const char* name, entity_type_t type, vec3 position, entity_t** out)
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
	glm_vec3_zero(entity->velocity);
	entity->parent_id = -1;
	entity->local_bbox = (bbox_t){0};
	entity->world_bbox = (bbox_t){0};
	entity->spawn_time = quark->time;

	*out = entity;
}

void entity_init_sun_light(quark_t* quark,
    const char* name,
    float x, float y, float z,
    vec3 dir, vec3 color, entity_t** out)
{
	if (!out) return;

	entity_t* entity = NULL;
	entity_init_common(quark, name, ENTITY_SUN_LIGHT, (vec3){x, y, z}, &entity);
	glm_vec3_copy(dir, entity->data.sun_light.direction);
	glm_normalize(entity->data.sun_light.direction);
	glm_vec3_copy(color, entity->data.sun_light.color);
	glm_mat4_identity(entity->data.sun_light.matrix);

	*out = entity;
}

void entity_free(quark_t* quark, entity_t* entity) {
	if (!entity) return;
	if (entity->name)
		free(entity->name);
    free(entity);
}

mesh_t* entity_get_mesh(quark_t* quark, entity_t* entity) {
	switch (entity->type) {
	case ENTITY_MESH: return entity->data.mesh.mesh;
	case ENTITY_PROJECTILE: return entity->data.projectile.mesh;
	case ENTITY_PICKUP: return entity->data.pickup.mesh;
	case ENTITY_VEHICLE: return entity->data.vehicle.mesh;
	case ENTITY_TERRAIN: return entity->data.terrain.mesh;
	default: return NULL;
	}
}

void entity_get_materials(quark_t* quark,
	entity_t* entity,
	material_t** materials,
	size_t* nmaterials)
{
	switch (entity->type) {
	case ENTITY_MESH: {
		memcpy(materials, entity->data.mesh.materials, sizeof(material_t*) * MAX_MATERIALS);
		if (nmaterials)
			*nmaterials = entity->data.mesh.mesh->nmaterials;
		break;
	}
	case ENTITY_PROJECTILE: {
		memcpy(materials, entity->data.projectile.materials, sizeof(material_t*) * MAX_MATERIALS);
		if (nmaterials)
			*nmaterials = entity->data.projectile.mesh->nmaterials;
		break;
	}
	case ENTITY_PICKUP: {
		memcpy(materials, entity->data.pickup.materials, sizeof(material_t*) * MAX_MATERIALS);
		if (nmaterials)
			*nmaterials = entity->data.pickup.mesh->nmaterials;
		break;
	}
	case ENTITY_VEHICLE: {
		memcpy(materials, entity->data.vehicle.materials, sizeof(material_t*) * MAX_MATERIALS);
		if (nmaterials)
			*nmaterials = entity->data.vehicle.mesh->nmaterials;
		break;
	}
	case ENTITY_TERRAIN: {
		memcpy(materials, entity->data.terrain.materials, sizeof(material_t*) * MAX_MATERIALS);
		if (nmaterials)
			*nmaterials = entity->data.terrain.mesh->nmaterials;
		break;
	}
	default: break;
	}
}

bool entity_get_drawcall(quark_t* quark, entity_t* entity, drawcall_t* drawcall) {
	if (!entity || !drawcall)
		return false;
	
	if (entity->type != ENTITY_MESH &&
		entity->type != ENTITY_PROJECTILE &&
		entity->type != ENTITY_PICKUP &&
		entity->type != ENTITY_VEHICLE &&
		entity->type != ENTITY_TERRAIN)
		return false;

	if (entity->type == ENTITY_MESH && !entity->data.mesh.is_visible)
		return false;
	
	drawcall->entity = malloc(strlen(entity->name));
	strcpy(drawcall->entity, entity->name);

	drawcall->mesh = entity_get_mesh(quark, entity);

	size_t nmaterials = 0;
	entity_get_materials(quark, entity, drawcall->materials, &nmaterials);

	drawcall->local_bbox = entity->local_bbox;
	drawcall->world_bbox = entity->world_bbox;

	glm_mat4_identity(drawcall->model);
	glm_translate(drawcall->model, entity->position);
	glm_quat_rotate(drawcall->model, entity->rotation, drawcall->model);
	glm_scale(drawcall->model, entity->scale);

	glm_vec3_copy(entity->position, drawcall->position);
	glm_quat_rotate(GLM_MAT4_IDENTITY, entity->rotation, drawcall->rotation);
	glm_vec3_copy(entity->scale, drawcall->scale);

	drawcall->distance_to_camera = 0.0f;

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

void entlist_init(quark_t* quark, entlist_t* entlist) {
	entlist->len = 0;
	entlist->ents = NULL;
}

void entlist_free(quark_t* quark, entlist_t* entlist) {
	for (size_t i = 0; i < entlist->len; i++) {
		entity_t* entity = entlist->ents[i];
		if (!entity) continue;
		entity_free(quark, entity);
		entlist->ents[i] = NULL;
	}

	info(quark, "freed %d entities", entlist->len);
	free(entlist->ents);
}

static void compute_bounding_box(quark_t* quark, entity_t* entity) {
	mesh_t* mesh = entity_get_mesh(quark, entity);
	if (!mesh) return;
	
	entity->local_bbox = mesh->bbox;

	entity->world_bbox = entity->local_bbox;
	entity->world_bbox = bbox_rotate(&entity->world_bbox, entity->rotation);
	entity->world_bbox = bbox_scale(&entity->world_bbox, entity->scale);
	entity->world_bbox = bbox_translate(&entity->world_bbox, entity->position);
}

void entlist_tick(quark_t* quark, entlist_t* entlist) {
	prof_start(quark, &quark->prof);

	for (size_t i = 0; i < entlist->len; i++) {
		entity_t* entity = entlist->ents[i];
		if (!entity) continue;
		compute_bounding_box(quark, entity);

		switch (entity->type) {
		case ENTITY_MESH: {
			entity_tick_mesh(quark, entity, &entity->data.mesh);
			break;
		}
		case ENTITY_PROJECTILE: {
			entity_tick_projectile(quark, entity, &entity->data.projectile);
			break;
		}
		case ENTITY_EXPLOSION: {
			entity_tick_explosion(quark, entity, &entity->data.explosion);
			break;
		}
		case ENTITY_PICKUP: {
			entity_tick_pickup(quark, entity, &entity->data.pickup);
			break;
		}
		case ENTITY_VEHICLE: {
			entity_tick_vehicle(quark, entity, &entity->data.vehicle);
			break;
		}
		case ENTITY_POINT_LIGHT: {
			entity_tick_point_light(quark, entity, &entity->data.point_light);
			break;
		}
		default: break;
		}
	}

	prof_end(quark, &quark->prof);
}

void entlist_add(quark_t* quark, entlist_t* entlist, entity_t* entity) {
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

void entlist_remove(quark_t* quark, entlist_t* entlist, entity_t* entity) {
	if (!entity || entity->id == -1) return;
	entlist->ents[entity->id] = NULL;
	entity_free(quark, entity);
}

entity_t* entlist_find_by_name(quark_t* quark, entlist_t* entlist, const char* name) {
	for (size_t i = 0; i < entlist->len; i++) {
		entity_t* entity = entlist->ents[i];
		if (!entity) continue;

		if (strcmp(entity->name, name) == 0)
			return entity;
	}
	return NULL;
}
