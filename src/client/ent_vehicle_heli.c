#include "entity.h"
#include "quark.h"

void entity_tick_vehicle_helicopter(
    quark_t* quark,
    entity_t* entity,
    veh_helicopter* heli)
{
    vec3 move;
    glm_vec3_copy(entity->velocity, move);
    glm_vec3_scale(move, quark->dt, move);
    glm_vec3_add(entity->position, move, entity->position);
}
