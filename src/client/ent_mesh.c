#include "entity.h"
#include "quark.h"

void entity_init_mesh(quark_t* quark,
	const char* name, float x, float y, float z, mesh_t* mesh, entity_t** out)
{
    entity_t* entity = NULL;
	entity_init_common(quark, name, ENTITY_MESH, (vec3){x, y, z}, &entity);
    entity->data.mesh.mesh = mesh_copy(quark, mesh);
    for (int i = 0; i < MAX_MATERIALS; i++) {
        entity->data.mesh.materials[i] = NULL;
	}
	entity->data.mesh.is_visible = true;
	entity->data.mesh.is_pickup = false;
	entity->data.mesh.enable_collision = true;
	entity->data.mesh.enable_physics = false;

    *out = entity;
}

void entity_tick_mesh(quark_t* quark, entity_t* entity, entity_mesh_t* mesh) {
	if (mesh->is_pickup) {
		glm_quat(entity->rotation, rad(quark->time * PICKUP_SPIN_RATE), 0.0f, 1.0f, 0.0f);
	}

	if (mesh->enable_physics) {
		vec3 move;
		glm_vec3_copy(entity->velocity, move);
		glm_vec3_scale(move, quark->dt, move);
		glm_vec3_add(entity->position, move, entity->position);
	}
}

void entity_mesh_set_material(quark_t* quark, entity_t* entity, material_t* material, int slot) {
	if (!entity || entity->type != ENTITY_MESH) return;
	
	if (slot >= MAX_MATERIALS) {
		error(quark, "material limit per entity reached (%d)", MAX_MATERIALS);
		return;
	}

    entity->data.mesh.materials[slot] = material;
}
