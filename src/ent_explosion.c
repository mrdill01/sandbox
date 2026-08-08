#include "entity.h"
#include "sbox.h"

void entity_init_explosion(sbox_t* sbox,
	const char* name, vec3 position, float radius, vec3 direction,
    float min_force, float max_force, entity_t** out)
{
    entity_t* entity = NULL;
	entity_init_common(sbox, name, ENTITY_EXPLOSION, position, &entity);
	entity->data.explosion.radius = radius;
    glm_vec3_copy(direction, entity->data.explosion.direction);
    glm_vec3_normalize(entity->data.explosion.direction);
	entity->data.explosion.min_force = min_force;
	entity->data.explosion.max_force = max_force;

    *out = entity;
}

void entity_tick_explosion(sbox_t* sbox, entity_t* entity, entity_explosion_t* explosion) {
    r_add_partfx_explosion(sbox, &sbox->renderer, entity->position, explosion->radius);
    a_play(sbox, &sbox->audio, sbox->audio.explosion_sound, entity->position, random(0.85f, 1.15f));

    for (int i = 0; i < MAX_PLAYERS; i++) {
        player_t* player = sbox->players[i];
        if (!player) continue;

        float dist = glm_vec3_distance(player->position, entity->position);
        if (dist < explosion->radius) {
            float factor = 1.0f - clamp(dist / explosion->radius, 0.0f, 1.0f);
            float force = lerp(2.0f, 8.0f, factor);

            vec3 force_vec;
            glm_vec3_sub(player->position, entity->position, force_vec);
            glm_vec3_scale(force_vec, force, force_vec);

            glm_vec3_add(player->velocity, force_vec, player->velocity);

            if (!player->is_me)
                player_add_damage(sbox, player, explosion->damage * factor);
        }
    }

    if (sv_destruction.value) {
        for (int i = 0; i < sbox->map.entlist.len; i++) {
            entity_t* other = sbox->map.entlist.ents[i];
            if (!other || other->type != ENTITY_MESH) continue;

            if (bbox_sphere_intersects(&other->world_bbox, entity->position, explosion->radius)) {
                vec3 direction;
                glm_vec3_sub(entity->position, other->position, direction);
                glm_vec3_normalize(direction);

                mesh_deform(sbox,
                    other->data.mesh.mesh, other->position, entity->position, direction, 0.1f);
            }
        }
    }

    entlist_remove(sbox, &sbox->map.entlist, entity);
}
