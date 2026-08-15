#include "player.h"
#include "../shared/quark.h"

static void reset_input(quark_t* quark, player_t* player) {
    glm_vec3_zero(player->move_input);
    glm_vec3_zero(player->target_dir);
    player->buttons = 0;
}

static void camera_controls(quark_t* quark, player_t* player, camera_t* camera) {
    if (quark->keys[SDL_SCANCODE_Z])
        camera_add_pitch(camera, -m_sens.value);

    if (quark->keys[SDL_SCANCODE_X])
        camera_add_pitch(camera, m_sens.value);

    if (quark->keys[SDL_SCANCODE_LEFT])
        camera_add_yaw(camera, -m_sens.value);

    if (quark->keys[SDL_SCANCODE_RIGHT])
        camera_add_yaw(camera, m_sens.value);
}

void player_input(quark_t* quark, player_t* player) {
    reset_input(quark, player);
    if (!player->is_me || SDL_IsTextInputActive()) return;
    camera_t* camera = &quark->renderer.camera;

    if (player_is_dead(player)) {
        if (quark->keys[SDL_SCANCODE_SPACE]) {
            quark->keys[SDL_SCANCODE_SPACE] = false;
            gm_respawn_player(quark, player);
        }

        camera_controls(quark, player, camera);
        return;
    }

    if (quark->ui_state != UI_STATE_IN_GAME)
        return;

    if (quark->keys[SDL_SCANCODE_F]) {
        quark->keys[SDL_SCANCODE_F] = false;
        player->buttons |= PLAYER_BUTTON_INTERACT;
    }

    if (player->vehicle != -1) {
        camera_controls(quark, player, camera);

        entity_t* vehicle = quark->map.entlist.ents[player->vehicle];

        if (quark->keys[SDL_SCANCODE_W] || quark->keys[SDL_SCANCODE_UP])
            vehicle->velocity[1] += 2.0f * quark->dt;

        if (quark->keys[SDL_SCANCODE_S] || quark->keys[SDL_SCANCODE_UP])
            vehicle->velocity[1] -= 2.0f * quark->dt;

        return;
    }
    
    camera_controls(quark, player, camera);

    if (quark->keys[SDL_SCANCODE_V]) {
        quark->keys[SDL_SCANCODE_V] = false;
        player_add_damage(quark, player, 10.0f);
    }

    /*if (quark->mxdt != 0.0f)
        camera_add_yaw(camera, quark->mxdt * m_sens.value);

    if (quark->mydt != 0.0f)
        camera_add_pitch(camera, quark->mydt * -m_sens.value);*/
    
    if (quark->keys[SDL_SCANCODE_W] || quark->keys[SDL_SCANCODE_UP])
        player->move_input[2] += 1.0f;

    if (quark->keys[SDL_SCANCODE_S] || quark->keys[SDL_SCANCODE_DOWN])
        player->move_input[2] -= 1.0f;

    if (quark->keys[SDL_SCANCODE_D])
        player->move_input[0] -= 1.0f;

    if (quark->keys[SDL_SCANCODE_A])
        player->move_input[0] += 1.0f;

    if (quark->keys[SDL_SCANCODE_SPACE]) {
        player->buttons |= PLAYER_BUTTON_JUMP;
        if (player->move_mode == MOVE_FLIGHT)
            player->move_input[1] += 1.0f;
    }
    
    if (quark->keys[SDL_SCANCODE_LCTRL]) {
        player->buttons |= PLAYER_BUTTON_CROUCH;
        if (player->move_mode == MOVE_FLIGHT)
            player->move_input[1] -= 1.0f;
    }

    if (quark->keys[SDL_SCANCODE_LSHIFT]) {
        player->buttons |= PLAYER_BUTTON_SPRINT;
    }

    if (quark->keys[SDL_SCANCODE_E] || quark->keys[SDL_SCANCODE_RCTRL])
        player->buttons |= PLAYER_BUTTON_FIRE;

    if (quark->keys[SDL_SCANCODE_R]) {
        quark->keys[SDL_SCANCODE_R] = false;
        item_t* item = inventory_get_item(quark, &quark->player->inventory);
        weapon_reload(quark, &item->data.weapon, player);
    }

    if (quark->keys[SDL_SCANCODE_LALT])
        player->buttons |= PLAYER_BUTTON_AIM;

    if (quark->keys[SDL_SCANCODE_B]) {
        quark->keys[SDL_SCANCODE_B] = false;
        cvar_toggle(quark, "edit_mode");
    }

    if (quark->keys[SDL_SCANCODE_C]) {
        quark->keys[SDL_SCANCODE_C] = false;
        player->is_thirdperson = !player->is_thirdperson;
    }

    if (quark->keys[SDL_SCANCODE_1]) {
        quark->keys[SDL_SCANCODE_1] = false;
        inventory_select_hotbar_slot(quark, &player->inventory, 0);
    }

    if (quark->keys[SDL_SCANCODE_2]) {
        quark->keys[SDL_SCANCODE_2] = false;
        inventory_select_hotbar_slot(quark, &player->inventory, 1);
    }

    if (quark->keys[SDL_SCANCODE_3]) {
        quark->keys[SDL_SCANCODE_3] = false;
        inventory_select_hotbar_slot(quark, &player->inventory, 2);
    }

    if (quark->keys[SDL_SCANCODE_4]) {
        quark->keys[SDL_SCANCODE_4] = false;
        inventory_select_hotbar_slot(quark, &player->inventory, 3);
    }

    if (quark->keys[SDL_SCANCODE_5]) {
        quark->keys[SDL_SCANCODE_5] = false;
        inventory_select_hotbar_slot(quark, &player->inventory, 4);
    }

    if (quark->keys[SDL_SCANCODE_6]) {
        quark->keys[SDL_SCANCODE_6] = false;
        inventory_select_hotbar_slot(quark, &player->inventory, 5);
    }

    if (quark->keys[SDL_SCANCODE_I]) {
        quark->keys[SDL_SCANCODE_I] = false;
        inventory_toggle(quark, &player->inventory);
    }

    if (quark->keys[SDL_SCANCODE_N]) {
        quark->keys[SDL_SCANCODE_N] = false;
        cvar_toggle(quark, "noclip");
    }

    if (quark->keys[SDL_SCANCODE_J]) {
        quark->keys[SDL_SCANCODE_J] = false;
        const char* args[] = {"spawn"};
        cmd_run(quark, "bot", args, 1);
    }

    if (quark->keys[SDL_SCANCODE_G]) {
        quark->keys[SDL_SCANCODE_G] = false;
        if (sv_timescale.value == 1.0f)
            cvar_set(quark, "sv_timescale", "0.1f");
        else
            cvar_set(quark, "sv_timescale", "1.0f");
    }
}
