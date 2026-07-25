#include "physics.h"
#include "sbox.h"

bool phys_line_trace(
    vec3 start, vec3 dir, double max_distance, entlist_t* entlist, trace_result_t* out)
{
    trace_result_t trace;
    glm_vec3_copy(start, trace.point);
    glm_vec3_copy(GLM_VEC3_ZERO, trace.normal);
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
            if (entity->type != ENTITY_MESH) continue;
            if (!entity->data.prop.enable_collision) continue;
            
            if (bbox_point_intersects(&entity->world_bbox, trace.point)) {
                if (entity->data.prop.materials[0]->is_water) {
                    trace.water_level = trace.distance / max_distance;
                    continue;
                }

                vec3 center;
                bbox_get_center(&entity->world_bbox, center);

                vec3 half_size;
                bbox_get_half_size(&entity->local_bbox, half_size);

                vec3 diff;
                glm_vec3_sub(trace.point, center, diff);

                float bias = 1.00001f;
                if (fabsf(diff[0]) > half_size[0] * bias - 0.001f)
                    trace.normal[0] = sign(diff[0]);
                if (fabsf(diff[1]) > half_size[1] * bias - 0.001f)
                    trace.normal[1] = sign(diff[1]);
                if (fabsf(diff[2]) > half_size[2] * bias - 0.001f)
                    trace.normal[2] = sign(diff[2]);

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
