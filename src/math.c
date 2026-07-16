#include "math.h"

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

float clip(float x, float a, float b) {
    return min(max(x, a), b);
}

float interp_to(float current, float target, float speed, float dt) {
    return current + (target - current) * (speed * dt);
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
    if (!center) return;

    vec3 result = {
        (bbox->min[0] + bbox->min[0]) / 2,
        (bbox->min[1] + bbox->min[1]) / 2,
        (bbox->min[2] + bbox->min[2]) / 2};
    glm_vec3_copy(result, center);
}

bbox_t bbox_translate(bbox_t* bbox, vec3 position) {
    bbox_t result;
    glm_vec3_add(bbox->min, position, result.min);
    glm_vec3_add(bbox->max, position, result.max);
    return result;
}

bbox_t bbox_scale(bbox_t* bbox, vec3 scale) {
    bbox_t result;
    glm_vec3_mul(bbox->min, scale, result.min);
    glm_vec3_mul(bbox->max, scale, result.max);
    return result;
}

bool bbox_point_intersects(const bbox_t* bbox, vec3 point) {
    return point[0] > bbox->min[0] &&
        point[1] > bbox->min[1] &&
        point[2] > bbox->min[2] &&
        point[0] < bbox->max[0] &&
        point[1] < bbox->max[1] &&
        point[2] < bbox->max[2];
}
