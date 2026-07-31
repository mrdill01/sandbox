#include "player.h"
#include "sbox.h"

static body_part_t create_body_part(
    sbox_t* sbox, const char* path, vec3 offset, body_part_t* parent)
{
    body_part_t part;
    part.mesh = mesh_load(sbox, path);
    glm_vec3_copy(offset, part.offset);
    glm_quat_identity(part.rotation);
    part.parent = parent;
    return part;
}

static void setup_rig(sbox_t* sbox, player_t* player, body_t* body) {
    body->parts[BODY_TORSO] = create_body_part(
        sbox, "res/meshes/player/torso.obj", GLM_VEC3_ZERO, NULL);
    body->parts[BODY_HEAD] = create_body_part(
        sbox, "res/meshes/player/head.obj", (vec3){0.0f, 0.6f, 0.0f}, &body->parts[BODY_TORSO]);
    body->parts[BODY_LEFT_UPPER_ARM] = create_body_part(
        sbox, "res/meshes/player/upper_leg.obj", (vec3){-0.25f, 0.25f, 0.0f}, &body->parts[BODY_TORSO]);
    body->parts[BODY_LEFT_LOWER_ARM] = create_body_part(
        sbox, "res/meshes/player/upper_leg.obj", (vec3){-0.25f, -0.05f, 0.0f},
            &body->parts[BODY_LEFT_UPPER_ARM]);
    body->parts[BODY_RIGHT_UPPER_ARM] = create_body_part(
        sbox, "res/meshes/player/upper_leg.obj", (vec3){0.25f, 0.25f, 0.0f}, &body->parts[BODY_TORSO]);
    body->parts[BODY_RIGHT_LOWER_ARM] = create_body_part(
        sbox, "res/meshes/player/upper_leg.obj", (vec3){0.25f, -0.05f, 0.0f},
            &body->parts[BODY_RIGHT_UPPER_ARM]);
    body->parts[BODY_LEFT_UPPER_LEG] = create_body_part(
        sbox, "res/meshes/player/upper_leg.obj", (vec3){-0.1f, -0.1f, 0.0f}, &body->parts[BODY_TORSO]);
    body->parts[BODY_LEFT_LOWER_LEG] = create_body_part(
        sbox, "res/meshes/player/lower_leg.obj", (vec3){-0.1f, -0.4f, 0.0f},
            &body->parts[BODY_LEFT_UPPER_LEG]);
    body->parts[BODY_RIGHT_UPPER_LEG] = create_body_part(
        sbox, "res/meshes/player/upper_leg.obj", (vec3){0.1f, -0.1f, 0.0f}, &body->parts[BODY_TORSO]);
    body->parts[BODY_RIGHT_LOWER_LEG] = create_body_part(
        sbox, "res/meshes/player/lower_leg.obj", (vec3){0.1f, -0.4f, 0.0f},
            &body->parts[BODY_RIGHT_UPPER_LEG]);
}

static void setup_poses(sbox_t* sbox, player_t* player, body_t* body) {
    for (int i = 0; i < NUM_BODY_PARTS; i++) {
        glm_quat_identity(body->default_pose.bones[i]);
        glm_quat_identity(body->current_pose.bones[i]);
        glm_quat_identity(body->walk_a.bones[i]);
        glm_quat_identity(body->walk_b.bones[i]);
    }

    glm_quat(body->walk_a.bones[BODY_LEFT_UPPER_LEG],
        rad(-30.0f), 1.0f, 0.0f, 0.0f);
    glm_quat(body->walk_a.bones[BODY_RIGHT_UPPER_LEG],
        rad(30.0f), 1.0f, 0.0f, 0.0f);
    glm_quat(body->walk_a.bones[BODY_RIGHT_UPPER_ARM],
        rad(-30.0f), 1.0f, 0.0f, 0.0f);
    glm_quat(body->walk_a.bones[BODY_LEFT_UPPER_ARM],
        rad(30.0f), 1.0f, 0.0f, 0.0f);

    glm_quat(body->walk_b.bones[BODY_LEFT_UPPER_LEG],
        rad(30.0f), 1.0f, 0.0f, 0.0f);
    glm_quat(body->walk_b.bones[BODY_RIGHT_UPPER_LEG],
        rad(-30.0f), 1.0f, 0.0f, 0.0f);
    glm_quat(body->walk_b.bones[BODY_RIGHT_UPPER_ARM],
        rad(30.0f), 1.0f, 0.0f, 0.0f);
    glm_quat(body->walk_b.bones[BODY_LEFT_UPPER_ARM],
        rad(-30.0f), 1.0f, 0.0f, 0.0f);
}

void player_init_body(sbox_t* sbox, player_t* player) {
    body_t* body = &player->body;
    setup_rig(sbox, player, body);    
    setup_poses(sbox, player, body);   
    body->walk_timer = 0.0f;
    body->walk_cycle = false;
}

static void apply_pose(sbox_t* sbox, player_t* player, pose_t* pose) {
    body_t* body = &player->body;
    for (int i = 0; i < NUM_BODY_PARTS; i++) {
        glm_quat_copy(pose->bones[i], body->current_pose.bones[i]);
    }
}

static void blend_poses(sbox_t* sbox, player_t* player, pose_t* a, pose_t* b, float t) {
    body_t* body = &player->body;
    for (int i = 0; i < NUM_BODY_PARTS; i++) {
        quat rotation;
        glm_quat_slerp(a->bones[i], b->bones[i], t, rotation);
        glm_quat_copy(rotation, body->current_pose.bones[i]);
    }
}

void player_tick_body(sbox_t* sbox, player_t* player) {
    body_t* body = &player->body;

    if (player->is_me) {
        glm_quat(body->rotation, rad(-sbox->renderer.camera.angles[1] + 90.0f), 0.0f, 1.0f, 0.0f);

        if (player_is_dead(player))
            glm_quat(body->rotation, rad(-90.0f), 1.0f, 0.0f, 0.0f);
    } else {
        glm_quat_identity(body->rotation);
    }

    vec3 velocity;
    glm_vec3_copy(player->velocity, velocity);
    velocity[1] = 0.0f;

    if (player->is_grounded && glm_vec3_dot(velocity, velocity) > 0.0f) {
        if (body->walk_cycle) {
            blend_poses(sbox, player, &body->walk_a, &body->walk_b,
                body->walk_timer / player_get_step_rate(sbox, player));
        } else {
            blend_poses(sbox, player, &body->walk_b, &body->walk_a,
                body->walk_timer / player_get_step_rate(sbox, player));
        }
        
        body->walk_timer += sbox->dt;
        if (body->walk_timer >= player_get_step_rate(sbox, player)) {
            body->walk_timer = 0.0f;
            body->walk_cycle = !body->walk_cycle;
        }
        body->idle_timer = 0.0f;

    } else {
        body->walk_timer = 0.0f;
        body->walk_cycle = false;
        body->idle_timer += sbox->dt;
        blend_poses(sbox, player, &body->current_pose, &body->default_pose,
            body->idle_timer / player_get_step_rate(sbox, player));
    }

    for (int i = 0; i < NUM_BODY_PARTS; i++) {
        body_part_t* part = &body->parts[i];
        glm_quat_copy(body->current_pose.bones[i], body->parts[i].rotation);
    }
}

void player_render_body(sbox_t* sbox, player_t* player, renderer_t* renderer) {
    if (!player->is_thirdperson && player->is_me) return;
    
    body_t* body = &player->body;
    for (int i = 0; i < NUM_BODY_PARTS; i++) {
        body_part_t* part = &body->parts[i];
        if (!part->mesh) continue;

        drawcall_t drawcall;
        drawcall.entity = malloc(strlen("body part"));
        strcpy(drawcall.entity, "body part");
        
        drawcall.mesh = part->mesh;
        drawcall.materials[0] = NULL;
        drawcall.materials[1] = NULL;
        drawcall.materials[2] = NULL;
        drawcall.materials[3] = NULL;

        vec3 position;
        glm_vec3_copy(player->position, position);

        quat rotation;
        glm_quat_identity(rotation);
        glm_quat_mul(body->rotation, rotation, rotation);
        glm_quat_mul(part->rotation, rotation, rotation);
        if (part->parent)
            glm_quat_mul(part->parent->rotation, rotation, rotation);

        vec3 target_offset;
        glm_quat_rotatev(rotation, part->offset, target_offset);
        glm_vec3_add(position, target_offset, position);

        vec3 diff;
        glm_vec3_sub(target_offset, part->offset, diff);

        vec3 rotated_offset;
        glm_quat_rotatev(rotation, diff, rotated_offset);

        glm_vec3_add(position, rotated_offset, position);

        glm_mat4_identity(drawcall.model);
        glm_translate(drawcall.model, position);
        glm_quat_rotate(drawcall.model, rotation, drawcall.model);

        drawcall.local_bbox = drawcall.mesh->bbox;
        drawcall.world_bbox = bbox_translate(&drawcall.local_bbox, position);

        glm_vec3_copy(position, drawcall.position);
        glm_vec3_copy((vec3){1.0f, 1.0f, 1.0f}, drawcall.scale);
        glm_quat_copy(rotation, *drawcall.rotation);

        drawcall.dist_to_camera = 0.0f;
        drawcall.is_translucent = false;
        r_add_drawcall(renderer, drawcall);
    }
}
