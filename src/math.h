#ifndef MATH_H
#define MATH_H

#include <math.h>
#include <stdbool.h>

#include "../include/cglm/cglm.h"
#include "../include/cglm/quat.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define X_AXIS (vec3){1.0f, 0.0f, 0.0f}
#define Y_AXIS (vec3){0.0f, 1.0f, 0.0f}
#define Z_AXIS (vec3){0.0f, 0.0f, 1.0f}

typedef versor quat;

typedef struct {
    vec3 min;
    vec3 max;
} bbox_t;

float rad(float degrees);
float deg(float radians);
float min(float a, float b);
float max(float a, float b);
float clip(float x, float a, float b);
float lerp(float a, float b, float t);
float interp_to(float current, float target, float speed, float dt);
float random(float start, float end);

bbox_t bbox_new(vec3 min, vec3 max);
void bbox_get_center(const bbox_t* bbox, vec3 center);
void bbox_get_size(const bbox_t* bbox, vec3 size);
void bbox_get_half_size(const bbox_t* bbox, vec3 half_size);
bbox_t bbox_translate(bbox_t* bbox, vec3 position);
bbox_t bbox_rotate(bbox_t* bbox, mat4 rotation);
bbox_t bbox_scale(bbox_t* bbox, vec3 scale);
bool bbox_point_intersects(const bbox_t* bbox, vec3 point);

#endif
