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

void player_init_body(sbox_t* sbox, player_t* player) {
    body_t* body = &player->body;
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

void player_tick_body(sbox_t* sbox, player_t* player) {
    body_t* body = &player->body;
    glm_quat(body->rotation, rad(-sbox->renderer.camera.angles[1] + 90.0f), 0.0f, 1.0f, 0.0f);
    glm_quat(body->parts[BODY_LEFT_UPPER_ARM].rotation, rad(-90.0f), 1.0f, 0.0f, 0.0f);

    for (int i = 0; i < NUM_BODY_PARTS; i++) {
        body_part_t* part = &body->parts[i];

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
        glm_vec3_add(position, part->offset, position);

        quat rotation;
        glm_quat_identity(rotation);
        glm_quat_mul(rotation, body->rotation, rotation);
        glm_quat_mul(rotation, part->rotation, rotation);
        if (part->parent)
            glm_quat_mul(rotation, part->parent->rotation, rotation);

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
