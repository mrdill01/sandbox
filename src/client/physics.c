#include "physics.h"
#include "quark.h"

static void compute_trace_normal(trace_result_t* trace, const bbox_t* bbox) {
    vec3 center;
    bbox_get_center(bbox, center);

    vec3 size;
    bbox_get_size(bbox, size);

    vec3 local_point;
    glm_vec3_sub(center, trace->point, local_point);

    vec3 abs_point;
    glm_vec3_abs(local_point, abs_point);

    vec3 diff;
    glm_vec3_sub(abs_point, size, diff);

    float max_value = max(max(diff[0], diff[1]), diff[2]);
    if (max_value == diff[0]) trace->normal[0] = sign(local_point[0]);
    else if (max_value == diff[1]) trace->normal[1] = sign(local_point[1]);
    else trace->normal[2] = sign(local_point[2]);
}

bool phys_line_trace(
    quark_t* quark,
    vec3 start,
    vec3 dir,
    double max_distance,
    entlist_t* entlist,
    int ignore_player_id,
    int ignore_entities[PHYS_MAX_IGNORE_ENTITES],
    size_t nignore_entities,
    trace_result_t* out)
{
    trace_result_t trace;
    glm_vec3_copy(start, trace.point);
    glm_vec3_zero(trace.normal);
    trace.water_level = 0.0f;
    glm_vec3_zero(trace.enter_water_point);
    trace.start_in_water = false;
    trace.entity = NULL;
    trace.player_id = -1;
    trace.material = NULL;
    trace.phys_mat = PHYS_MAT_NONE;

    for (trace.distance = 0.0f; trace.distance < max_distance; trace.distance += PHYS_TRACE_STEP) {
        for (size_t i = 0; i < entlist->len; i++) {
            entity_t* entity = entlist->ents[i];
            if (!entity) continue;

            bool skip_entity = false;
            for (size_t j = 0; j < nignore_entities; j++)
                if (ignore_entities[j] == entity->id)
                    skip_entity = true;

            if (skip_entity)
                continue;
            
            if (entity->type == ENTITY_MESH && !entity->data.mesh.enable_collision) continue;
            if (entity->type == ENTITY_PROJECTILE) continue;
            
            if (!bbox_point_intersects(&entity->world_bbox, trace.point))
                continue;
            
            /*if (ray_intersects_mesh(
                entity->position, entity->rotation, start, dir,
                entity->data.mesh.mesh, &trace.distance, max_distance))
                continue;*/

            vec3 scaled_dir;
            glm_vec3_copy(dir, scaled_dir);
            glm_vec3_scale(scaled_dir, trace.distance, scaled_dir);

            vec3 end;
            glm_vec3_add(start, scaled_dir, end);

            glm_vec3_copy(end, trace.point);
            
            if (entity->data.mesh.materials[0]->is_water) {
                if (trace.enter_water_point[0] == 0.0f &&
                    trace.enter_water_point[1] == 0.0f &&
                    trace.enter_water_point[2] == 0.0f)
                {
                    glm_vec3_copy(trace.point, trace.enter_water_point);
                }
                
                trace.water_level = trace.distance / max_distance;
                
                if (trace.distance == 0.0f) {
                    trace.start_in_water = true;
                }

                continue;
            }

            compute_trace_normal(&trace, &entity->world_bbox);
            trace.entity = entity;
            trace.material = entity->data.mesh.materials[0];
            trace.phys_mat = entity->data.mesh.materials[0]->phys_mat;

            if (out)
                *out = trace;

            return true;
        }

        for (size_t j = 0; j < NET_MAX_PLAYERS; j++) {
            if (j == ignore_player_id) continue;
            
            player_t* player = quark->players[j];
            if (!player) continue;

            bbox_t bbox = player->bbox;
            bbox = bbox_translate(&bbox, player->position);
            if (bbox_point_intersects(&bbox, trace.point)) {
                compute_trace_normal(&trace, &bbox);
                trace.player_id = j;
                trace.material = NULL;
                trace.phys_mat = PHYS_MAT_PLAYER;

                if (out)
                    *out = trace;

                return true;
            }
        }

        vec3 step;
        glm_vec3_scale(dir, PHYS_TRACE_STEP, step);
        glm_vec3_add(trace.point, step, trace.point);
    }

    if (out)
        *out = trace;
    
    return false;
}
