#include "player.h"
#include "sbox.h"

#define VIEWMODEL_POS_X -0.05f
#define VIEWMODEL_POS_Y -0.1f
#define VIEWMODEL_POS_Z 0.35f
#define WEAPON_SWITCH_DELAY 0.5f

static void tick_item(sbox_t* sbox, player_t* player, entlist_t* entlist) {
    item_t* item = inventory_get_item(sbox, &player->inventory);
    if (!item)
        return;
    
    weapon_t* weapon = &item->data.weapon;

    vec3 start;
    glm_vec3_copy(sbox->renderer.camera.position, start);
    
    if (player->is_thirdperson) {
        vec3 forward;
        glm_vec3_copy(sbox->renderer.camera.forward, forward);
        glm_vec3_scale(forward, PLAYER_THIRDPERSON_CAMERA_LENGTH, forward);
        glm_vec3_add(start, forward, start);
    }
    
    camera_t* camera = &sbox->renderer.camera;
    vec3 direction;
    glm_vec3_copy(camera->forward, direction);

    float spread = lerp(
        weapon->min_spread,
        weapon->max_spread,
        1.0f - player_get_accuracy(sbox, player));
    
    direction[0] += random(-spread, spread);
    direction[1] += random(-spread, spread);
    direction[2] += random(-spread, spread);
    glm_normalize(direction);

    if (player->buttons & PLAYER_BUTTON_FIRE) {
        if (sbox->time - weapon->last_fire < weapon->fire_rate)
            return;

        if (sbox->time - player->inventory.last_switch < WEAPON_SWITCH_DELAY)
            return;
        
        weapon->last_fire = sbox->time;
        a_play(sbox, &sbox->audio, weapon->fire_sound, player->position, 1.0f);

        player->item_anim[2] -= (player->buttons & PLAYER_BUTTON_AIM) ?
            weapon->recoil * 0.1f : weapon->recoil;
        
        sbox->renderer.camera.shake[0] += weapon->recoil * 14.0f;
        sbox->renderer.camera.shake[1] += weapon->recoil * 14.0f *
            ((random(0.0f, 1.0f) >= 0.5f) ? 1.0f : -1.0f);
        sbox->renderer.camera.shake[2] += weapon->recoil * 14.0f *
            ((random(0.0f, 1.0f) >= 0.5f) ? 1.0f : -1.0f);

        if (weapon->is_projectile) {
            vec3 velocity;
            glm_vec3_copy(direction, velocity);
            glm_vec3_scale(velocity, weapon->projectile_speed, velocity);

            entity_t* projectile = NULL;
            entity_init_projectile(sbox, "rocket", start, player->id,
                weapon->projectile_mesh, velocity,
                weapon->projectile_speed, weapon->damage, &projectile);
            projectile->data.projectile.materials[0] = weapon->projectile_material;
            glm_quat_copy(camera->rotation, projectile->rotation);
            entlist_add(sbox, &sbox->map.entlist, projectile);

        } else {
            float max_distance = 50.0f;
            trace_result_t trace;

            bool hit = phys_line_trace(sbox, start, direction, max_distance,
                entlist, player->id, &trace);
            if (hit) {
                sound_t* bullet_hit_sound = sbox->audio.bullet_hit_sounds[trace.phys_mat];
                a_play(sbox, &sbox->audio, bullet_hit_sound, trace.point, random(0.8f, 1.2f));
                r_add_partfx_shoot_hit(sbox, &sbox->renderer, trace);

                if (trace.water_level > 0.0f) {
                    r_add_partfx_shoot_hit_water(sbox, &sbox->renderer, trace);
                }

                if (trace.player_id != -1) {
                    player_add_damage(sbox, sbox->players[trace.player_id], weapon->damage);
                }
            }

            if (r_debug_draw_bullets.value) {
                vec3 end;
                glm_vec3_copy(start, end);

                float distance = max_distance;
                if (hit)
                    distance = trace.distance;
                
                end[0] += direction[0] * distance;
                end[1] += direction[1] * distance;
                end[2] += direction[2] * distance;

                r_add_line(sbox, &sbox->renderer, start, end, COLOR_LIGHT_BLUE, 2.5f);
            }

            vec3 beam_start;
            glm_vec3_copy(start, beam_start);
            
            vec3 forward;
            glm_vec3_copy(camera->forward, forward);
            glm_vec3_scale(forward, 0.25f, forward);

            glm_vec3_add(beam_start, forward, beam_start);

            r_add_partfx_shoot_beam(sbox, &sbox->renderer, beam_start, direction,
                (hit) ? trace.distance : max_distance);
        }
    }
}

static void tick_item_position(sbox_t* sbox, player_t* player) {
    vec3 target;
    if (player->buttons & PLAYER_BUTTON_AIM) {
        target[0] = 0.0f;
        target[1] = 0.0f;
        target[2] = 0.2f;
    } else {
        target[0] = VIEWMODEL_POS_X;
        target[1] = VIEWMODEL_POS_Y;
        target[2] = VIEWMODEL_POS_Z;
    }

    float aim_speed = 18.0f;
    player->item_position[0] = interp_to(player->item_position[0], target[0], aim_speed, sbox->dt);
    player->item_position[1] = interp_to(player->item_position[1], target[1], aim_speed, sbox->dt);
    player->item_position[2] = interp_to(player->item_position[2], target[2], aim_speed, sbox->dt);
}

static void tick_item_anim(sbox_t* sbox, player_t* player, item_t* item) {
    if (!item) return;

    if (sbox->time - player->inventory.last_switch < WEAPON_SWITCH_DELAY - 0.2f) {
        player->item_anim[1] = -0.65f;
        player->item_anim_angles[0] = -90.0f;
        player->item_anim_angles[1] = 35.0f;
        return;
    }

    if (item->type == ITEM_WEAPON && sbox->time - item->data.weapon.last_fire <
        min(item->data.weapon.fire_rate - 0.05f, 0.1f))
    {
        player->item_anim_angles[0] = (player->buttons & PLAYER_BUTTON_AIM) ?
            -3.0f : -15.0f;
    }

    if (player->move_mode == MOVE_SPRINT) {
        float anim_speed = 7.0f;
        player->item_anim_angles[0] = interp_to(player->item_anim_angles[0], 35.0f, anim_speed,
            sbox->dt);
        player->item_anim_angles[1] = interp_to(player->item_anim_angles[1], 10.0f, anim_speed,
            sbox->dt);
        player->item_anim_angles[2] = interp_to(player->item_anim_angles[2], 0.0f, anim_speed,
            sbox->dt);

        player->item_anim[1] = -0.2f;
        
    } else {
        float reset_speed = 7.0f;
        player->item_anim_angles[0] = interp_to(player->item_anim_angles[0], 0.0f, reset_speed,
            sbox->dt);
        player->item_anim_angles[1] = interp_to(player->item_anim_angles[1], 0.0f, reset_speed,
            sbox->dt);
        player->item_anim_angles[2] = interp_to(player->item_anim_angles[2], 0.0f, reset_speed,
            sbox->dt);
    }

    if (player->target_speed < 1.0f || !player->is_grounded) {
        float reset_speed = 5.0f;
        player->item_anim[0] = interp_to(player->item_anim[0], 0.0f, reset_speed, sbox->dt);
        player->item_anim[2] = interp_to(player->item_anim[2], 0.0f, reset_speed, sbox->dt);

        float y = clamp(-player->velocity[1], -0.024f, 0.024f);
        float y_speed = 7.0f;
        player->item_anim[1] = interp_to(player->item_anim[1], y, y_speed, sbox->dt);
        return;
    }

    vec3 anim;
    if (player->buttons & PLAYER_BUTTON_AIM) {
        anim[0] = 0.0f;
        anim[1] = 0.0f;
        anim[2] = 0.0f;
    } else {
        float speed = 1.0f;
        if (player->move_mode == MOVE_SPRINT)
            speed *= 2.0f;
        
        anim[0] = 0.0f;
        anim[1] = 0.0f;
        anim[2] = sin(player->walk_timer * 5.0f * speed) * 0.025f;
    }

    float set_speed = 6.5f;
    player->item_anim[0] = interp_to(player->item_anim[0], anim[0], set_speed, sbox->dt);
    player->item_anim[1] = interp_to(player->item_anim[1], anim[1], set_speed, sbox->dt);
    player->item_anim[2] = interp_to(player->item_anim[2], anim[2], set_speed, sbox->dt);
}

void player_tick_item(sbox_t* sbox, player_t* player) {
    tick_item(sbox, player, &sbox->map.entlist);
    tick_item_position(sbox, player);
    tick_item_anim(sbox, player, inventory_get_item(sbox, &player->inventory));
}

void player_render_item(sbox_t* sbox, player_t* player, renderer_t* renderer) {
    if (player->is_thirdperson || edit_mode.value || !r_viewmodel.value || !player->is_me) return;
    item_t* item = inventory_get_item(sbox, &player->inventory);
    if (!item) return;

    drawcall_t drawcall;
    drawcall.entity = NULL;
    
    drawcall.mesh = item->mesh;
    memcpy(drawcall.materials, item->materials, sizeof(material_t*) * MAX_MATERIALS);

    vec3 position;
    glm_vec3_copy(renderer->camera.position, position);

    vec3 forward;
    glm_vec3_copy(renderer->camera.forward, forward);
    glm_vec3_scale(forward, player->item_position[2] + player->item_anim[2], forward);
    glm_vec3_add(position, forward, position);
    
    vec3 right;
    glm_vec3_copy(renderer->camera.right, right);
    glm_vec3_scale(right, player->item_position[0] + player->item_anim[0], right);
    glm_vec3_add(position, right, position);

    vec3 up;
    glm_vec3_copy(renderer->camera.up, up);
    glm_vec3_scale(up, player->item_position[1] + player->item_anim[1], up);
    glm_vec3_add(position, up, position);

    vec3 scale = {1.0f, 1.0f, 1.0f};

    quat yaw;
    quat pitch;
    glm_quat(yaw, rad(-renderer->camera.angles[1] + 90.0f), 0.0f, 1.0f, 0.0f);
    glm_quat(pitch, rad(-renderer->camera.angles[0]), 1.0f, 0.0f, 0.0f);

    quat rotation;
    glm_quat_identity(rotation);
    glm_quat_mul(rotation, yaw, rotation);
    glm_quat_mul(rotation, pitch, rotation);

    quat anim_yaw;
    quat anim_pitch;
    quat anim_roll;
    glm_quat(anim_yaw, rad(player->item_anim_angles[1]), 0.0f, 1.0f, 0.0f);
    glm_quat(anim_pitch, rad(player->item_anim_angles[0]), 1.0f, 0.0f, 0.0f);
    glm_quat(anim_roll, rad(player->item_anim_angles[2]), 0.0f, 0.0f, 1.0f);

    glm_quat_mul(rotation, anim_pitch, rotation);
    glm_quat_mul(rotation, anim_yaw, rotation);
    glm_quat_mul(rotation, anim_roll, rotation);

    glm_mat4_identity(drawcall.model);
    glm_translate(drawcall.model, position);
    glm_scale(drawcall.model, scale);
    glm_quat_rotate(drawcall.model, rotation, drawcall.model);

    drawcall.local_bbox = drawcall.mesh->bbox;
    drawcall.world_bbox = bbox_translate(&drawcall.local_bbox, position);

    glm_vec3_copy(position, drawcall.position);
    glm_vec3_copy(scale, drawcall.scale);
    glm_quat_rotate(GLM_MAT4_IDENTITY, rotation, drawcall.rotation);

    drawcall.distance_to_camera = 0.0f;
    drawcall.is_translucent = false;
    r_add_drawcall(renderer, drawcall);
}
