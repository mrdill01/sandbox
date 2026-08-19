#include "entity.h"
#include "quark.h"

void entity_init_point_light(quark_t* quark,
    const char* name, vec3 position, vec3 color, float extinguish, entity_t** out)
{
	if (!out) return;

	entity_t* entity = NULL;
	entity_init_common(quark, name, ENTITY_POINT_LIGHT, position, &entity);
	glm_vec3_copy(color, entity->data.point_light.color);
    entity->data.point_light.extinguish = extinguish;

	*out = entity;
}

void entity_tick_point_light(quark_t* quark, entity_t* entity, entity_point_light_t* point_light) {
    if (point_light->extinguish > 0.0f) {
        glm_vec3_lerp(point_light->color,
            (vec3){0.0f, 0.0f, 0.0f},
            (quark->time - entity->spawn_time) / point_light->extinguish,
            point_light->color);
        
        if (quark->time - entity->spawn_time >= point_light->extinguish) {
            entlist_remove(quark, &quark->map.entlist, entity);
        }
    }
}
