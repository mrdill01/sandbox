#include "render.h"
#include "sbox.h"

#define PITCH_LIMIT 89.99

void camera_init(sbox_t* sbox, camera_t* camera) {
    glm_vec3_zero(camera->position);
    glm_quat_identity(camera->rotation);
    glm_vec3_copy((vec3){0.0f, 90.0f, 0.0f}, camera->angles);
    glm_vec3_zero(camera->shake);
    glm_vec3_copy(X_AXIS, camera->right);
    glm_vec3_copy(Y_AXIS, camera->up);
    glm_vec3_copy(Z_AXIS, camera->forward);
    camera->fov = r_fov.value;
    camera->near = 0.001f;
    camera->far = 100.0f;
}

void camera_tick(sbox_t* sbox, camera_t* camera) {
    quat yaw;
    quat roll;
    quat pitch;

    glm_quat(yaw, rad(-camera->angles[1] + camera->shake[1] + 90.0f), 0.0f, 1.0f, 0.0f);
    glm_quat(roll, rad(camera->angles[2] + camera->shake[2]), 0.0f, 0.0f, 1.0f);
    glm_quat(pitch, rad(-camera->angles[0] + camera->shake[0]), 1.0f, 0.0f, 0.0f);

    glm_quat_identity(camera->rotation);
    glm_quat_mul(camera->rotation, yaw, camera->rotation);
    glm_quat_mul(camera->rotation, roll, camera->rotation);
    glm_quat_mul(camera->rotation, pitch, camera->rotation);

    camera->forward[0] = cos(rad(camera->angles[1] + camera->shake[1])) *
        cos(rad(camera->angles[0] + camera->shake[0]));
    camera->forward[1] = sin(rad(camera->angles[0] + camera->shake[0]));
    camera->forward[2] = sin(rad(camera->angles[1] + camera->shake[1])) *
        cos(rad(camera->angles[0] + camera->shake[0]));

    glm_cross(Y_AXIS, camera->forward, camera->right);
    glm_normalize(camera->right);

    glm_cross(camera->forward, camera->right, camera->up);
}

void camera_add_pitch(camera_t* camera, float pitch) {
    camera->angles[0] += pitch;
    camera->angles[0] = clamp(camera->angles[0], -PITCH_LIMIT, PITCH_LIMIT);        
}

void camera_add_yaw(camera_t* camera, float yaw) {
    camera->angles[1] += yaw;
}

void camera_add_roll(camera_t* camera, float roll) {
    camera->angles[2] += roll;
}

void camera_get_projection_matrix(camera_t* camera, int width, int height, mat4 proj) {
    glm_perspective(rad(camera->fov), (float)width / height, camera->near, camera->far, proj);
}

void camera_get_view_matrix(camera_t* camera, mat4 view) {
    vec3 center;
    glm_vec3_add(camera->position, camera->forward, center);

    vec3 roll_axis;
    glm_vec3_sub(center, camera->position, roll_axis);

    mat4 rotation;
    glm_mat4_identity(rotation);
    glm_rotate(rotation, rad(camera->angles[2] + camera->shake[2]), roll_axis);

    vec4 up;
    glm_mat4_mulv(rotation, (vec4){0.0f, 1.0f, 0.0f, 1.0f}, up);

    glm_lookat(camera->position, center, (vec3){up[0], up[1], up[2]}, view);
}
