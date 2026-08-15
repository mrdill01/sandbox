#include "entity.h"
#include "quark.h"

void entity_init_explosion(quark_t* quark,
	const char* name, vec3 position, float radius, vec3 direction,
    float min_force, float max_force, entity_t** out)
{
    entity_t* entity = NULL;
	entity_init_common(quark, name, ENTITY_EXPLOSION, position, &entity);
	entity->data.explosion.radius = radius;
    glm_vec3_copy(direction, entity->data.explosion.direction);
    glm_vec3_normalize(entity->data.explosion.direction);
	entity->data.explosion.min_force = min_force;
	entity->data.explosion.max_force = max_force;

    *out = entity;
}

void entity_tick_explosion(quark_t* quark, entity_t* entity, entity_explosion_t* explosion) {
    r_add_partfx_explosion(quark, &quark->renderer, entity->position, explosion->radius);
    a_play(quark, &quark->audio, quark->audio.explosion_sound, entity->position, random(0.85f, 1.15f));

    for (int i = 0; i < NET_MAX_PLAYERS; i++) {
        player_t* player = quark->players[i];
        if (!player) continue;

        float distance = glm_vec3_distance(player->position, entity->position);
        if (distance < explosion->radius) {
            float factor = clamp(explosion->radius / distance, 0.0f, 1.0f);
            float force = lerp(2.0f, 8.0f, factor);

            vec3 force_vec;
            glm_vec3_sub(player->position, entity->position, force_vec);
            glm_vec3_scale(force_vec, force, force_vec);

            glm_vec3_add(player->velocity, force_vec, player->velocity);

            if (!player->is_me)
                player_add_damage(quark, player, explosion->damage * factor);
        }
    }

    if (sv_destruction.value) {
        for (int i = 0; i < quark->map.entlist.len; i++) {
            entity_t* other = quark->map.entlist.ents[i];
            if (!other || other->type != ENTITY_MESH) continue;
            if (other->data.mesh.materials[0] && other->data.mesh.materials[0]->is_water)
                continue;

            if (bbox_sphere_intersects(&other->world_bbox, entity->position, explosion->radius)) {
                vec3 direction;
                glm_vec3_sub(other->position, entity->position, direction);
                glm_vec3_normalize(direction);

                float amount = 0.5f;
                mesh_deform(quark,
                    other->data.mesh.mesh, other->position, entity->position,
                    direction, explosion->radius, amount);
            }
        }
    }

    entlist_remove(quark, &quark->map.entlist, entity);
}
