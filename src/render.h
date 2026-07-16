#ifndef VIDEO_H
#define VIDEO_H

#include "math.h"

#include <stdint.h>

#define R_GL_MAJ 3
#define R_GL_MIN 3
#define MAX_MATERIALS 4

#define COLOR_WHITE (vec4){1.0f, 1.0f, 1.0f, 1.0f}

typedef struct sbox_t sbox_t;
typedef enum phys_material_t phys_material_t;

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
    uint8_t nmaterials;
    bbox_t bbox;
    struct mesh_t* next;
} mesh_t;

typedef enum {
    TEX_2D,
    TEX_CUBE,
} texture_type_t;

typedef enum {
    TEX_FORMAT_RGB,
    TEX_FORMAT_RGBA,
    TEX_FORMAT_RGBA_F16,
    TEX_FORMAT_DEPTH,
} texture_format_t;

typedef struct texture_t {
    uint32_t id;
    texture_type_t type;
    int width;
    int height;
    texture_format_t format;
    struct texture_t* next;
} texture_t;

typedef struct material_t {
    char* name;
    texture_t* albedo;
    texture_t* roughness;
    texture_t* normal;
    float tilex;
    float tiley;
    float scrollx;
    float scrolly;
    float scroll_speed;
    bool is_translucent;
    bool is_water;
    int phys_mat;
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
    const material_t* materials[MAX_MATERIALS];
    mat4 model;
    float dist_to_camera;
    bool is_translucent;
} drawcall_t;

typedef struct {
    shader_t* shader;
    texture_t* font;
    mesh_t* quad;
    mat4 projection;
} ui_t;

typedef struct {
    camera_t camera;

    size_t ndrawcalls;
    drawcall_t* drawcalls;

    size_t ntranslucent_drawcalls;
    drawcall_t* translucent_drawcalls;

    shader_t* world_shader;
    shader_t* viewmodel_shader;
    shader_t* ambient_light_shader;
    shader_t* sun_light_shader;
    shader_t* sun_shadow_shader;
    shader_t* point_light_shader;
    shader_t* skybox_shader;
    shader_t* screen_shader;
    shader_t* current_shader;
    mesh_t* quad_mesh;
    mesh_t* sphere_mesh;
    material_t* default_material;

    framebuffer_t* gbuffer;
    framebuffer_t* screen_buffer;
    framebuffer_t* sun_shadow_buffer;

    mat4 projection;
    mat4 view;

    ui_t ui;
} renderer_t;

void camera_init(sbox_t* sbox, camera_t* camera);
void camera_tick(sbox_t* sbox, camera_t* camera);
void camera_add_pitch(camera_t* camera, float pitch);
void camera_add_yaw(camera_t* camera, float yaw);
void camera_add_roll(camera_t* camera, float roll);
void camera_get_projection_matrix(camera_t* camera, int width, int height, mat4 proj);
void camera_get_view_matrix(camera_t* camera, mat4 view);

shader_t* shader_new(sbox_t* sbox, const char* vs, const char* vname, const char* fs, const char* fname);
shader_t* shader_load(sbox_t* sbox, const char* vpath, const char* fpath);
void shader_free(sbox_t* sbox, shader_t* shader);

mesh_t* mesh_new(sbox_t* sbox,
    float* vertices, size_t nvertices,
    uint32_t* indices, size_t nindices,
    uint8_t nmaterials, bbox_t bbox);
mesh_t* mesh_load(sbox_t* sbox, const char* path);
void mesh_free(sbox_t* sbox, mesh_t* mesh);

texture_t* texture_new(sbox_t* sbox, int width, int height, uint8_t* data, texture_format_t format);
texture_t* texture_load(sbox_t* sbox, const char* path);
texture_t* texture_load_cubemap(sbox_t* sbox, const char* paths[6]);
void texture_free(sbox_t* sbox, texture_t* texture);

material_t* material_load(sbox_t* sbox,
    const char* name,
    const char* albedo_path,
    const char* roughness_path,
    const char* normal_path,
    float tilex,
    float tiley,
    bool is_translucent,
    int phys_mat);
void material_free(sbox_t* sbox, material_t* material);

framebuffer_t* framebuffer_new(sbox_t* sbox);
void framebuffer_add_texture(
    sbox_t* sbox, framebuffer_t* framebuffer, int width, int height, texture_format_t format);
void framebuffer_add_depth_buffer(sbox_t* sbox, framebuffer_t* framebuffer, int width, int height);
void framebuffer_finish(sbox_t* sbox, framebuffer_t* framebuffer);
void framebuffer_free(framebuffer_t* framebuffer);

void ui_init(sbox_t* sbox, ui_t* ui);
void ui_draw_tex(sbox_t* sbox, ui_t* ui, texture_t* tex, vec2 pos, vec2 size, vec3 color);
void ui_render(sbox_t* sbox, ui_t* ui, renderer_t* renderer);

void r_init(sbox_t* sbox, renderer_t* renderer);
void r_free(sbox_t* sbox, renderer_t* renderer);
void r_tick(sbox_t* sbox, renderer_t* renderer);
void r_on_resize(sbox_t* sbox);

void r_add_drawcall(renderer_t* renderer, drawcall_t drawcall);
void r_clear_drawcalls(renderer_t* renderer);

void r_set_shader(renderer_t* renderer, shader_t* shader);
void r_set_texture(renderer_t* renderer, texture_t* texture, int slot);
void r_set_material(renderer_t* renderer, const material_t* material, int slot);
void r_set_framebuffer(renderer_t* renderer, framebuffer_t* framebuffer);

void r_set_int(renderer_t* renderer, const char* name, int i);
void r_set_float(renderer_t* renderer, const char* name, float f);
void r_set_vec2(renderer_t* renderer, const char* name, vec2 v);
void r_set_vec3(renderer_t* renderer, const char* name, vec3 v);
void r_set_vec4(renderer_t* renderer, const char* name, vec4 v);
void r_set_mat4(renderer_t* renderer, const char* name, mat4 m);

void r_draw_mesh(const mesh_t* mesh);
void r_render(sbox_t* sbox, renderer_t* renderer);

#endif
