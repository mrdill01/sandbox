#include "physics.h"
#include "sbox.h"

#define PHYS_TRACE_STEP 0.01

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
            if (!entity->data.prop.enable_collision) continue;
            
            if (bbox_point_intersects(&entity->bbox, trace.point)) {
                if (entity->data.prop.materials[0]->is_water) {
                    trace.water_level = trace.distance / max_distance;
                    continue;
                }

                /*if (dist[0] > dist[1] && dist[0] > dist[2]) {
                    printf("X\n");
                } else if (dist[1] > dist[0] && dist[1] > dist[2]) {
                    printf("Y\n");
                } else {
                    printf("Z\n");
                }*/

                glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, trace.normal);

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
