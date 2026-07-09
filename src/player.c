#include "player.h"
#include "sbox.h"
#include "render.h"

#define P_GRAVITY 8
#define P_STOPSPEED 100
#define P_MAXSPEED_WALK 240.0f
#define P_MAXSPEED_SPRINT 360.0f
#define P_MAXSPEED_CROUCH 140.0f
#define P_ACCELERATE 10
#define P_FRICTION 4
#define P_STOPSPEED 100

static float get_max_speed(sbox_t* sbox, player_t* player);
static float get_camera_height(sbox_t* sbox, player_t* player);

void player_init(sbox_t* sbox, player_t* player) {
    vec3 position = {0.0f, 0.0f, -2.5f};
    glm_vec3_copy(position, player->position);
    player->move_mode = MOVE_WALK;
    glm_vec3_copy((vec3)GLM_VEC3_ZERO_INIT, player->wishdir);
    glm_vec3_copy((vec3)GLM_VEC3_ZERO_INIT, player->wishvel);
    glm_vec3_copy((vec3)GLM_VEC3_ZERO_INIT, player->velocity);
    player->wishspeed = get_max_speed(sbox, player);
    player->camera_height = get_camera_height(sbox, player);
}

static void set_move_mode(player_t* player, move_mode_t move_mode) {
    player->move_mode = move_mode;
}

static float get_max_speed(sbox_t* sbox, player_t* player) {
    float speed;
    switch (player->move_mode) {
    case MOVE_WALK: speed = P_MAXSPEED_WALK; break;
    case MOVE_SPRINT: speed = P_MAXSPEED_SPRINT; break;
    case MOVE_CROUCH: speed = P_MAXSPEED_CROUCH; break;
    default: unreachable(sbox);
    }
    
    return speed;
}

static float get_camera_height(sbox_t* sbox, player_t* player) {
    switch (player->move_mode) {
    case MOVE_WALK:
    case MOVE_SPRINT: return 1.8f;
    case MOVE_CROUCH: return 0.8f;
    default: unreachable(sbox);
    }

    return 0.0f;
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
    
    set_move_mode(player, MOVE_WALK);

    if (sbox->keys[SDL_SCANCODE_LCTRL])
        set_move_mode(player, MOVE_CROUCH);

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
    player->velocity[0] = player->wishdir[0] * player->wishspeed * sbox->dt;
    player->velocity[2] = player->wishdir[2] * player->wishspeed * sbox->dt;
}

static void tick_camera(sbox_t* sbox, player_t* player, camera_t* camera) {
    player->camera_height = get_camera_height(sbox, player) / 2.0f;

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

    float new_speed = speed - drop;
    if (new_speed < 0.0f)
        new_speed = 0.0f;
    new_speed /= speed;

    for (int i = 0; i < 3; i++)
        player->velocity[i] *= new_speed;
}

void accelerate(sbox_t* sbox, player_t* player) {
    float current_speed = glm_vec3_dot(player->velocity, player->wishdir);

    float add_speed = player->wishspeed - current_speed;
    if (add_speed <= 0.0f)
        return;

    float accel_speed = P_ACCELERATE * sbox->dt * player->wishspeed;
    if (accel_speed > add_speed)
        accel_speed = add_speed;    

    for (int i = 0; i < 3; i++)
        player->velocity[i] += player->wishdir[i] * accel_speed;
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

    float max_speed = get_max_speed(sbox, player);
    if (player->wishspeed > max_speed) {
        glm_vec3_scale(player->wishvel, max_speed / player->wishspeed, player->wishvel);
        player->wishspeed = max_speed;
    }

    accelerate(sbox, player);
    apply_friction(sbox, player);
}

void player_tick(sbox_t* sbox, player_t* player, camera_t* camera) {
    tick_input(sbox, player, camera);
    tick_camera(sbox, player, camera);
    move_ground(sbox, player);
}
