#include "mathlib.h"
#include "../client/render.h"

float sign(float num) {
    if (num < 0.0f) return -1.0f;
    if (num > 0.0f) return 1.0f;
    return 0.0f;
}

float rad(float degrees) {
    return degrees * (M_PI / 180.0f);
}

float deg(float radians) {
    return radians * (180.0f / M_PI);
}

float min(float a, float b) {
	return (a < b) ? a : b;
}

float max(float a, float b) {
	return (a > b) ? a : b;
}

float clamp(float x, float lo, float hi) {
    return max(lo, min(x, hi));
}

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

float interp_to(float current, float target, float speed, float dt) {
    if (speed <= 0.0f)
        return target;
    
    float dist = target - current;
    if (dist * dist < 0.00001f)
        return target;

    float delta_move = clamp(speed * dt, 0.0f, 1.0f);
    return current + dist * delta_move;
}

float random(float start, float end) {
	float scale = rand() / (float)RAND_MAX;
	return start + scale * (end - start);
}

bbox_t bbox_new(vec3 min, vec3 max) {
    bbox_t bbox;
    glm_vec3_copy(min, bbox.min);
    glm_vec3_copy(max, bbox.max);
    return bbox;
}

void bbox_get_center(const bbox_t* bbox, vec3 center) {
    vec3 result = {
        (bbox->min[0] + bbox->max[0]) / 2.0f,
        (bbox->min[1] + bbox->max[1]) / 2.0f,
        (bbox->min[2] + bbox->max[2]) / 2.0f};
    glm_vec3_copy(result, center);
}

void bbox_get_size(const bbox_t* bbox, vec3 size) {
    vec3 result = {
        bbox->max[0] - bbox->min[0],
        bbox->max[1] - bbox->min[1],
        bbox->max[2] - bbox->min[2]};
    glm_vec3_copy(result, size);
}

void bbox_get_half_size(const bbox_t* bbox, vec3 half_size) {
    vec3 result = {
        (bbox->max[0] - bbox->min[0]) / 2.0f,
        (bbox->max[1] - bbox->min[1]) / 2.0f,
        (bbox->max[2] - bbox->min[2]) / 2.0f};
    glm_vec3_copy(result, half_size);
}

float bbox_get_enclosing_sphere(const bbox_t* bbox) {
    vec3 half_size;
    bbox_get_half_size(bbox, half_size);
    return sqrtf(glm_vec3_dot(half_size, half_size));
}

bbox_t bbox_translate(bbox_t* bbox, vec3 position) {
    bbox_t result;
    glm_vec3_add(bbox->min, position, result.min);
    glm_vec3_add(bbox->max, position, result.max);
    return result;
}

bbox_t bbox_rotate(bbox_t* bbox, quat rotation) {
    vec3 corners[] = {
        {bbox->min[0], bbox->min[1], bbox->min[2]},
        {bbox->max[0], bbox->min[1], bbox->min[2]},
        {bbox->max[0], bbox->max[1], bbox->min[2]},
        {bbox->min[0], bbox->max[1], bbox->min[2]},
        {bbox->min[0], bbox->min[1], bbox->max[2]},
        {bbox->max[0], bbox->min[1], bbox->max[2]},
        {bbox->max[0], bbox->max[1], bbox->max[2]},
        {bbox->min[0], bbox->max[1], bbox->max[2]},
    };

    bbox_t result = {0};

    for (int i = 0; i < 8; i++) {
        vec3 rotated;
        glm_quat_rotatev(rotation, corners[i], rotated);
        result.min[0] = min(result.min[0], rotated[0]);
        result.min[1] = min(result.min[1], rotated[1]);
        result.min[2] = min(result.min[2], rotated[2]);
        result.max[0] = max(result.max[0], rotated[0]);
        result.max[1] = max(result.max[1], rotated[1]);
        result.max[2] = max(result.max[2], rotated[2]);
    }

    return result;
}

bbox_t bbox_scale(bbox_t* bbox, vec3 scale) {
    bbox_t result;
    glm_vec3_mul(bbox->min, scale, result.min);
    glm_vec3_mul(bbox->max, scale, result.max);
    return result;
}

bool bbox_point_intersects(const bbox_t* bbox, vec3 point) {
    return point[0] >= bbox->min[0] &&
        point[1] >= bbox->min[1] &&
        point[2] >= bbox->min[2] &&
        point[0] < bbox->max[0] &&
        point[1] < bbox->max[1] &&
        point[2] < bbox->max[2];
}

bool bbox_sphere_intersects(const bbox_t* bbox, vec3 center, float radius) {
    float closest_x = max(bbox->min[0], min(center[0], bbox->max[0]));
    float closest_y = max(bbox->min[1], min(center[1], bbox->max[1]));
    float closest_z = max(bbox->min[2], min(center[2], bbox->max[2]));

    float distance_x = closest_x - center[0];
    float distance_y = closest_y - center[1];
    float distance_z = closest_z - center[2];

    float squared_distance = (distance_x * distance_x) +
        (distance_y * distance_y) +
        (distance_z * distance_z);

    return squared_distance <= radius * radius;
}

void point_on_ray(ray_t ray, float t, vec3 end) {
    vec3 scaled_dir;
    glm_vec3_copy(ray.dir, scaled_dir);
    glm_vec3_scale(scaled_dir, t, scaled_dir);
    glm_vec3_add(ray.origin, scaled_dir, end);
}

bool raycast_bbox(ray_t ray, const bbox_t* box, float* t, float tmax) {
    float tmin = -INFINITY;

    float inv_dir_x = 1.0f / ray.dir[0];
    float t1 = (box->min[0] - ray.origin[0]) * inv_dir_x;
    float t2 = (box->max[0] - ray.origin[0]) * inv_dir_x;
    if (inv_dir_x < 0.0f) { float tmp = t1; t1 = t2; t2 = tmp; }
    tmin = t1 > tmin ? t1 : tmin;
    tmax = t2 < tmax ? t2 : tmax;
    if (tmin > tmax) return false;

    float inv_dir_y = 1.0f / ray.dir[1];
    float t3 = (box->min[1] - ray.origin[1]) * inv_dir_y;
    float t4 = (box->max[1] - ray.origin[1]) * inv_dir_y;
    if (inv_dir_y < 0.0f) { float tmp = t3; t3 = t4; t4 = tmp; }
    tmin = t3 > tmin ? t3 : tmin;
    tmax = t4 < tmax ? t4 : tmax;
    if (tmin > tmax) return false;

    float inv_dir_z = 1.0f / ray.dir[2];
    float t5 = (box->min[2] - ray.origin[2]) * inv_dir_z;
    float t6 = (box->max[2] - ray.origin[2]) * inv_dir_z;
    if (inv_dir_z < 0.0f) { float tmp = t5; t5 = t6; t6 = tmp; }
    tmin = t5 > tmin ? t5 : tmin;
    tmax = t6 < tmax ? t6 : tmax;
    if (tmin > tmax) return false;

    if (tmax < 0.0f) return false;
    *t = (tmin >= 0.0f) ? tmin : tmax;
    return true;
}

bool raycast_mesh(ray_t ray, vec3 position, quat rotation, mesh_t* mesh, float* t, float tmax) {
    for (int i = 0; i < mesh->nbuffers; i++) {
        mesh_buffer_t* buffer = mesh->buffers[i];
        if (!buffer) continue;

        const int stride = 9;
        for (int i = 0; i < buffer->nvertices; i += stride * 3) {
            vec3 v0 = {
                buffer->vertices[i + 0],
                buffer->vertices[i + 1],
                buffer->vertices[i + 2]};
            vec3 v1 = {
                buffer->vertices[i + stride + 0],
                buffer->vertices[i + stride + 1],
                buffer->vertices[i + stride + 2]};
            vec3 v2 = {
                buffer->vertices[i + (stride * 2) + 0],
                buffer->vertices[i + (stride * 2) + 1],
                buffer->vertices[i + (stride * 2) + 2]};

            glm_quat_rotatev(rotation, v0, v0);
            glm_quat_rotatev(rotation, v1, v1);
            glm_quat_rotatev(rotation, v2, v2);

            glm_vec3_add(position, v0, v0);
            glm_vec3_add(position, v1, v1);
            glm_vec3_add(position, v2, v2);

            float u = 0.0f;
            float v = 0.0f;
            if (raycast_triangle(ray, v0, v1, v2, t, &u, &v, tmax))
                return true;
        }
    }

    return false;
}

bool raycast_triangle(
    ray_t ray, vec3 v0, vec3 v1, vec3 v2, float* t, float* u, float* v, float tmax)
{
    const float EPSILON = 0.01f;
    
    vec3 edge1;
    glm_vec3_sub(v1, v0, edge1);

    vec3 edge2;
    glm_vec3_sub(v2, v0, edge2);
    
    vec3 h;
    glm_vec3_cross(ray.dir, edge2, h);
    
    float a = glm_vec3_dot(edge1, h);
    if (a > -EPSILON && a < EPSILON) return false;
    
    float f = 1.0f / a;
    vec3 s;
    glm_vec3_sub(ray.origin, v0, s);
    *u = f * glm_vec3_dot(s, h);
    if (*u < 0.0f || *u > 1.0f) return false;
    
    vec3 q;
    glm_vec3_cross(s, edge1, q);
    *v = f * glm_vec3_dot(ray.dir, q);
    if (*v < 0.0f || *u + *v > 1.0f) return false;
    
    *t = f * glm_vec3_dot(edge2, q);
    if (*t < EPSILON) {
        *t = tmax;
        return true;
    }
    return *t > EPSILON && *t <= tmax;
}
