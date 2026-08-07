#include "entity.h"
#include "sbox.h"

void entity_mesh_set_material(sbox_t* sbox, entity_t* entity, material_t* material, int slot) {
	if (!entity) return;
	
	if (slot >= MAX_MATERIALS) {
		error(sbox, "material limit per entity reached (%d)", MAX_MATERIALS);
		return;
	}

    entity->data.mesh.materials[slot] = material;
}
