#include "entity.h"
#include "sbox.h"

void entity_init_explosion(sbox_t* sbox,
	const char* name, vec3 position, float radius, entity_t** out)
{
    entity_t* entity = NULL;
	entity_init_common(sbox, name, ENTITY_EXPLOSION, position, &entity);
	entity->data.explosion.radius = radius;

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
            float factor = 1.0f - (dist / explosion->radius);
            float force = lerp(2.0f, 7.0f, factor);

            vec3 dir;
            glm_vec3_sub(player->position, entity->position, dir);
            glm_vec3_scale(dir, force, dir);

            glm_vec3_add(player->velocity, dir, player->velocity);
            player->buttons |= PLAYER_BUTTON_JUMP;
        }
    }

    entlist_remove(sbox, &sbox->map.entlist, entity);
}
