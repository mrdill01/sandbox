#include "physics.h"
#include "sbox.h"

bool phys_line_trace(
    vec3 start, vec3 dir, double max_distance, entlist_t* entlist, trace_result_t* out)
{
    trace_result_t trace;
    glm_vec3_copy(start, trace.point);
    trace.water_level = 0.0f;
    trace.entity = NULL;
    trace.phys_mat = PHYS_MAT_NONE;
    bool hit = false;

    for (trace.distance = 0; trace.distance < max_distance; trace.distance += PHYS_TRACE_STEP) {
        vec3 step;
        glm_vec3_scale(dir, PHYS_TRACE_STEP, step);
        glm_vec3_add(trace.point, step, trace.point);

        for (size_t j = 0; j < entlist->len; j++) {
            entity_t* entity = entlist->ents[j];
            if (entity->type != ENTITY_PROP) continue;
            if (!entity->data.prop.collision_enabled) continue;
            
            bbox_t bbox = bbox_translate(&entity->data.prop.mesh->bbox, entity->position);

            if (bbox_point_intersects(&bbox, trace.point)) {
                if (entity->data.prop.materials[0]->is_water) {
                    trace.water_level = trace.distance / max_distance;
                    continue;
                }

                hit = true;
                trace.entity = entity;
                trace.phys_mat = entity->data.prop.materials[0]->phys_mat;
                goto on_hit;
            }
        }
    }

    on_hit:

    if (out)
        *out = trace;
    
    return hit;
}
