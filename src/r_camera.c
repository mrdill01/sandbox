#include "render.h"
#include "sbox.h"

#define PITCH_LIMIT 89.99

void camera_init(sbox_t* sbox, camera_t* camera) {
    glm_vec3_copy((vec3)GLM_VEC3_ZERO_INIT, camera->position);
    vec3 angles = {0.0f, 90.0f, 0.0f};
    glm_vec3_copy(angles, camera->angles);
    glm_vec3_copy(X_AXIS, camera->right);
    glm_vec3_copy(Y_AXIS, camera->up);
    glm_vec3_copy(Z_AXIS, camera->forward);
    camera->fov = sbox->cfg.r_fov;
    camera->near = 0.001f;
    camera->far = 100.0f;
}

void camera_tick(sbox_t* sbox, camera_t* camera) {
    camera->forward[0] = cos(rad(camera->angles[1])) * cos(rad(camera->angles[0]));
    camera->forward[1] = sin(rad(camera->angles[0]));
    camera->forward[2] = sin(rad(camera->angles[1])) * cos(rad(camera->angles[0]));

    glm_cross(Y_AXIS, camera->forward, camera->right);
    glm_normalize(camera->right);

    glm_cross(camera->forward, camera->right, camera->up);

    camera->fov = sbox->cfg.r_fov;
}

void camera_add_pitch(camera_t* camera, float pitch) {
    camera->angles[0] += pitch;
    camera->angles[0] = clip(camera->angles[0], -PITCH_LIMIT, PITCH_LIMIT);        
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
    glm_lookat(camera->position, center, Y_AXIS, view);
}
