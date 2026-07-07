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

void bbox_get_center(bbox_t* bbox, vec3 center) {
    if (!center) return;

    vec3 result = {
        bbox->max[0] - bbox->min[0],
        bbox->max[1] - bbox->min[1],
        bbox->max[2] - bbox->min[2]};
    glm_vec3_copy(result, center);
}
