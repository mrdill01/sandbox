#ifndef VIDEO_H
#define VIDEO_H

#include "math.h"

#include <stdint.h>

#define MAX_MATERIALS 4

typedef struct sbox_t sbox_t;

typedef struct camera_t {
    vec3 position;
    vec3 angles;
    vec3 right;
    vec3 up;
    vec3 forward;
    float fov;
    float near;
    float far;
} camera_t;

typedef struct shader_t {
    uint32_t id;
    const char* vs_path;
    const char* fs_path;
    struct shader_t* next;
} shader_t;

typedef struct mesh_t {
    uint32_t vao;
    uint32_t vbo;
    uint32_t ebo;
    uint32_t ntris;
    bbox_t bbox;
    struct mesh_t* next;
} mesh_t;

typedef struct texture_t {
    uint32_t id;
    int width;
    int height;
    struct texture_t* next;
} texture_t;

typedef struct material_t {
    texture_t* albedo;
    texture_t* roughness;
    texture_t* normal;
    float tilex;
    float tiley;
    bool is_translucent;
    struct material_t* next;
} material_t;

typedef struct {
    uint32_t id;
    size_t ntextures;
    texture_t** textures;
    uint32_t depth_buffer;
} framebuffer_t;

typedef struct {
    const mesh_t* mesh;
    size_t nmaterials;
    const material_t* materials[MAX_MATERIALS];
    mat4 model;
} drawcall_t;

typedef struct {
    texture_t* font;
} ui_t;

typedef struct {
    camera_t camera;

    size_t ndrawcalls;
    drawcall_t* drawcalls;

    shader_t* world_shader;
    shader_t* viewmodel_shader;
    shader_t* lighting_shader;
    shader_t* ui_shader;
    mesh_t* quad_mesh;
    material_t* default_material;

    framebuffer_t* gbuffer;

    mat4 projection;
    mat4 view;

    ui_t ui;
} renderer_t;

void camera_init(camera_t* camera);
void camera_tick(sbox_t* sbox, camera_t* camera);
void camera_add_pitch(camera_t* camera, float pitch);
void camera_add_yaw(camera_t* camera, float yaw);
void camera_add_roll(camera_t* camera, float roll);
void camera_get_projection_matrix(camera_t* camera, int width, int height, mat4 proj);
void camera_get_view_matrix(camera_t* camera, mat4 view);

shader_t* shader_new(sbox_t* sbox, const char* vs, const char* vname, const char* fs, const char* fname);
shader_t* shader_load(sbox_t* sbox, const char* vpath, const char* fpath);
void shader_set_int(shader_t* shader, const char* name, int i);
void shader_set_float(shader_t* shader, const char* name, float f);
void shader_set_vec2(shader_t* shader, const char* name, vec2 v);
void shader_set_vec3(shader_t* shader, const char* name, vec3 v);
void shader_set_vec4(shader_t* shader, const char* name, vec4 v);
void shader_set_mat4(shader_t* shader, const char* name, mat4 m);
void shader_free(sbox_t* sbox, shader_t* shader);

mesh_t* mesh_new(sbox_t* sbox,
    float* vertices, size_t nvertices,
    uint32_t* indices, size_t nindices,
    bbox_t bbox);
mesh_t* mesh_load(sbox_t* sbox, const char* path);
void mesh_free(sbox_t* sbox, mesh_t* mesh);

texture_t* texture_new(sbox_t* sbox, int width, int height, uint8_t* data);
texture_t* texture_load(sbox_t* sbox, const char* path);
void texture_free(sbox_t* sbox, texture_t* texture);

material_t* material_load(sbox_t* sbox,
    const char* albedo_path,
    const char* roughness_path,
    const char* normal_path,
    float tilex,
    float tiley,
    bool is_translucent);
void material_free(sbox_t* sbox, material_t* material);

framebuffer_t* framebuffer_new(sbox_t* sbox);
void framebuffer_add_texture(sbox_t* sbox, framebuffer_t* framebuffer);
void framebuffer_add_depth_buffer(sbox_t* sbox, framebuffer_t* framebuffer);
void framebuffer_finish(sbox_t* sbox, framebuffer_t* framebuffer);
void framebuffer_free(framebuffer_t* framebuffer);

void r_init(sbox_t* sbox, renderer_t* renderer);
void r_free(sbox_t* sbox, renderer_t* renderer);
void r_add_drawcall(renderer_t* renderer, drawcall_t drawcall);
void r_set_shader(shader_t* shader);
void r_set_texture(texture_t* texture, int slot);
void r_set_material(renderer_t* renderer, shader_t* shader, material_t* material, int slot);
void r_set_framebuffer(renderer_t* renderer, framebuffer_t* framebuffer);
void r_draw_mesh(mesh_t* mesh);
void r_render(sbox_t* sbox, renderer_t* renderer);

#endif
