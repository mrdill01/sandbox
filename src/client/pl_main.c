#include "player.h"
#include "quark.h"
#include "entity.h"

#define HEIGHT 1.5f
#define HEIGHT_CROUCH 0.75f
#define RADIUS 0.4f
#define JUMPFORCE 3.4f
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
#define FRICTION_FLIGHT 3.0f
#define AIR_CONTROL 0.0f
#define INTERP_HEIGHT_SPEED 5.5f
#define MAX_INTERACT_DISTANCE 8.0f

static float get_max_speed(quark_t* quark, player_t* player);
static float get_height(quark_t* quark, player_t* player);

static void update_bbox(quark_t* quark, player_t* player) {
    float height = get_height(quark, player);
    player->bbox = bbox_new((vec3){-RADIUS, -height / 2.0f, -RADIUS},
        (vec3){RADIUS, height / 2.0f, RADIUS});
}

player_t* player_new(quark_t* quark, int id, bool is_bot) {
    player_t* player = malloc(sizeof(player_t));
    player->id = id;
    player->is_me = false;
    player->is_bot = is_bot;
    player->name = malloc(NET_MAX_PLAYER_NAME);
    strcpy(player->name, cl_name.string);
    player->move_mode = MOVE_WALK;
    update_bbox(quark, player);
    glm_vec3_copy((vec3){0.0f, 2.0f, -4.5f}, player->position);
    glm_vec3_zero(player->velocity);
    glm_vec3_zero(player->move_input);
    glm_vec3_zero(player->target_dir);
    player->buttons = 0;
    player->target_speed = get_max_speed(quark, player);
    player->is_grounded = false;
    player->is_jumping = false;
    player->look_entity = -1;
    player->vehicle = -1;
    player->grabbed_mesh = -1;
    player->ground_mat = PHYS_MAT_NONE;
    player->fall_distance = 0.0f;
    player->water_level = 0.0f;
    player->head_in_water = false;
    player->last_step_time = 0.0f;
    player->head_blocked = false;
    player->is_thirdperson = false;
    player->height = get_height(quark, player);
    inventory_init(quark, &player->inventory);
    player->walk_timer = 0.0f;
    glm_vec3_zero(player->item_position);
    glm_vec3_zero(player->item_anim);
    glm_vec3_zero(player->item_anim_angles);
    player->health = 100.0f;
    player->death_time = 0.0f;
    player_init_body(quark, player);
    edit_init(quark, &player->editor);
    if (is_bot)
        bot_init(quark, player);
    return player;
}

void player_free(quark_t* quark, player_t* player) {
    inventory_free(quark, &player->inventory);
    free(player->name);
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

static float get_max_speed(quark_t* quark, player_t* player) {
    float speed;
    switch (player->move_mode) {
    case MOVE_WALK: speed = MAX_SPEED_WALK; break;
    case MOVE_CROUCH: speed = MAX_SPEED_CROUCH; break;
    case MOVE_SPRINT: speed = MAX_SPEED_SPRINT; break;
    case MOVE_FLIGHT: speed = MAX_SPEED_FLIGHT; break;
    default: unreachable(quark);
    }

    if (player->buttons & PLAYER_BUTTON_AIM)
        speed *= 0.65f;
    
    return speed;
}

static float get_height(quark_t* quark, player_t* player) {
    if (player_is_dead(player))
        return 0.3f;
    
    switch (player->move_mode) {
    case MOVE_WALK:
    case MOVE_SPRINT:
    case MOVE_FLIGHT: return HEIGHT;
    case MOVE_CROUCH: return HEIGHT_CROUCH;
    default: unreachable(quark);
    }

    return 0.0f;
}

static void apply_friction(quark_t* quark, player_t* player, float friction) {
    float speed = sqrt(player->velocity[0] * player->velocity[0] +
        player->velocity[1] * player->velocity[1] +
        player->velocity[2] * player->velocity[2]);

    float control = (speed < STOPSPEED) ? STOPSPEED : speed;
    float drop = control * friction * quark->dt;

    float new_speed = speed - drop;
    if (new_speed < 0.0f)
        new_speed = 0.0f;
    
    if (speed > 0.0f)
        new_speed /= speed;

    player->velocity[0] *= new_speed;
    player->velocity[2] *= new_speed;
}

static void accelerate(quark_t* quark, player_t* player, float target_speed, float accel) {
    float current_speed = glm_vec3_dot(player->velocity, player->target_dir);
    float add_speed = target_speed - current_speed;
    if (add_speed <= 0)
        return;

    float accel_speed = accel * target_speed * quark->dt;
    if (accel_speed > add_speed)
        accel_speed = add_speed;

    player->velocity[0] += accel_speed * player->target_dir[0];
    if (player->move_mode == MOVE_FLIGHT)
        player->velocity[1] += accel_speed * player->target_dir[1];
    player->velocity[2] += accel_speed * player->target_dir[2];
}

static void move_flight(quark_t* quark, player_t* player) {
    float accel;
    if (glm_vec3_dot(player->velocity, player->target_dir) < 0.0f)
        accel = AIR_STOPSPEED;
    else
        accel = AIR_ACCEL;
    
    apply_friction(quark, player, FRICTION_FLIGHT);
    accelerate(quark, player, player->target_speed, accel);
}

static void move_water(quark_t* quark, player_t* player) {
    if (player->is_grounded)
        player->velocity[1] = 0.0f;
    
    apply_friction(quark, player, FRICTION_WATER);
    accelerate(quark, player, player->target_speed, ACCEL);

    for (int i = 0; i < 3; i++)
        player->velocity[i] *= 0.7f;

    if ((player->buttons & PLAYER_BUTTON_JUMP) && player->is_grounded && !player->head_blocked) {
        player->velocity[1] = JUMPFORCE;
        player->is_jumping = true;
    }
}

static void move_ground(quark_t* quark, player_t* player) {
    player->velocity[1] = 0.0f;
    apply_friction(quark, player, FRICTION);
    accelerate(quark, player, player->target_speed, ACCEL);

    if ((player->buttons & PLAYER_BUTTON_JUMP) && !player->head_blocked) {
        player->velocity[1] = JUMPFORCE;
        player->is_jumping = true;
    }
}

static void air_control(quark_t* quark, player_t* player, float target_speed) {
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
    k *= AIR_CONTROL * dot * dot * quark->dt;

    if (dot != 0.0f) {
        for (int i = 0; i < 3; i++)
            player->velocity[i] *= speed + player->target_dir[i] * k;
        glm_vec3_normalize(player->velocity);
    }

    player->velocity[0] *= speed;
    player->velocity[1] = y_speed;
    player->velocity[2] *= speed;
}

static void move_air(quark_t* quark, player_t* player) {
    float accel;
    if (glm_vec3_dot(player->velocity, player->target_dir) < 0.0f)
        accel = AIR_STOPSPEED;
    else
        accel = AIR_ACCEL;
    
    accelerate(quark, player, player->target_speed, accel);

    if (AIR_CONTROL > 0.0f)
        air_control(quark, player, player->target_speed);

    player->velocity[1] -= PHYS_GRAVITY * quark->dt;
}

static void hit_ground(quark_t* quark, player_t* player, trace_result_t trace) {
    sound_t* sound = quark->audio.jump_land_sounds[player->ground_mat];
    vec3 position;
    player_get_bottom_position(quark, player, position);
    a_play(quark, &quark->audio, sound, position, random(0.85f, 1.15f));

    sound = quark->audio.jump_land_base_sound;
    a_play(quark, &quark->audio, sound, position, random(0.85f, 1.15f));

    if (player->water_level == 0.0f) {
        vec3 position;
        player_get_bottom_position(quark, player, position);
        r_add_partfx_hit_ground(quark, &quark->renderer, position, trace.material);
    }

    player->last_step_time = quark->time;
    player->fall_distance = 0.0f;
    player->is_jumping = false;
    player->height -= 0.5f;
    player->item_anim[1] = -0.04f;

    if (player->velocity[1] < -8.0f && player->water_level == 0.0f) {
        float fall_damage = powf(-player->velocity[1], 1.25f);
        player_add_damage(quark, player, fall_damage);

        a_play(quark, &quark->audio, quark->audio.fall_damage_sound,
            player->position, random(0.85f, 1.15f));
    }

    player->velocity[1] = 0.0f;
}

static void leave_ground(quark_t* quark, player_t* player) {
    if (player->buttons & PLAYER_BUTTON_JUMP) {
        vec3 position;
        player_get_bottom_position(quark, player, position);
        a_play(quark, &quark->audio, quark->audio.jump_sound, position, random(0.85f, 1.15f));
    }
}

static void enter_water(quark_t* quark, player_t* player) {
    vec3 position;
    player_get_bottom_position(quark, player, position);
    position[1] += get_height(quark, player) / 2.0f * player->water_level;
    a_play(quark, &quark->audio, quark->audio.enter_water_sound, position, random(0.9f, 1.1f));
    r_add_partfx_enter_water(quark, &quark->renderer, position, player->velocity);
}

static void exit_water(quark_t* quark, player_t* player) {
    vec3 position;
    player_get_bottom_position(quark, player, position);
    a_play(quark, &quark->audio, quark->audio.exit_water_sound, position, random(0.9f, 1.1f));
}

static void trace_ground(quark_t* quark, player_t* player, entlist_t* entlist, bool was_grounded) {
    if (player->move_mode == MOVE_FLIGHT) return;

    vec3 start;
    player_get_top_position(quark, player, start);
    vec3 dir = {0.0f, -1.0f, 0.0f};
    float max_distance = get_height(quark, player);
    trace_result_t trace;
    
    bool was_in_water = player->water_level > 0.0f;

    if (phys_line_trace(quark, start, dir, max_distance, entlist, player->id, NULL, 0, &trace)) {
        player->position[1] = trace.point[1] + get_height(quark, player) / 2.0f;
        player->ground_mat = trace.phys_mat;
        player->water_level = trace.water_level;

        player->is_grounded = true;
        if (!was_grounded)
            hit_ground(quark, player, trace);

        if (player->water_level > 0.0f && !was_in_water)
            enter_water(quark, player);
        
    } else {
        player->is_grounded = false;
        if (was_grounded)
            leave_ground(quark, player);

        player->water_level = 0.0f;
        if (was_in_water)
            exit_water(quark, player);
    }
}

static void trace_downforce(quark_t* quark, player_t* player, entlist_t* entlist, bool was_grounded) {
    if (player->is_jumping ||
        player->fall_distance > MAX_STEDOWN ||
        player->move_mode == MOVE_FLIGHT)
        return;
    
    vec3 start;
    player_get_bottom_position(quark, player, start);
    vec3 dir = {0.0f, -1.0f, 0.0f};
    float max_distance = MAX_STEDOWN;
    trace_result_t trace;
    
    if (phys_line_trace(quark, start, dir, max_distance, entlist, player->id, NULL, 0, &trace))
        player->position[1] = trace.point[1] + get_height(quark, player) / 2.0f;
}

static void trace_head(quark_t* quark, player_t* player, entlist_t* entlist) {
    if (player->move_mode == MOVE_FLIGHT) return;

    vec3 start;
    player_get_top_position(quark, player, start);
    vec3 dir = {0.0f, 1.0f, 0.0f};
    float max_distance = player->velocity[1] * quark->dt;
    trace_result_t trace;
    
    player->head_blocked = false;
    if (phys_line_trace(quark, start, dir, max_distance, entlist, player->id, NULL, 0, &trace)) {
        player->head_blocked = true;
        player->velocity[1] = 0.0f;
    }
}

static void trace_walls(quark_t* quark, player_t* player, entlist_t* entlist) {
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
    
    float max_distance = speed * quark->dt;
    trace_result_t trace;

    if (phys_line_trace(quark, start, dir, max_distance, entlist, player->id, NULL, 0, &trace)) {
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

static void move_and_collide(quark_t* quark, player_t* player, entlist_t* entlist) {
    for (int i = 0; i < 3; i++)
        player->position[i] += player->velocity[i] * quark->dt;
    
    if (!player->is_grounded && player->velocity[1] < 0.0f)
        player->fall_distance += player->velocity[1] * quark->dt;

    bool was_grounded = player->is_grounded;
    trace_ground(quark, player, entlist, was_grounded);
    trace_downforce(quark, player, entlist, was_grounded);
    trace_head(quark, player, entlist);
    trace_walls(quark, player, entlist);
}

static void tick_camera(quark_t* quark, player_t* player, camera_t* camera) {
    if (!player->is_me) return;

    camera_tick(quark, camera);

    if (player->buttons & PLAYER_BUTTON_AIM)
        camera->fov = interp_to(camera->fov, r_fov.value * 0.75f, 12.0f, quark->dt);
    else
        camera->fov = interp_to(camera->fov, r_fov.value, 12.0f, quark->dt);

    float target_height = get_height(quark, player) / 2.0f;

    if (player_get_xz_speed(quark, player) > 0.0f && player->is_grounded && player->vehicle == -1) {
        float speed = 1.0f;
        if (player->move_mode == MOVE_SPRINT)
            speed *= 2.0f;
        target_height += sin(player->walk_timer * speed * 5.0f) * 0.15f;
    }

    player->height = interp_to(player->height, target_height, INTERP_HEIGHT_SPEED, quark->dt);

    camera->position[0] = player->position[0];
    camera->position[1] = player->position[1] + player->height - 0.1f;
    camera->position[2] = player->position[2];

    if (player->move_input[0]) {
        float roll = -player->move_input[0] * 3.0f;
        camera->angles[2] = interp_to(camera->angles[2], roll, 4.0f, quark->dt);
    } else {
        camera->angles[2] = interp_to(camera->angles[2], 0.0f, 8.0f, quark->dt);
    }

    camera->shake[0] = interp_to(camera->shake[0], 0.0f, 4.0f, quark->dt);
    camera->shake[1] = interp_to(camera->shake[1], 0.0f, 4.0f, quark->dt);
    camera->shake[2] = interp_to(camera->shake[2], 0.0f, 4.0f, quark->dt);

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

        if (player->vehicle != -1) {
            if (phys_line_trace(quark,
                position,
                dir,
                camera_distance,
                &quark->map.entlist,
                player->id,
                (int[]){player->vehicle},
                1,
                &trace))
                camera_distance = trace.distance - 0.1f;
        } else {
            if (phys_line_trace(quark,
                position,
                dir,
                camera_distance,
                &quark->map.entlist,
                player->id,
                NULL,
                0,
                &trace))
                camera_distance = trace.distance - 0.1f;
        }

        vec3 offset;
        glm_vec3_copy(camera->forward, offset);
        glm_vec3_scale(offset, -camera_distance, offset);
        glm_vec3_add(camera->position, offset, camera->position);
    }
}

static void trace_look_ray(quark_t* quark, player_t* player, camera_t* camera, entlist_t* entlist) {
    vec3 start;
    glm_vec3_copy(camera->position, start);
    
    vec3 dir;
    glm_vec3_copy(camera->forward, dir);
    float max_distance = 50.0f;

    player->look_entity = -1;
    player->head_in_water = false;

    if (phys_line_trace(quark, start, dir, max_distance, entlist,
        player->id, NULL, 0, &player->look_trace))
    {
        player->editor.trace = player->look_trace;

        if (player->look_trace.entity->type == ENTITY_MESH ||
            player->look_trace.entity->type == ENTITY_VEHICLE)
            player->look_entity = player->look_trace.entity->id;

        if (player->buttons & PLAYER_BUTTON_FIRE) {
            if (edit_mode.value) {
                player->buttons &= ~PLAYER_BUTTON_FIRE;
                 if (player->editor.selection) {
                    edit_deselect(quark, &player->editor);
                } else {
                    edit_select(quark, &player->editor, player->look_trace.entity);
                }
                return;
            }
        }
    }

    if (player->buttons & PLAYER_BUTTON_INTERACT) {
        if (player->look_entity != -1) {
            player->buttons &= ~PLAYER_BUTTON_INTERACT;

            if (player->vehicle != -1) {
                player->vehicle = -1;
                player->buttons &= ~PLAYER_BUTTON_INTERACT;
            } else if (player->grabbed_mesh != -1) {
                player->grabbed_mesh = -1;
                player->buttons &= ~PLAYER_BUTTON_INTERACT;
            } else {
                switch (player->look_trace.entity->type) {
                case ENTITY_MESH: {
                    if (player->look_trace.entity->data.mesh.enable_physics)
                        player->grabbed_mesh = player->look_entity;
                    break;
                }
                case ENTITY_VEHICLE: {
                    player->vehicle = player->look_entity;
                    break;
                }
                default: unreachable(quark);
                }
            }
        }
    }

    if (player->look_trace.start_in_water) {
        player->head_in_water = true;
    }
}

static void tick_step_sounds(quark_t* quark, player_t* player) {
    vec3 velocity;
    glm_vec3_copy(player->velocity, velocity);
    velocity[1] = 0.0f;
    float xz_speed = glm_vec3_dot(velocity, velocity);

    bool play_sound = player->is_grounded &&
        quark->time - player->last_step_time >= player_get_step_rate(quark, player) &&
        xz_speed > 1.0f;
    
    if (play_sound) {
        vec3 position;
        player_get_bottom_position(quark, player, position);
        
        sound_t* sound = quark->audio.step_sounds[player->ground_mat];
        a_play(quark, &quark->audio, sound, position, random(0.85f, 1.15f));

        if (player->water_level > 0.0f) {
            vec3 water_surface;
            player_get_bottom_position(quark, player, water_surface);
            water_surface[1] += 0.75f * get_height(quark, player);

            sound = quark->audio.step_sounds[PHYS_MAT_WATER];
            a_play(quark, &quark->audio, sound, water_surface, random(0.85f, 1.15f));

            r_add_partfx_step_water(quark, &quark->renderer, water_surface, player->velocity);
        }

        player->last_step_time = quark->time;
    }
}

static void tick_grabbed_mesh(quark_t* quark, player_t* player) {
    if (player->grabbed_mesh == -1) return;
    entity_t* entity = quark->map.entlist.ents[player->grabbed_mesh];

    float distance = glm_vec3_distance(quark->player->position, entity->position);
    float radius = bbox_get_enclosing_sphere(&entity->local_bbox);
    if (distance > radius) {
        vec3 direction;
        glm_vec3_sub(quark->player->position, entity->position, direction);
        glm_normalize(direction);

        vec3 velocity;
        glm_vec3_copy(direction, velocity);
        glm_vec3_scale(velocity, 1.0f * quark->dt, velocity);

        glm_vec3_add(entity->velocity, velocity, entity->velocity);
    }
}

void player_tick(quark_t* quark, player_t* player, camera_t* camera, entlist_t* entlist) {
    prof_start(quark, &quark->prof);

    if (player->buttons & PLAYER_BUTTON_CROUCH) set_move_mode(player, MOVE_CROUCH);
    else if (player->buttons & PLAYER_BUTTON_SPRINT) set_move_mode(player, MOVE_SPRINT);
    else set_move_mode(player, MOVE_WALK);

    update_bbox(quark, player);

    vec3 forward;
    glm_vec3_copy(camera->forward, forward);
    glm_vec3_scale(forward, player->move_input[2], forward);

    vec3 right;
    glm_vec3_copy(camera->right, right);
    glm_vec3_scale(right, player->move_input[0], right);

    vec3 up;
    glm_vec3_copy(camera->up, up);
    glm_vec3_scale(up, player->move_input[1], up);

    glm_vec3_add(player->target_dir, forward, player->target_dir);
    glm_vec3_add(player->target_dir, right, player->target_dir);
    glm_vec3_add(player->target_dir, up, player->target_dir);

    player->target_speed = 0.0f;
    if (glm_vec3_dot(player->target_dir, player->target_dir) != 0.0f) {
        glm_vec3_normalize(player->target_dir);
        float dot = glm_vec3_dot(player->target_dir, player->target_dir);
        player->target_speed = dot * get_max_speed(quark, player);
        player->walk_timer += quark->dt;
    } else {
        player->walk_timer = 0.0f;
    }

    move_and_collide(quark, player, entlist);

    if (player->vehicle != -1) {
        entity_t* vehicle = quark->map.entlist.ents[player->vehicle];
        glm_vec3_copy(vehicle->position, player->position);

    } else if (noclip.value) {
        set_move_mode(player, MOVE_FLIGHT);
        move_flight(quark, player);
    } else if (player->water_level > 0.0f)
        move_water(quark, player);
    else if (player->is_grounded)
        move_ground(quark, player);
    else
        move_air(quark, player);
    
    tick_camera(quark, player, camera);
    trace_look_ray(quark, player, camera, entlist);
    player_tick_item(quark, player);
    player_tick_body(quark, player);
    tick_step_sounds(quark, player);
    tick_grabbed_mesh(quark, player);
    edit_tick(quark, &player->editor, player);

    if (player->is_bot)
        bot_tick(quark, player);

    prof_end(quark, &quark->prof);
}

void player_render(quark_t* quark, player_t* player, renderer_t* renderer) {
    if (player->look_entity != -1 && player->vehicle == -1 && player->grabbed_mesh == -1) {
        entity_t* vehicle = quark->map.entlist.ents[player->look_entity];
        r_add_line_box(quark, &quark->renderer,
            &vehicle->world_bbox, COLOR_LIGHT_BLUE, 0.0f);
    }
    
    player_render_body(quark, player, renderer);
    player_render_item(quark, player, renderer);
}

void player_add_damage(quark_t* quark, player_t* player, float damage) {
    if (player_is_dead(player) || damage < 0.0f) return;

    player->health -= damage;
    player->health = clamp(player->health, 0.0f, 100.0f);

    a_play(quark, &quark->audio, quark->audio.hurt_sound, player->position, random(0.95f, 1.05f    ));

    if (player_is_dead(player)) {
        player->death_time = quark->time;
        
        if (player->is_me) {
            player->is_thirdperson = true;
            quark->ui_state = UI_STATE_DEAD;
        }
    }
}

bool player_is_dead(player_t* player) {
    return player->health == 0.0f;
}

void player_respawn(quark_t* quark, player_t* player) {
    glm_vec3_zero(player->velocity);
    glm_vec3_copy((vec3){0.0f, 90.0f, 0.0f}, quark->renderer.camera.angles);
    player->is_thirdperson = r_third_person.value;
    player->health = 100.0f;
    quark->ui_state = UI_STATE_IN_GAME;
    player_teleport(quark, player, (vec3){0.0f, 2.0f, -4.5f});
    
    inventory_free(quark, &player->inventory);
    inventory_init(quark, &player->inventory);
}

void player_teleport(quark_t* quark, player_t* player, vec3 destination) {
    glm_vec3_copy(destination, player->position);
}

void player_get_top_position(quark_t* quark, player_t* player, vec3 position) {
    if (!position) return;
    position[0] = player->position[0];
    position[1] = player->position[1] + get_height(quark, player) / 2.0f;
    position[2] = player->position[2];
}

void player_get_bottom_position(quark_t* quark, player_t* player, vec3 position) {
    if (!position) return;
    position[0] = player->position[0];
    position[1] = player->position[1] - get_height(quark, player) / 2.0f;
    position[2] = player->position[2];
}

float player_get_speed(quark_t* quark, player_t* player) {
    return glm_vec3_dot(player->velocity, player->velocity);
}

float player_get_step_rate(quark_t* quark, player_t* player) {
    float speed;
    switch (player->move_mode) {
    case MOVE_WALK: speed = 0.55f; break;
    case MOVE_CROUCH: speed = 0.8f; break;
    case MOVE_SPRINT: speed = 0.3f; break;
    case MOVE_FLIGHT: speed = 0.0f; break;
    default: unreachable(quark);
    }

    if (player->buttons & PLAYER_BUTTON_AIM) {
        speed *= 1.0 / 0.65f;
    }
    
    return speed;
}

float player_get_accuracy(quark_t* quark, player_t* player) {
    float accuracy = clamp(1.0f - (player_get_speed(quark, player) / 50.0f), 0.0f, 1.0f);
    if (player->buttons & PLAYER_BUTTON_AIM)
        accuracy = max(accuracy, 0.85f);
    if (!player->is_grounded)
        accuracy = 0.0f;
    return accuracy;
}

float player_get_xz_speed(quark_t* quark, player_t* player) {
    vec3 velocity;
    glm_vec3_copy(player->velocity, velocity);
    velocity[1] = 0.0f;
    return glm_vec3_dot(velocity, velocity);
}
