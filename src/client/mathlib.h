#ifndef MATH_H
#define MATH_H

#include <math.h>
#include <stdbool.h>

#include "../../include/cglm/cglm.h"
#include "../../include/cglm/quat.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define X_AXIS (vec3){1.0f, 0.0f, 0.0f}
#define Y_AXIS (vec3){0.0f, 1.0f, 0.0f}
#define Z_AXIS (vec3){0.0f, 0.0f, 1.0f}

typedef versor quat;
typedef struct mesh_t mesh_t;

typedef struct {
    vec3 min;
    vec3 max;
} bbox_t;

typedef struct {
    vec3 origin;
    vec3 dir;
} ray_t;

float sign(float);
float rad(float degrees);
float deg(float radians);
float min(float a, float b);
float max(float a, float b);
float clamp(float x, float a, float b);
float lerp(float a, float b, float t);
float interp_to(float current, float target, float speed, float dt);
float random(float start, float end);

bbox_t bbox_new(vec3 min, vec3 max);
void bbox_get_center(const bbox_t* bbox, vec3 center);
void bbox_get_size(const bbox_t* bbox, vec3 size);
void bbox_get_half_size(const bbox_t* bbox, vec3 half_size);
float bbox_get_enclosing_sphere(const bbox_t* bbox);
bbox_t bbox_translate(bbox_t* bbox, vec3 position);
bbox_t bbox_rotate(bbox_t* bbox, quat rotation);
bbox_t bbox_scale(bbox_t* bbox, vec3 scale);
bool bbox_point_intersects(const bbox_t* bbox, vec3 point);
bool bbox_sphere_intersects(const bbox_t* bbox, vec3 center, float radius);

void point_on_ray(ray_t ray, float t, vec3 end);
bool raycast_bbox(ray_t ray, const bbox_t* box, float* t, float tmax);
bool raycast_mesh(ray_t ray, vec3 position, quat rotation, mesh_t* mesh, float* t, float tmax);
bool raycast_triangle(
    ray_t ray, vec3 v0, vec3 v1, vec3 v2, float* t, float* u, float* v, float tmax);

#endif
