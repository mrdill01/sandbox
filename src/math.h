#ifndef MATH_H
#define MATH_H

#include <math.h>

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
float interp_to(float current, float target, float speed, float dt);

void bbox_get_center(const bbox_t* bbox, vec3 center);

#endif
