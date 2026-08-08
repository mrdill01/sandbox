#include "player.h"
#include "sbox.h"
#include "entity.h"

#define HEIGHT 1.5f
#define HEIGHT_CROUCH 0.75f
#define RADIUS 0.4f
#define JUMPFORCE 3.0f
#define MAX_STEDOWN 0.5f
#define MAX_SPEED_WALK 3.2f
#define MAX_SPEED_CROUCH 2.2f
#define MAX_SPEED_SPRINT 4.0f
#define MAX_SPEED_FLIGHT 5.0f
#define ACCEL 10.0f
#define AIR_ACCEL 1.0f
#define STOPSPEED 2.5f
#define AIR_STOPSPEED 2.0f
#define FRICTION 5.0f
#define FRICTION_WATER 2.0f
#define AIR_CONTROL 0.0f
#define INTERP_HEIGHT_SPEED 5.5f
#define MAX_INTERACT_DISTANCE 8.0f

static float get_max_speed(sbox_t* sbox, player_t* player);
static float get_height(sbox_t* sbox, player_t* player);

player_t* player_new(sbox_t* sbox, int id, bool is_bot) {
    player_t* player = malloc(sizeof(player_t));
    player->id = id;
    player->is_me = false;
    player->is_bot = is_bot;
    player->bbox = bbox_new((vec3){-RADIUS, -HEIGHT / 2.0f, -RADIUS},
        (vec3){RADIUS, HEIGHT / 2.0f, RADIUS});
    player->move_mode = MOVE_WALK;
    glm_vec3_copy((vec3){0.0f, 2.0f, -4.5f}, player->position);
    glm_vec3_zero(player->velocity);
    glm_vec3_zero(player->move_input);
    glm_vec3_zero(player->target_dir);
    player->buttons = 0;
    player->target_speed = get_max_speed(sbox, player);
    player->is_grounded = true;
    player->is_jumping = false;
    player->ground_mat = PHYS_MAT_NONE;
    player->fall_distance = 0.0f;
    player->water_level = 0.0f;
    player->head_in_water = false;
    player->last_step_time = 0.0f;
    player->head_blocked = false;
    player->is_thirdperson = false;
    player->height = get_height(sbox, player);
    inventory_init(sbox, &player->inventory);
    player->walk_timer = 0.0f;
    glm_vec3_zero(player->item_position);
    glm_vec3_zero(player->item_anim);
    glm_vec3_zero(player->item_anim_angles);
    player->health = 100.0f;
    player->death_time = 0.0f;
    player_init_body(sbox, player);
    edit_init(sbox, &player->editor);
    return player;
}

void player_free(sbox_t* sbox, player_t* player) {
    inventory_free(sbox, &player->inventory);
    free(player);
}

static void set_move_mode(player_t* player, move_mode_t new_mode) {
    if (player->move_mode == MOVE_FLIGHT)
        return;
    
    if (new_mode == MOVE_SPRINT &&
        (player->buttons & PLAYER_BUTTON_FIRE ||
            player->buttons & PLAYER_BUTTON_AIM ||
            player->move_input[2] != 1.0f))
    {
        player->move_mode = MOVE_WALK;
        return;
    }
    
    if (player->move_mode == MOVE_CROUCH && new_mode != MOVE_CROUCH && player->head_blocked)
        return;

    player->move_mode = new_mode;
}

static float get_max_speed(sbox_t* sbox, player_t* player) {
    float speed;
    switch (player->move_mode) {
    case MOVE_WALK: speed = MAX_SPEED_WALK; break;
    case MOVE_CROUCH: speed = MAX_SPEED_CROUCH; break;
    case MOVE_SPRINT: speed = MAX_SPEED_SPRINT; break;
    case MOVE_FLIGHT: speed = MAX_SPEED_FLIGHT; break;
    default: unreachable(sbox);
    }

    if (player->buttons & PLAYER_BUTTON_AIM)
        speed *= 0.65f;
    
    return speed;
}

static float get_height(sbox_t* sbox, player_t* player) {
    switch (player->move_mode) {
    case MOVE_WALK:
    case MOVE_SPRINT:
    case MOVE_FLIGHT: return HEIGHT;
    case MOVE_CROUCH: return HEIGHT_CROUCH;
    default: unreachable(sbox);
    }

    return 0.0f;
}

static void apply_friction(sbox_t* sbox, player_t* player, float friction) {
    float speed = sqrt(player->velocity[0] * player->velocity[0] +
        player->velocity[1] * player->velocity[1] +
        player->velocity[2] * player->velocity[2]);

    float control = (speed < STOPSPEED) ? STOPSPEED : speed;
    float drop = control * friction * sbox->dt;

    float new_speed = speed - drop;
    if (new_speed < 0.0f)
        new_speed = 0.0f;
    
    if (speed > 0.0f)
        new_speed /= speed;

    player->velocity[0] *= new_speed;
    player->velocity[2] *= new_speed;
}

static void accelerate(sbox_t* sbox, player_t* player, float target_speed, float accel) {
    float current_speed = glm_vec3_dot(player->velocity, player->target_dir);
    float add_speed = target_speed - current_speed;
    if (add_speed <= 0)
        return;

    float accel_speed = accel * target_speed * sbox->dt;
    if (accel_speed > add_speed)
        accel_speed = add_speed;

    player->velocity[0] += accel_speed * player->target_dir[0];
    player->velocity[2] += accel_speed * player->target_dir[2];
}

static void move_flight(sbox_t* sbox, player_t* player) {
    float accel;
    if (glm_vec3_dot(player->velocity, player->target_dir) < 0.0f)
        accel = AIR_STOPSPEED;
    else
        accel = AIR_ACCEL;
    
    accelerate(sbox, player, player->target_speed, accel);
}

static void move_water(sbox_t* sbox, player_t* player) {
    if (player->is_grounded)
        player->velocity[1] = 0.0f;
    
    apply_friction(sbox, player, FRICTION_WATER);
    accelerate(sbox, player, player->target_speed, ACCEL);

    for (int i = 0; i < 3; i++)
        player->velocity[i] *= 0.7f;

    if ((player->buttons & PLAYER_BUTTON_JUMP) && player->is_grounded && !player->head_blocked) {
        player->velocity[1] = JUMPFORCE;
        player->is_jumping = true;
    }
}

static void move_ground(sbox_t* sbox, player_t* player) {
    player->velocity[1] = 0.0f;
    apply_friction(sbox, player, FRICTION);
    accelerate(sbox, player, player->target_speed, ACCEL);

    if ((player->buttons & PLAYER_BUTTON_JUMP) && !player->head_blocked) {
        player->velocity[1] = JUMPFORCE;
        player->is_jumping = true;
    }
}

static void air_control(sbox_t* sbox, player_t* player, float target_speed) {
    if (fabs(target_speed) < 0.001f)
        return;
    
    float y_speed = player->velocity[1];
    player->velocity[1] = 0.0f;
    float speed = 0.0f;
    if (glm_vec3_dot(player->velocity, player->velocity) != 0.0f) {
        speed = glm_vec3_dot(player->velocity, player->velocity);
        glm_vec3_normalize(player->velocity);
    }

    float dot = glm_vec3_dot(player->velocity, player->target_dir);
    float k = 32;
    k *= AIR_CONTROL * dot * dot * sbox->dt;

    if (dot != 0.0f) {
        for (int i = 0; i < 3; i++)
            player->velocity[i] *= speed + player->target_dir[i] * k;
        glm_vec3_normalize(player->velocity);
    }

    player->velocity[0] *= speed;
    player->velocity[1] = y_speed;
    player->velocity[2] *= speed;
}

static void move_air(sbox_t* sbox, player_t* player) {
    float accel;
    if (glm_vec3_dot(player->velocity, player->target_dir) < 0.0f)
        accel = AIR_STOPSPEED;
    else
        accel = AIR_ACCEL;
    
    accelerate(sbox, player, player->target_speed, accel);

    if (AIR_CONTROL > 0.0f)
        air_control(sbox, player, player->target_speed);

    player->velocity[1] -= PHYS_GRAVITY * sbox->dt;
}

static void hit_ground(sbox_t* sbox, player_t* player, trace_result_t trace) {
    sound_t* sound = sbox->audio.jump_land_sounds[player->ground_mat];
    vec3 position;
    player_get_bottom_position(sbox, player, position);
    a_play(sbox, &sbox->audio, sound, position, random(0.85f, 1.15f));

    sound = sbox->audio.jump_land_base_sound;
    a_play(sbox, &sbox->audio, sound, position, random(0.85f, 1.15f));

    if (player->water_level == 0.0f) {
        vec3 position;
        player_get_bottom_position(sbox, player, position);
        r_add_partfx_hit_ground(sbox, &sbox->renderer, position, trace.material);
    }

    player->last_step_time = sbox->time;
    player->fall_distance = 0.0f;
    player->is_jumping = false;
    player->height -= 0.3f;
    player->item_anim[1] = -0.035f;

    if (player->velocity[1] < -8.0f && player->water_level == 0.0f) {
        float fall_damage = powf(-player->velocity[1], 1.4f);
        fall_damage = 0.0f; /* TODO: temporary */
        player_add_damage(sbox, player, fall_damage);
    }
}

static void leave_ground(sbox_t* sbox, player_t* player) {
    if (player->buttons & PLAYER_BUTTON_JUMP) {
        vec3 position;
        player_get_bottom_position(sbox, player, position);
        a_play(sbox, &sbox->audio, sbox->audio.jump_sound, position, random(0.85f, 1.15f));
    }
}

static void enter_water(sbox_t* sbox, player_t* player) {
    vec3 position;
    player_get_bottom_position(sbox, player, position);
    position[1] += get_height(sbox, player) / 2.0f * player->water_level;
    a_play(sbox, &sbox->audio, sbox->audio.enter_water_sound, position, random(0.9f, 1.1f));
    r_add_partfx_enter_water(sbox, &sbox->renderer, position, player->velocity);
}

static void exit_water(sbox_t* sbox, player_t* player) {
    vec3 position;
    player_get_bottom_position(sbox, player, position);
    a_play(sbox, &sbox->audio, sbox->audio.exit_water_sound, position, random(0.9f, 1.1f));
}

static void trace_ground(sbox_t* sbox, player_t* player, entlist_t* entlist, bool was_grounded) {
    if (player->move_mode == MOVE_FLIGHT) return;

    vec3 start;
    player_get_top_position(sbox, player, start);
    vec3 dir = {0.0f, -1.0f, 0.0f};
    float max_distance = get_height(sbox, player);
    trace_result_t trace;
    
    bool was_in_water = player->water_level > 0.0f;

    if (phys_line_trace(sbox, start, dir, max_distance, entlist, player->id, &trace)) {
        player->position[1] = trace.point[1] + get_height(sbox, player) / 2.0f;
        player->ground_mat = trace.phys_mat;
        player->water_level = trace.water_level;

        player->is_grounded = true;
        if (!was_grounded)
            hit_ground(sbox, player, trace);

        if (player->water_level > 0.0f && !was_in_water)
            enter_water(sbox, player);
        
    } else {
        player->is_grounded = false;
        if (was_grounded)
            leave_ground(sbox, player);

        player->water_level = 0.0f;
        if (was_in_water)
            exit_water(sbox, player);
    }
}

static void trace_downforce(sbox_t* sbox, player_t* player, entlist_t* entlist, bool was_grounded) {
    if (player->is_jumping ||
        player->fall_distance >= MAX_STEDOWN ||
        player->move_mode == MOVE_FLIGHT)
        return;
    
    vec3 start;
    player_get_bottom_position(sbox, player, start);
    vec3 dir = {0.0f, -1.0f, 0.0f};
    float max_distance = MAX_STEDOWN;
    trace_result_t trace;
    
    if (phys_line_trace(sbox, start, dir, max_distance, entlist, player->id, &trace))
        player->position[1] = trace.point[1] + get_height(sbox, player) / 2.0f;
}

static void trace_head(sbox_t* sbox, player_t* player, entlist_t* entlist) {
    vec3 start;
    player_get_top_position(sbox, player, start);
    vec3 dir = {0.0f, 1.0f, 0.0f};
    float max_distance = player->velocity[1] * sbox->dt;
    trace_result_t trace;
    
    player->head_blocked = false;
    if (phys_line_trace(sbox, start, dir, max_distance, entlist, player->id, &trace)) {
        player->head_blocked = true;
        player->velocity[1] = 0.0f;
    }
}

static void trace_walls(sbox_t* sbox, player_t* player, entlist_t* entlist) {
    return;

    vec3 start;
    glm_vec3_copy(player->position, start);
    
    vec3 dir;
    glm_vec3_zero(dir);
    float speed = glm_vec3_dot(player->velocity, player->velocity);
    if (speed != 0.0f) {
        glm_vec3_copy(player->velocity, dir);
        glm_vec3_norm(dir);
    }
    
    float max_distance = speed * sbox->dt;
    trace_result_t trace;

    if (phys_line_trace(sbox, start, dir, max_distance, entlist, player->id, &trace)) {
        float vel_dot_normal = glm_vec3_dot(player->velocity, trace.normal);

        vec3 slide;
        glm_vec3_scale(trace.normal, vel_dot_normal, slide);
        glm_vec3_sub(player->velocity, slide, slide);

        glm_vec3_copy(slide, player->velocity);

        player->position[0] = trace.point[0] - dir[0] * 0.1f;
        player->position[1] = trace.point[1] - dir[1] * 0.1f;
        player->position[2] = trace.point[2] - dir[2] * 0.1f;
    }
}

static void move_and_collide(sbox_t* sbox, player_t* player, entlist_t* entlist) {
    for (int i = 0; i < 3; i++)
        player->position[i] += player->velocity[i] * sbox->dt;
    
    if (!player->is_grounded && player->velocity[1] < 0.0f)
        player->fall_distance += player->velocity[1] * sbox->dt;

    bool was_grounded = player->is_grounded;
    trace_ground(sbox, player, entlist, was_grounded);
    trace_downforce(sbox, player, entlist, was_grounded);
    trace_head(sbox, player, entlist);
    trace_walls(sbox, player, entlist);
}

static void tick_camera(sbox_t* sbox, player_t* player, camera_t* camera) {
    if (!player->is_me) return;

    camera_tick(sbox, camera);

    if (player->buttons & PLAYER_BUTTON_AIM)
        camera->fov = interp_to(camera->fov, r_fov.value * 0.75f, 12.0f, sbox->dt);
    else
        camera->fov = interp_to(camera->fov, r_fov.value, 12.0f, sbox->dt);

    player->height = interp_to(player->height,
        get_height(sbox, player) / 2.0f, INTERP_HEIGHT_SPEED, sbox->dt);

    camera->position[0] = player->position[0];
    camera->position[1] = player->position[1] + player->height- 0.1f;
    camera->position[2] = player->position[2];

    if (player->is_thirdperson) {
        vec3 y_offset;
        glm_vec3_copy(camera->up, y_offset);
        glm_vec3_scale(y_offset, 0.3f, y_offset);
        glm_vec3_add(camera->position, y_offset, camera->position);

        vec3 dir;
        glm_vec3_copy(camera->forward, dir);
        glm_vec3_inv(dir);

        vec3 position;
        glm_vec3_copy(camera->position, position);

        float camera_distance = PLAYER_THIRDPERSON_CAMERA_LENGTH;
        trace_result_t trace;

        if (phys_line_trace(sbox,
            position,
            dir,
            camera_distance,
            &sbox->map.entlist,
            player->id,
            &trace))
            camera_distance = trace.distance - 0.1f;

        vec3 offset;
        glm_vec3_copy(camera->forward, offset);
        glm_vec3_scale(offset, -camera_distance, offset);
        glm_vec3_add(camera->position, offset, camera->position);
    }
}

static void trace_look_ray(sbox_t* sbox, player_t* player, camera_t* camera, entlist_t* entlist) {
    vec3 start;
    glm_vec3_copy(camera->position, start);
    
    vec3 dir;
    glm_vec3_copy(camera->forward, dir);
    float max_distance = 50.0f;

    player->head_in_water = false;

    if (phys_line_trace(sbox, start, dir, max_distance, entlist, player->id, &player->look_trace)) {
        if (player->buttons & PLAYER_BUTTON_FIRE) {
            if (player->look_trace.entity && player->look_trace.entity->type == ENTITY_VEHICLE) {

            }

            if (edit_mode.value) {
                 if (player->editor.selection) {
                    player->editor.selection->data.mesh.enable_collision = true;
                    player->editor.selection = NULL;
                } else {
                    player->editor.selection = player->look_trace.entity;
                    player->editor.selection->data.mesh.enable_collision = false;
                }

                player->editor.trace = player->look_trace;
            }
        }
    }

    if (player->look_trace.start_in_water) {
        player->head_in_water = true;
    }
}

static void tick_step_sounds(sbox_t* sbox, player_t* player) {
    vec3 velocity;
    glm_vec3_copy(player->velocity, velocity);
    velocity[1] = 0.0f;
    float xz_speed = glm_vec3_dot(velocity, velocity);

    bool play_sound = player->is_grounded &&
        sbox->time - player->last_step_time >= player_get_step_rate(sbox, player) &&
        xz_speed > 1.0f;
    
    if (play_sound) {
        vec3 position;
        player_get_bottom_position(sbox, player, position);
        
        sound_t* sound = sbox->audio.step_sounds[player->ground_mat];
        a_play(sbox, &sbox->audio, sound, position, random(0.85f, 1.15f));

        if (player->water_level > 0.0f) {
            vec3 water_surface;
            player_get_bottom_position(sbox, player, water_surface);
            water_surface[1] += 0.75f * get_height(sbox, player);

            sound = sbox->audio.step_sounds[PHYS_MAT_WATER];
            a_play(sbox, &sbox->audio, sound, water_surface, random(0.85f, 1.15f));

            r_add_partfx_step_water(sbox, &sbox->renderer, water_surface, player->velocity);
        }

        player->last_step_time = sbox->time;
    }
}

void player_tick(sbox_t* sbox, player_t* player, camera_t* camera, entlist_t* entlist) {
    if (player->buttons & PLAYER_BUTTON_CROUCH) set_move_mode(player, MOVE_CROUCH);
    else if (player->buttons & PLAYER_BUTTON_SPRINT) set_move_mode(player, MOVE_SPRINT);
    else set_move_mode(player, MOVE_WALK);

    vec3 forward;
    glm_vec3_copy(camera->forward, forward);
    glm_vec3_scale(forward, player->move_input[2], forward);

    vec3 right;
    glm_vec3_copy(camera->right, right);
    glm_vec3_scale(right, player->move_input[0], right);

    vec3 up;
    glm_vec3_copy(camera->up, up);
    glm_vec3_scale(up, player->move_input[2], up);

    glm_vec3_add(player->target_dir, forward, player->target_dir);
    glm_vec3_add(player->target_dir, right, player->target_dir);
    glm_vec3_add(player->target_dir, up, player->target_dir);

    player->target_speed = 0.0f;
    if (glm_vec3_dot(player->target_dir, player->target_dir) != 0.0f) {
        player->target_speed = glm_vec3_norm(player->target_dir) * get_max_speed(sbox, player);
        player->walk_timer += sbox->dt;
    } else {
        player->walk_timer = 0.0f;
    }

    move_and_collide(sbox, player, entlist);

    if (noclip.value) {
        set_move_mode(player, MOVE_FLIGHT);
        move_flight(sbox, player);
    } else if (player->water_level > 0.0f)
        move_water(sbox, player);
    else if (player->is_grounded)
        move_ground(sbox, player);
    else
        move_air(sbox, player);
    
    tick_camera(sbox, player, camera);
    trace_look_ray(sbox, player, camera, entlist);
    player_tick_item(sbox, player);
    player_tick_body(sbox, player);
    tick_step_sounds(sbox, player);
    edit_tick(sbox, &player->editor, player);

    if (player->is_bot)
        bot_tick(sbox, player);
}

void player_render(sbox_t* sbox, player_t* player, renderer_t* renderer) {
    player_render_body(sbox, player, renderer);
    player_render_item(sbox, player, renderer);
}

void player_add_damage(sbox_t* sbox, player_t* player, float damage) {
    if (player_is_dead(player) || damage < 0.0f) return;

    player->health -= damage;
    player->health = clamp(player->health, 0.0f, 100.0f);

    if (player_is_dead(player)) {
        player->death_time = sbox->time;
        
        if (player->is_me) {
            player->is_thirdperson = true;
            sbox->ui_state = UI_STATE_DEAD;
        }
    }
}

bool player_is_dead(player_t* player) {
    return player->health == 0.0f;
}

void player_respawn(sbox_t* sbox, player_t* player) {
    glm_vec3_zero(player->velocity);
    glm_vec3_copy((vec3){0.0f, 90.0f, 0.0f}, sbox->renderer.camera.angles);
    player->is_thirdperson = r_third_person.value;
    player->health = 100.0f;
    sbox->ui_state = UI_STATE_IN_GAME;
    player_teleport(sbox, player, (vec3){0.0f, 2.0f, -4.5f});
}

void player_teleport(sbox_t* sbox, player_t* player, vec3 destination) {
    glm_vec3_copy(destination, player->position);
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

float player_get_speed(sbox_t* sbox, player_t* player) {
    return glm_vec3_dot(player->velocity, player->velocity);
}

float player_get_step_rate(sbox_t* sbox, player_t* player) {
    float speed;
    switch (player->move_mode) {
    case MOVE_WALK: speed = 0.55f; break;
    case MOVE_CROUCH: speed = 0.8f; break;
    case MOVE_SPRINT: speed = 0.3f; break;
    case MOVE_FLIGHT: speed = 0.0f; break;
    default: unreachable(sbox);
    }

    if (player->buttons & PLAYER_BUTTON_AIM) {
        speed *= 1.0 / 0.65f;
    }
    
    return speed;
}

float player_get_accuracy(sbox_t* sbox, player_t* player) {
    float accuracy = clamp(1.0f - (player_get_speed(sbox, player) / 50.0f), 0.0f, 1.0f);
    if (player->buttons & PLAYER_BUTTON_AIM)
        accuracy = max(accuracy, 0.85f);
    if (!player->is_grounded)
        accuracy = 0.0f;
    return accuracy;
}
