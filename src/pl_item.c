#include "player.h"
#include "quark.h"

#define VIEWMODEL_POS_X -0.05f
#define VIEWMODEL_POS_Y -0.1f
#define VIEWMODEL_POS_Z 0.35f

static void tick_item(quark_t* quark, player_t* player, entlist_t* entlist) {
    item_t* item = inventory_get_item(quark, &player->inventory);
    if (!item)
        return;
    
    weapon_t* weapon = &item->data.weapon;
    if (player->buttons & PLAYER_BUTTON_FIRE) {
        weapon_fire(quark, weapon, player);
    }

    if (weapon->is_reloading && quark->time - weapon->reload_start >= weapon->reload_time) {
        weapon_finish_reload(quark, weapon, player);
    }
}

static void tick_item_position(quark_t* quark, player_t* player) {
    vec3 target;

    item_t* item = inventory_get_item(quark, &quark->player->inventory);
    if (item->data.weapon.is_reloading) {
        target[1] = -1.0f;
    } else if (player->buttons & PLAYER_BUTTON_AIM) {
        target[0] = 0.0f;
        target[1] = 0.0f;
        target[2] = 0.2f;
    } else {
        target[0] = VIEWMODEL_POS_X;
        target[1] = VIEWMODEL_POS_Y;
        target[2] = VIEWMODEL_POS_Z;
    }

    float aim_speed = 18.0f;
    player->item_position[0] = interp_to(player->item_position[0], target[0], aim_speed, quark->dt);
    player->item_position[1] = interp_to(player->item_position[1], target[1], aim_speed, quark->dt);
    player->item_position[2] = interp_to(player->item_position[2], target[2], aim_speed, quark->dt);
}

static void tick_item_anim(quark_t* quark, player_t* player, item_t* item) {
    if (!item) return;

    if (quark->time - player->inventory.last_switch < WEAPON_SWITCH_DELAY - 0.2f) {
        player->item_anim[1] = -0.65f;
        player->item_anim_angles[0] = -90.0f;
        player->item_anim_angles[1] = 35.0f;
        return;
    }

    if (item->type == ITEM_WEAPON && quark->time - item->data.weapon.last_fire <
        min(item->data.weapon.fire_rate - 0.05f, 0.1f))
    {
        player->item_anim_angles[0] = (player->buttons & PLAYER_BUTTON_AIM) ?
            -3.0f : -15.0f;
    }

    if (player->move_mode == MOVE_SPRINT) {
        float anim_speed = 7.0f;
        player->item_anim_angles[0] = interp_to(player->item_anim_angles[0], 35.0f, anim_speed,
            quark->dt);
        player->item_anim_angles[1] = interp_to(player->item_anim_angles[1], 10.0f, anim_speed,
            quark->dt);
        player->item_anim_angles[2] = interp_to(player->item_anim_angles[2], 0.0f, anim_speed,
            quark->dt);

        player->item_anim[1] = -0.2f;
        
    } else {
        float reset_speed = 7.0f;
        player->item_anim_angles[0] = interp_to(player->item_anim_angles[0], 0.0f, reset_speed,
            quark->dt);
        player->item_anim_angles[1] = interp_to(player->item_anim_angles[1], 0.0f, reset_speed,
            quark->dt);
        player->item_anim_angles[2] = interp_to(player->item_anim_angles[2], 0.0f, reset_speed,
            quark->dt);
    }



    if (player->target_speed < 1.0f || !player->is_grounded) {
        float reset_speed = 5.0f;
        player->item_anim[0] = interp_to(player->item_anim[0], 0.0f, reset_speed, quark->dt);
        player->item_anim[2] = interp_to(player->item_anim[2], 0.0f, reset_speed, quark->dt);

        float y = clamp(-player->velocity[1], -0.024f, 0.024f);
        float y_speed = 7.0f;
        player->item_anim[1] = interp_to(player->item_anim[1], y, y_speed, quark->dt);
        return;
    }

    vec3 anim;
    if (player->buttons & PLAYER_BUTTON_AIM) {
        anim[0] = 0.0f;
        anim[1] = 0.0f;
        anim[2] = 0.0f;
    } else if (player_get_xz_speed(quark, player) > 0.0f && player->is_grounded) {
        float speed = 1.0f;
        if (player->move_mode == MOVE_SPRINT)
            speed *= 2.0f;
        
        anim[0] = 0.0f;
        anim[1] = 0.0f;
        anim[2] = sin(player->walk_timer * 5.0f * speed) * 0.025f;
    }

    float set_speed = 6.5f;
    player->item_anim[0] = interp_to(player->item_anim[0], anim[0], set_speed, quark->dt);
    player->item_anim[1] = interp_to(player->item_anim[1], anim[1], set_speed, quark->dt);
    player->item_anim[2] = interp_to(player->item_anim[2], anim[2], set_speed, quark->dt);
}

void player_tick_item(quark_t* quark, player_t* player) {
    tick_item(quark, player, &quark->map.entlist);
    tick_item_position(quark, player);
    tick_item_anim(quark, player, inventory_get_item(quark, &player->inventory));
}

void player_render_item(quark_t* quark, player_t* player, renderer_t* renderer) {
    if (player->is_thirdperson ||
        player->vehicle ||
        edit_mode.value ||
        !r_viewmodel.value ||
        !player->is_me) return;
    item_t* item = inventory_get_item(quark, &player->inventory);
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
