#include "entity.h"
#include "sbox.h"

void entity_init_pickup(sbox_t* sbox,
    const char* name, vec3 position, mesh_t* mesh, void* pickup_sound, entity_t** out)
{
    entity_t* entity = NULL;
	entity_init_common(sbox, name, ENTITY_PICKUP, position, &entity);
    entity->data.pickup.mesh = mesh;
    for (int i = 0; i < MAX_MATERIALS; i++) {
        entity->data.pickup.materials[i] = NULL;
	}
    entity->data.pickup.pickup_sound = pickup_sound;
    entity->data.pickup.is_coin = false;

    *out = entity;
}

void entity_pickup_set_material(sbox_t* sbox, entity_t* entity, material_t* material, int slot) {
	if (!entity || entity->type != ENTITY_PICKUP) return;
	
	if (slot >= MAX_MATERIALS) {
		error(sbox, "material limit per entity reached (%d)", MAX_MATERIALS);
		return;
	}

    entity->data.mesh.materials[slot] = material;
}

void entity_tick_pickup(sbox_t* sbox, entity_t* entity, entity_pickup_t* pickup) {
    glm_quat(entity->rotation, rad(sbox->time * 180.0f), 0.0f, 1.0f, 0.0f);

    float distance = glm_vec3_distance(entity->position, sbox->player->position);
    if (distance < PICKUP_COLLECT_DISTANCE) {
        a_play(sbox, &sbox->audio, (sound_t*)pickup->pickup_sound,
            entity->position, random(0.85f, 1.15f));
        
        if (pickup->is_coin) {
            sbox->player->inventory.coins++;
            r_add_partfx_pickup_coin(sbox, &sbox->renderer, entity->position);
        }
        
        entlist_remove(sbox, &sbox->map.entlist, entity);

    } else if (distance < PICKUP_SUCK_IN_DISTANCE) {
        vec3 direction;
        glm_vec3_sub(sbox->player->position, entity->position, direction);

        float factor = lerp(0.1f, 1.0f, distance / PICKUP_SUCK_IN_DISTANCE);

        vec3 velocity;
        glm_vec3_copy(direction, velocity);
        glm_vec3_scale(velocity, sbox->dt * PICKUP_SUCK_IN_SPEED * factor, velocity);

        glm_vec3_add(entity->velocity, velocity, entity->velocity);

    } else {
        glm_vec3_zero(entity->velocity);
    }
}
