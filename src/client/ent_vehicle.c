#include "entity.h"
#include "quark.h"

void entity_init_vehicle(quark_t* quark,
    const char* name, vec3 position, mesh_t* mesh, vehicle_type_t type, entity_t** out)
{
    entity_t* entity = NULL;
	entity_init_common(quark, name, ENTITY_VEHICLE, position, &entity);
    entity->data.vehicle.mesh = mesh;
    for (int i = 0; i < MAX_MATERIALS; i++) {
		entity->data.vehicle.materials[i] = NULL;
	}
	entity->data.vehicle.type = type;

    *out = entity;
}

void entity_tick_vehicle(quark_t* quark, entity_t* entity, entity_vehicle_t* vehicle) {
    switch (vehicle->type) {
    case VEHICLE_CAR: {
        //entity_tick_vehicle_car(quark, entity, &vehicle->data.car);
        break;
    }
    case VEHICLE_HELICOPTER: {
        entity_tick_vehicle_helicopter(quark, entity, &vehicle->data.heli);
        break;
    }
    default: unreachable(quark);
    }
}

void entity_vehicle_set_material(quark_t* quark, entity_t* entity, material_t* material, int slot) {
    if (!entity || entity->type != ENTITY_VEHICLE) return;
	
	if (slot >= MAX_MATERIALS) {
		error(quark, "material limit per entity reached (%d)", MAX_MATERIALS);
		return;
	}

    entity->data.vehicle.materials[slot] = material;
}
