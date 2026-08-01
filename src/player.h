#ifndef PLAYER_H
#define PLAYER_H

#include "math.h"
#include "physics.h"
#include "edit.h"
#include "item.h"

typedef struct sbox_t sbox_t;
typedef struct camera_t camera_t;
typedef struct entlist_t entlist_t;

#define PLAYER_THIRDPERSON_CAMERA_LENGTH 3.0f

#define PLAYER_BUTTON_JUMP 1
#define PLAYER_BUTTON_CROUCH 2
#define PLAYER_BUTTON_SPRINT 4
#define PLAYER_BUTTON_FIRE 8
#define PLAYER_BUTTON_AIM 16

typedef enum {
    MOVE_WALK,
    MOVE_CROUCH,
    MOVE_SPRINT,
    MOVE_FLIGHT,
} move_mode_t;

typedef struct body_part_t {
    mesh_t* mesh;
    vec3 offset;
    quat rotation;
    struct body_part_t* parent;
} body_part_t;

typedef enum {
    BODY_TORSO,
    BODY_HEAD,
    BODY_LEFT_UPPER_ARM,
    BODY_LEFT_LOWER_ARM,
    BODY_RIGHT_UPPER_ARM,
    BODY_RIGHT_LOWER_ARM,
    BODY_LEFT_UPPER_LEG,
    BODY_LEFT_LOWER_LEG,
    BODY_RIGHT_UPPER_LEG,
    BODY_RIGHT_LOWER_LEG,
    NUM_BODY_PARTS,
} body_part_index_t;

typedef struct {
    quat bones[NUM_BODY_PARTS];
} pose_t;

typedef struct {
    body_part_t parts[NUM_BODY_PARTS];
    quat rotation;

    pose_t default_pose;
    pose_t current_pose;
    pose_t walk_a;
    pose_t walk_b;

    float walk_timer;
    bool walk_cycle;
    float idle_timer;
} body_t;

typedef struct player_t {
    int id;
    bool is_me;
    bool is_bot;
    bbox_t bbox;
    move_mode_t move_mode;
    vec3 position;
    vec3 velocity;
    vec3 move_input;
    vec3 target_dir;
    uint32_t buttons;
    float target_speed;
    bool is_grounded;
    bool is_jumping;
    trace_result_t look_trace;
    phys_material_t ground_mat;
    float fall_distance;
    float water_level;
    float last_step_time;
    bool head_blocked;
    bool is_thirdperson;
    float height;
    inventory_t inventory;
    vec3 item_position;
    vec3 item_anim;
    vec3 item_anim_angles;
    float health;
    float death_time;
    body_t body;
    editor_t editor;
} player_t;

player_t* player_new(sbox_t* sbox, int id, bool is_bot);
void player_free(sbox_t* sbox, player_t* player);

void player_input(sbox_t* sbox, player_t* player);

void player_tick(sbox_t* sbox, player_t* player, camera_t* camera, entlist_t* entlist);
void player_render(sbox_t* sbox, player_t* player, renderer_t* renderer);

void player_add_damage(sbox_t* sbox, player_t* player, float damage);
bool player_is_dead(player_t* player);
void player_respawn(sbox_t* sbox, player_t* player);
void player_teleport(sbox_t* sbox, player_t* player, vec3 destination);

void player_get_top_position(sbox_t* sbox, player_t* player, vec3 position);
void player_get_bottom_position(sbox_t* sbox, player_t* player, vec3 position);
float player_get_step_rate(sbox_t* sbox, player_t* player);

void player_tick_item(sbox_t* sbox, player_t* player);
void player_render_item(sbox_t* sbox, player_t* player, renderer_t* renderer);

void player_init_body(sbox_t* sbox, player_t* player);
void player_tick_body(sbox_t* sbox, player_t* player);
void player_render_body(sbox_t* sbox, player_t* player, renderer_t* renderer);

void bot_tick(sbox_t* sbox, player_t* player);

#endif
