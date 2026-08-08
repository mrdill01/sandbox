#include "player.h"
#include "sbox.h"

static void reset_input(sbox_t* sbox, player_t* player) {
    glm_vec3_zero(player->move_input);
    glm_vec3_zero(player->target_dir);
    player->buttons = 0;
}

void player_input(sbox_t* sbox, player_t* player) {
    reset_input(sbox, player);
    if (!player->is_me || SDL_IsTextInputActive()) return;
    
    if (player_is_dead(player)) {
        if (sbox->keys[SDL_SCANCODE_SPACE]) {
            sbox->keys[SDL_SCANCODE_SPACE] = false;
            gm_respawn_player(sbox, player);
        }

        return;
    }

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

    if (sbox->keys[SDL_SCANCODE_V]) {
        sbox->keys[SDL_SCANCODE_V] = false;
        player_add_damage(sbox, player, 10.0f);
    }

    /*if (sbox->mxdt != 0.0f)
        camera_add_yaw(camera, sbox->mxdt * m_sens.value);

    if (sbox->mydt != 0.0f)
        camera_add_pitch(camera, sbox->mydt * -m_sens.value);*/
    
    if (sbox->keys[SDL_SCANCODE_W] || sbox->keys[SDL_SCANCODE_UP])
        player->move_input[2] += 1.0f;

    if (sbox->keys[SDL_SCANCODE_S] || sbox->keys[SDL_SCANCODE_DOWN])
        player->move_input[2] -= 1.0f;

    if (sbox->keys[SDL_SCANCODE_D])
        player->move_input[0] -= 1.0f;

    if (sbox->keys[SDL_SCANCODE_A])
        player->move_input[0] += 1.0f;

    if (sbox->keys[SDL_SCANCODE_SPACE])
        player->buttons |= PLAYER_BUTTON_JUMP;
    
    if (sbox->keys[SDL_SCANCODE_LCTRL]) {
        player->buttons |= PLAYER_BUTTON_CROUCH;
        player->move_input[1] -= 1.0f;
    }

    if (sbox->keys[SDL_SCANCODE_LSHIFT]) {
        player->buttons |= PLAYER_BUTTON_SPRINT;
        player->move_input[1] += 1.0f;
    }

    if (sbox->keys[SDL_SCANCODE_E] || sbox->keys[SDL_SCANCODE_RCTRL])
        player->buttons |= PLAYER_BUTTON_FIRE;

    if (sbox->keys[SDL_SCANCODE_LALT])
        player->buttons |= PLAYER_BUTTON_AIM;

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

    if (sbox->keys[SDL_SCANCODE_N]) {
        sbox->keys[SDL_SCANCODE_N] = false;
        cvar_toggle(sbox, "noclip");
    }

    if (sbox->keys[SDL_SCANCODE_J]) {
        sbox->keys[SDL_SCANCODE_J] = false;
        const char* args[] = {"spawn"};
        cmd_run(sbox, "bot", args, 1);
    }

    if (sbox->keys[SDL_SCANCODE_G]) {
        sbox->keys[SDL_SCANCODE_G] = false;
        if (sv_timescale.value == 1.0f)
            cvar_set(sbox, "sv_timescale", "0.1f");
        else
            cvar_set(sbox, "sv_timescale", "1.0f");
    }
}
