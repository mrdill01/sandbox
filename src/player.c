#include "player.h"
#include "sbox.h"
#include "render.h"

#define P_GRAVITY 8
#define P_STOPSPEED 100
#define P_MAXSPEED 360.0f
#define P_ACCELERATE 10
#define P_FRICTION 4
#define P_STOPSPEED 100

void player_init(player_t* player) {
    vec3 position = {0.0f, 0.0f, -2.5f};
    glm_vec3_copy(position, player->position);
    glm_vec3_copy((vec3)GLM_VEC3_ZERO_INIT, player->wishdir);
    glm_vec3_copy((vec3)GLM_VEC3_ZERO_INIT, player->velocity);
    player->speed = P_MAXSPEED;
    player->camera_height = 1.8f / 2.0f;
}

static void tick_input(sbox_t* sbox, player_t* player, camera_t* camera) {
    if (sbox->keys[SDL_SCANCODE_Z])
        camera_add_pitch(camera, -m_sens.value);

    if (sbox->keys[SDL_SCANCODE_X])
        camera_add_pitch(camera, m_sens.value);

    if (sbox->keys[SDL_SCANCODE_LEFT])
        camera_add_yaw(camera, m_sens.value);

    if (sbox->keys[SDL_SCANCODE_RIGHT])
        camera_add_yaw(camera, -m_sens.value);
    
    glm_vec3_copy((vec3)GLM_VEC3_ZERO_INIT, player->wishdir);

    if (sbox->keys[SDL_SCANCODE_W] || sbox->keys[SDL_SCANCODE_UP])
        glm_vec3_add(player->wishdir, camera->forward, player->wishdir);

    if (sbox->keys[SDL_SCANCODE_S] || sbox->keys[SDL_SCANCODE_DOWN])
        glm_vec3_sub(player->wishdir, camera->forward, player->wishdir);

    if (sbox->keys[SDL_SCANCODE_D])
        glm_vec3_add(player->wishdir, camera->right, player->wishdir);

    if (sbox->keys[SDL_SCANCODE_A])
        glm_vec3_sub(player->wishdir, camera->right, player->wishdir);

    glm_vec3_normalize(player->wishdir);
    player->velocity[0] = player->wishdir[0] * player->speed * sbox->dt;
    player->velocity[2] = player->wishdir[2] * player->speed * sbox->dt;
}

static void tick_camera(sbox_t* sbox, player_t* player, camera_t* camera) {
    camera->position[0] = player->position[0];
    camera->position[1] = player->position[1] + player->camera_height;
    camera->position[2] = player->position[2];

    camera_tick(sbox, camera);
}

void apply_friction(sbox_t* sbox, player_t* player) {
    float speed = sqrt(player->velocity[0] * player->velocity[0] +
        player->velocity[1] * player->velocity[1] +
        player->velocity[2] * player->velocity[2]);

    if (speed < 1.0f) {
        player->velocity[0] = 0.0f;
        player->velocity[2] = 0.0f;
        return;
    }

    float control = (speed < P_STOPSPEED) ? P_STOPSPEED : speed;
    float drop = control * P_FRICTION * sbox->dt;

    float newspeed = speed - drop;
    if (newspeed < 0.0f)
        newspeed = 0.0f;
    newspeed /= speed;

    player->velocity[0] *= newspeed;
    player->velocity[1] *= newspeed;
    player->velocity[2] *= newspeed;
}

void move_ground(sbox_t* sbox, player_t* player) {
    player->velocity[1] = 0.0f;
    if (player->velocity[0] == 0.0f && player->velocity[1] == 0.0f && player->velocity[2] == 0.0f)
        return;

    vec3 move;
    move[0] = player->position[0] + player->velocity[0] * sbox->dt;
    move[1] = player->position[1]; 
    move[2] = player->position[2] + player->velocity[2] * sbox->dt;
    glm_vec3_copy(move, player->position);

    apply_friction(sbox, player);
}

void player_tick(sbox_t* sbox, player_t* player, camera_t* camera) {
    tick_input(sbox, player, camera);
    tick_camera(sbox, player, camera);
    move_ground(sbox, player);
}
