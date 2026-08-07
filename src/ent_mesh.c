#include "entity.h"
#include "sbox.h"

void entity_init_mesh(sbox_t* sbox,
	const char* name, float x, float y, float z, mesh_t* mesh, entity_t** out)
{
    entity_t* entity = NULL;
	entity_init_common(sbox, name, ENTITY_MESH, (vec3){x, y, z}, &entity);
    entity->data.mesh.mesh = mesh_copy(sbox, mesh);
    for (int i = 0; i < MAX_MATERIALS; i++) {
        entity->data.mesh.materials[i] = NULL;
	}
	entity->data.mesh.is_visible = true;
	entity->data.mesh.is_viewmodel = false;
	entity->data.mesh.is_pickup = false;
	entity->data.mesh.enable_collision = true;

    *out = entity;
}

void entity_mesh_set_material(sbox_t* sbox, entity_t* entity, material_t* material, int slot) {
	if (!entity) return;
	
	if (slot >= MAX_MATERIALS) {
		error(sbox, "material limit per entity reached (%d)", MAX_MATERIALS);
		return;
	}

    entity->data.mesh.materials[slot] = material;
}
