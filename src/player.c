#include "player.h"
#include "sbox.h"
#include "physics.h"

#define P_GRAVITY 9.81f
#define P_JUMPFORCE 2.8f
#define P_MAXSPEED_WALK 3.2f
#define P_MAXSPEED_SPRINT 16.0f
#define P_MAXSPEED_CROUCH 1.8f
#define P_ACCEL 10.0f
#define P_AIR_ACCEL 2.0f
#define P_STOPSPEED 2.5f
#define P_AIR_STOPSPEED 2.0f
#define P_FRICTION 5.0f
#define P_AIR_CONTROL 0.0f

static float get_max_speed(sbox_t* sbox, player_t* player);
static float get_height(sbox_t* sbox, player_t* player);

void player_init(sbox_t* sbox, player_t* player) {
    player->move_mode = MOVE_WALK;
    glm_vec3_copy((vec3){0.0f, 0.0f, -4.5f}, player->position);
    glm_vec3_copy((vec3)GLM_VEC3_ZERO_INIT, player->velocity);
    glm_vec3_copy((vec3)GLM_VEC3_ZERO_INIT, player->move_input);
    player->target_speed = get_max_speed(sbox, player);
    player->is_grounded = true;
    player->water_level = 0.0f;
    player->last_step_time = 0.0f;
    player->height = get_height(sbox, player);
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

static float get_height(sbox_t* sbox, player_t* player) {
    switch (player->move_mode) {
    case MOVE_WALK:
    case MOVE_SPRINT: return 1.6f;
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

    glm_vec3_copy((vec3)GLM_VEC3_ZERO_INIT, player->move_input);

    if (sbox->keys[SDL_SCANCODE_W] || sbox->keys[SDL_SCANCODE_UP])
        glm_vec3_add(player->move_input, camera->forward, player->move_input);

    if (sbox->keys[SDL_SCANCODE_S] || sbox->keys[SDL_SCANCODE_DOWN])
        glm_vec3_sub(player->move_input, camera->forward, player->move_input);

    if (sbox->keys[SDL_SCANCODE_D])
        glm_vec3_add(player->move_input, camera->right, player->move_input);

    if (sbox->keys[SDL_SCANCODE_A])
        glm_vec3_sub(player->move_input, camera->right, player->move_input);

    player->pressed_jump = sbox->keys[SDL_SCANCODE_SPACE];
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
    
    if (speed > 0.0f)
        new_speed /= speed;

    player->velocity[0] *= new_speed;
    player->velocity[2] *= new_speed;
}

void accelerate(sbox_t* sbox, player_t* player, float target_speed, float accel) {
    float current_speed = glm_vec3_dot(player->velocity, player->move_input);
    float add_speed = target_speed - current_speed;
    if (add_speed <= 0)
        return;

    float accel_speed = accel * target_speed * sbox->dt;
    if (accel_speed > add_speed)
        accel_speed = add_speed;

    player->velocity[0] += accel_speed * player->move_input[0];
    player->velocity[2] += accel_speed * player->move_input[2];
}

void move_ground(sbox_t* sbox, player_t* player) {
    player->velocity[1] = 0.0f;
    apply_friction(sbox, player);
    accelerate(sbox, player, player->target_speed, P_ACCEL);

    if (player->pressed_jump) {
        player->velocity[1] = P_JUMPFORCE;
    }
}

void air_control(sbox_t* sbox, player_t* player, float target_speed) {
    if (fabs(target_speed) < 0.001f)
        return;
    
    float y_speed = player->velocity[1];
    player->velocity[1] = 0.0f;
    float speed = glm_vec3_norm(player->velocity);

    float dot = glm_vec3_dot(player->velocity, player->move_input);
    float k = 32;
    k *= P_AIR_CONTROL * dot * dot * sbox->dt;

    if (dot > 0.0f) {
        for (int i = 0; i < 3; i++)
            player->velocity[i] *= speed + player->move_input[i] * k;
        glm_vec3_norm(player->velocity);
    }

    player->velocity[0] *= speed;
    player->velocity[1] = y_speed;
    player->velocity[2] *= speed;
}

void move_air(sbox_t* sbox, player_t* player) {
    float accel;
    if (glm_vec3_dot(player->velocity, player->move_input) < 0.0f)
        accel = P_AIR_STOPSPEED;
    else
        accel = P_AIR_ACCEL;
    
    accelerate(sbox, player, player->target_speed, accel);

    if (P_AIR_CONTROL > 0.0f)
        air_control(sbox, player, player->target_speed);

    player->velocity[1] -= P_GRAVITY * sbox->dt;
}

static void hit_ground(sbox_t* sbox, player_t* player) {
    a_play(sbox, &sbox->audio, sbox->audio.jump_land_sound, 1.0f);
    player->last_step_time = sbox->time;
}

static void leave_ground(sbox_t* sbox, player_t* player) {
    if (player->pressed_jump)
       a_play(sbox, &sbox->audio, sbox->audio.jump_sound, random(0.85f, 1.15f));
}

static void enter_water(sbox_t* sbox, player_t* player) {
    //a_play(sbox, &sbox->audio, sbox->audio.enter_water_sound);
    printf("enter water\n");
}

static void exit_water(sbox_t* sbox, player_t* player) {
    printf("exit water\n");
}

void move_and_collide(sbox_t* sbox, player_t* player, entlist_t* entlist) {
    for (int i = 0; i < 3; i++)
        player->position[i] += player->velocity[i] * sbox->dt;

    vec3 start;
    player_get_top_position(sbox, player, start);
    vec3 dir = {0.0f, -1.0f, 0.0f};
    float max_distance = get_height(sbox, player);
    trace_result_t trace;
    
    bool was_grounded = player->is_grounded;
    bool was_in_water = player->water_level > 0.0f;

    if (phys_line_trace(start, dir, max_distance, entlist, &trace)) {
        player->position[1] = trace.point[1] + get_height(sbox, player) / 2.0f;
        player->is_grounded = true;
        if (!was_grounded)
            hit_ground(sbox, player);

        player->water_level = trace.water_level;
        if (player->water_level > 0.0f && !was_in_water)
            enter_water(sbox, player);
        
    } else {
        if (player->position[1] < -1.5f) {
            player->position[1] = -1.5f;
            player->is_grounded = true;
            if (!was_grounded)
                hit_ground(sbox, player);

        } else {
            player->is_grounded = false;
            if (was_grounded)
                leave_ground(sbox, player);

            player->water_level = 0.0f;
            if (was_in_water)
                exit_water(sbox, player);
        }
    }
    //printf("%g\n", player->water_level);
}

static void tick_camera(sbox_t* sbox, player_t* player, camera_t* camera) {
    player->height = get_height(sbox, player) / 2.0f;

    camera->position[0] = player->position[0];
    camera->position[1] = player->position[1] + player->height;
    camera->position[2] = player->position[2];

    camera_tick(sbox, camera);
}

static void tick_step_sounds(sbox_t* sbox, player_t* player) {
    vec3 velocity;
    glm_vec3_copy(player->velocity, velocity);
    velocity[1] = 0.0f;
    float xz_speed = glm_vec3_dot(velocity, velocity);

    bool play_sound = player->is_grounded &&
        sbox->time - player->last_step_time > 0.5f &&
        xz_speed > P_STOPSPEED;
    
    if (play_sound) {
        player->last_step_time = sbox->time;
        a_play(sbox, &sbox->audio, sbox->audio.step_metal_sound, random(0.85f, 1.15f));
    }
}

void player_tick(sbox_t* sbox, player_t* player, camera_t* camera, entlist_t* entlist) {
    tick_input(sbox, player, camera);
    move_and_collide(sbox, player, entlist);

    player->target_speed = glm_vec3_norm(player->move_input);
    player->target_speed *= get_max_speed(sbox, player);

    if (player->is_grounded)
        move_ground(sbox, player);
    else
        move_air(sbox, player);

    tick_camera(sbox, player, camera);
    tick_step_sounds(sbox, player);
}

void player_get_top_position(sbox_t* sbox, player_t* player, vec3 position) {
    if (!position) return;
    position[0] = player->position[0];
    position[1] = player->position[1] + get_height(sbox, player) / 2.0f;
    position[2] = player->position[2];
}

void player_get_bottom_position(sbox_t* sbox, player_t* player, vec3 position) {
    if (!position) return;
    position[0] = player->position[0];
    position[1] = player->position[1] - get_height(sbox, player) / 2.0f;
    position[2] = player->position[2];
}
