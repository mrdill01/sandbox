#include "player.h"
#include "sbox.h"
#include "video.h"

#define P_GRAVITY 8
#define P_STOPSPEED 100
#define P_MAXSPEED 320
#define P_ACCELERATE 10
#define P_FRICTION 6

void player_init(player_t* player) {
    glm_vec3_copy((vec3)GLM_VEC3_ZERO_INIT, player->position);
    glm_vec3_copy((vec3)GLM_VEC3_ZERO_INIT, player->wishdir);
    glm_vec3_copy((vec3)GLM_VEC3_ZERO_INIT, player->velocity);
    player->speed = 1.5f;
    player->camera_height = 1.8f / 2.0f;
}

static void tick_input(sbox_t* sbox, player_t* player, camera_t* camera) {
    if (sbox->keys[SDL_SCANCODE_Z])
        camera_add_pitch(camera, -sbox->cfg.m_sens);

    if (sbox->keys[SDL_SCANCODE_X])
        camera_add_pitch(camera, sbox->cfg.m_sens);

    if (sbox->keys[SDL_SCANCODE_LEFT])
        camera_add_yaw(camera, -sbox->cfg.m_sens);

    if (sbox->keys[SDL_SCANCODE_RIGHT])
        camera_add_yaw(camera, sbox->cfg.m_sens);
    
    if (sbox->keys[SDL_SCANCODE_W] || sbox->keys[SDL_SCANCODE_UP]) {
        vec3 move;
        glm_vec3_copy(camera->forward, move);
        glm_vec3_scale(move, player->speed * sbox->dt, move);
        glm_vec3_add(player->position, move, player->position);
    }

    if (sbox->keys[SDL_SCANCODE_S] || sbox->keys[SDL_SCANCODE_DOWN]) {
        vec3 move;
        glm_vec3_copy(camera->forward, move);
        glm_vec3_scale(move, player->speed * sbox->dt, move);
        glm_vec3_sub(player->position, move, player->position);
    }

    if (sbox->keys[SDL_SCANCODE_D]) {
        vec3 move;
        glm_vec3_copy(camera->right, move);
        glm_vec3_scale(move, player->speed * sbox->dt, move);
        glm_vec3_sub(player->position, move, player->position);
    }

    if (sbox->keys[SDL_SCANCODE_A]) {
        vec3 move;
        glm_vec3_copy(camera->right, move);
        glm_vec3_scale(move, player->speed * sbox->dt, move);
        glm_vec3_add(player->position, move, player->position);
    }
}

static void tick_camera(sbox_t* sbox, player_t* player, camera_t* camera) {
    camera->position[0] = player->position[0];
    camera->position[1] = player->position[1] + player->camera_height;
    camera->position[2] = player->position[2];

    camera_tick(sbox, camera);
}

void move_ground(sbox_t* sbox, player_t* player) {
    player->velocity[1] = 0.0f;
    if (player->velocity[0] == 0.0f && player->velocity[1] == 0.0f && player->velocity[2] == 0.0f)
        return;

    vec3 dest;
    dest[0] = player->position[0] + player->velocity[0] * sbox->dt;
    dest[1] = player->position[1]; 
    dest[2] = player->position[2] + player->velocity[2] * sbox->dt; 
}

void player_tick(sbox_t* sbox, player_t* player, camera_t* camera) {
    tick_input(sbox, player, camera);
    tick_camera(sbox, player, camera);
    move_ground(sbox, player);
}
