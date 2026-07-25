#include "player.h"
#include "sbox.h"

static void reset_input(sbox_t* sbox, player_t* player) {
    glm_vec3_zero(player->move_input);
    glm_vec3_zero(player->target_dir);
    player->buttons = 0;
}

void player_input(sbox_t* sbox, player_t* player) {
    reset_input(sbox, player);
    if (sbox->ui_state != UI_STATE_IN_GAME)
        return;

    camera_t* camera = &sbox->renderer.camera;
    
    if (sbox->keys[SDL_SCANCODE_Z])
        camera_add_pitch(camera, -m_sens.value);

    if (sbox->keys[SDL_SCANCODE_X])
        camera_add_pitch(camera, m_sens.value);

    if (sbox->keys[SDL_SCANCODE_LEFT])
        camera_add_yaw(camera, -m_sens.value);

    if (sbox->keys[SDL_SCANCODE_RIGHT])
        camera_add_yaw(camera, m_sens.value);

    /*if (sbox->mxdt != 0.0f)
        camera_add_yaw(camera, sbox->mxdt * m_sens.value);

    if (sbox->mydt != 0.0f)
        camera_add_pitch(camera, sbox->mydt * -m_sens.value);*/
    
    if (sbox->keys[SDL_SCANCODE_W] || sbox->keys[SDL_SCANCODE_UP]) {
        player->move_input[2] += 1.0f;
        glm_vec3_add(player->target_dir, camera->forward, player->target_dir);
    }

    if (sbox->keys[SDL_SCANCODE_S] || sbox->keys[SDL_SCANCODE_DOWN]) {
        player->move_input[2] -= 1.0f;
        glm_vec3_sub(player->target_dir, camera->forward, player->target_dir);
    }

    if (sbox->keys[SDL_SCANCODE_D]) {
        player->move_input[0] -= 1.0f;
        glm_vec3_sub(player->target_dir, camera->right, player->target_dir);
    }

    if (sbox->keys[SDL_SCANCODE_A]) {
        player->move_input[0] += 1.0f;
        glm_vec3_add(player->target_dir, camera->right, player->target_dir);
    }

    if (glm_vec3_dot(player->move_input, player->move_input) > 0.0f)
        glm_vec3_norm(player->move_input);

    if (sbox->keys[SDL_SCANCODE_SPACE])
        player->buttons |= PLAYER_BUTTON_JUMP;
    
    if (sbox->keys[SDL_SCANCODE_LCTRL])
        player->buttons |= PLAYER_BUTTON_CROUCH;

    if (sbox->keys[SDL_SCANCODE_E] || sbox->keys[SDL_SCANCODE_RCTRL]) {
        player->buttons |= PLAYER_BUTTON_FIRE;
    }

    if (sbox->keys[SDL_SCANCODE_G]) {
        sbox->keys[SDL_SCANCODE_G] = false;
        player_t* bot = gm_spawn_player(sbox);
        player_teleport(sbox, bot, player->look_trace.point);
    }

    if (sbox->keys[SDL_SCANCODE_LALT]) {
        player->buttons |= PLAYER_BUTTON_AIM;
    }

    if (sbox->keys[SDL_SCANCODE_B]) {
        sbox->keys[SDL_SCANCODE_B] = false;
        cvar_toggle(sbox, "edit_mode");
    }

    if (sbox->keys[SDL_SCANCODE_C]) {
        sbox->keys[SDL_SCANCODE_C] = false;
        player->is_thirdperson = !player->is_thirdperson;
    }

    if (sbox->keys[SDL_SCANCODE_1]) {
        sbox->keys[SDL_SCANCODE_1] = false;
        inventory_select_hotbar_slot(sbox, &player->inventory, 0);
    }

    if (sbox->keys[SDL_SCANCODE_2]) {
        sbox->keys[SDL_SCANCODE_2] = false;
        inventory_select_hotbar_slot(sbox, &player->inventory, 1);
    }

    if (sbox->keys[SDL_SCANCODE_3]) {
        sbox->keys[SDL_SCANCODE_3] = false;
        inventory_select_hotbar_slot(sbox, &player->inventory, 2);
    }

    if (sbox->keys[SDL_SCANCODE_4]) {
        sbox->keys[SDL_SCANCODE_4] = false;
        inventory_select_hotbar_slot(sbox, &player->inventory, 3);
    }

    if (sbox->keys[SDL_SCANCODE_I]) {
        sbox->keys[SDL_SCANCODE_I] = false;
        inventory_toggle(sbox, &player->inventory);
    }
}
