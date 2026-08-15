#ifndef VIDEO_H
#define VIDEO_H

#include "mathlib.h"

#include <stdint.h>
#include <stddef.h>

#define R_GL_MAJ 3
#define R_GL_MIN 3
#define MAX_MATERIALS 4
#define MAX_TEXTURES 16
#define MAX_LINES 4096
#define MAX_PARTICLES 8192
#define ITEM_PREVIEW_RES 256
#define FPS_SAMPLE_RATE 45
#define NUM_STEAM_PARTICLES 4

#define COLOR_WHITE (vec4){1.0f, 1.0f, 1.0f, 1.0f}
#define COLOR_GRAY (vec4){0.5f, 0.5f, 0.5f, 1.0f}
#define COLOR_BLACK (vec4){0.0f, 0.0f, 0.0f, 1.0f}
#define COLOR_MAGENTA (vec4){1.0f, 0.0f, 1.0f, 1.0f}
#define COLOR_RED (vec4){1.0f, 0.0f, 0.0f, 1.0f}
#define COLOR_YELLOW (vec4){1.0f, 1.0f, 0.0f, 1.0f}
#define COLOR_GREEN (vec4){0.0f, 1.0f, 0.0f, 1.0f}
#define COLOR_BLUE (vec4){0.0f, 0.0f, 1.0f, 1.0f}
#define COLOR_LIGHT_BLUE (vec4){0.0f, 0.5f, 1.0f, 1.0f}
#define COLOR_PURPLE (vec4){0.5f, 0.0f, 0.5f, 1.0f}

typedef struct quark_t quark_t;
typedef enum phys_material_t phys_material_t;
typedef struct trace_result_t trace_result_t;

typedef struct camera_t {
    vec3 position;
    quat rotation;
    vec3 angles;
    vec3 shake;
    vec3 right;
    vec3 up;
    vec3 forward;
    float fov;
    float near;
    float far;
} camera_t;

typedef struct shader_t {
    char* name;
    uint32_t id;
    const char* vs_path;
    const char* fs_path;
    struct shader_t* next;
} shader_t;

typedef struct {
    uint32_t vao;
    uint32_t vbo;
    uint32_t ebo;
    size_t nvertices;
    float* vertices;
    uint32_t nindices;
    uint32_t* indices;
} mesh_buffer_t;

typedef struct mesh_t {
    mesh_buffer_t** buffers;
    size_t nbuffers;
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

typedef enum {
    TEX_FILTER_NEAREST,
    TEX_FILTER_LINEAR,
} texture_filter_t;

typedef struct texture_t {
    uint32_t id;
    texture_type_t type;
    int width;
    int height;
    texture_format_t format;
    texture_filter_t filter;
    struct texture_t* next;
} texture_t;

typedef struct material_t {
    char* name;
    texture_t* albedo;
    texture_t* roughness;
    texture_t* normal;
    float tilex;
    float tiley;
    float wind_factor;
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
    bool show_msgbox;
    char* msgbox_message;
    
    shader_t* shader;
    texture_t* font;
    texture_t* button;
    texture_t* button_pressed;
    texture_t* crosshair;
    texture_t* item_slot;
    texture_t* item_slot_active;
    texture_t* pixel;
    mesh_t* quad;
    mat4 projection;
} ui_t;

typedef struct {
    bool is_free;
    mesh_t* mesh;
    vec3 start;
    vec3 end;
    vec4 color;
    float spawn_time;
    float decay_time;
} line_t;

#define PARTICLE_FADE_OUT 1
#define PARTICLE_SCALE_X2 2

typedef struct {
    bool is_free;
    vec3 position;
    vec3 velocity;
    texture_t* texture;
    vec3 color;
    float alpha;
    float init_alpha;
    float size;
    float init_size;
    float spawn_time;
    float lifetime;
    uint32_t flags;
    bool apply_gravity;
    mesh_t* mesh;
    float distance_to_camera;
} particle_t;

typedef struct {
    char* entity;
    mesh_t* mesh;
    material_t* materials[MAX_MATERIALS];
    mat4 model;
    vec3 position;
    mat4 rotation;
    vec3 scale;
    bbox_t local_bbox;
    bbox_t world_bbox;
    float distance_to_camera;
    bool is_translucent;
} drawcall_t;

typedef struct {
    int drawcalls;
    int meshes;
    int tris;
    int textures;
    int materials;
} render_stats_t;

typedef struct {
    camera_t camera;

    float fps;
    float fps_samples[FPS_SAMPLE_RATE];
    int nfps_samples;

    size_t ndrawcalls;
    drawcall_t* drawcalls;

    size_t ntranslucent_drawcalls;
    drawcall_t* translucent_drawcalls;

    shader_t* gbuffer_shader;
    shader_t* viewmodel_shader;
    shader_t* ambient_light_shader;
    shader_t* sun_light_shader;
    shader_t* sun_shadow_shader;
    shader_t* point_light_shader;
    shader_t* forward_shader;
    shader_t* skybox_shader;
    shader_t* partfx_shader;
    shader_t* screen_shader;
    shader_t* line_shader;
    shader_t* item_shader;
    shader_t* active_shader;

    mesh_t* quad_mesh;
    mesh_t* sphere_mesh;

    texture_t* bound_textures[MAX_TEXTURES];

    material_t* default_material;

    framebuffer_t* gbuffer;
    framebuffer_t* screen_buffer;
    framebuffer_t* sun_shadow_buffer;
    framebuffer_t* item_fbos[32];

    mat4 projection;
    mat4 view;

    ui_t ui;

    line_t lines[MAX_LINES];

    texture_t* p_fire;
    texture_t* p_cross;
    texture_t* p_corona;
    texture_t* p_smoke;
    texture_t* p_steam[NUM_STEAM_PARTICLES];
    texture_t* p_bullet_hole;
    texture_t* p_water;
    texture_t* p_blood;
    texture_t* p_coin;

    particle_t particles[MAX_PARTICLES];

    render_stats_t stats;
} renderer_t;

void camera_init(quark_t* quark, camera_t* camera);
void camera_tick(quark_t* quark, camera_t* camera);
void camera_add_pitch(camera_t* camera, float pitch);
void camera_add_yaw(camera_t* camera, float yaw);
void camera_add_roll(camera_t* camera, float roll);
void camera_get_projection_matrix(camera_t* camera, int width, int height, mat4 proj);
void camera_get_view_matrix(camera_t* camera, mat4 view);

shader_t* shader_new(quark_t* quark,
    const char* name,
    const char* vs, const char* vname,
    const char* fs, const char* fname);
shader_t* shader_load(quark_t* quark, const char* name, const char* vpath, const char* fpath);
void shader_free(quark_t* quark, shader_t* shader);

mesh_buffer_t* mesh_buffer_new(quark_t* quark, size_t num_vertices, size_t num_indices);
void mesh_buffer_upload(quark_t* quark, mesh_buffer_t* buffer);

mesh_t* mesh_new(quark_t* quark,
    mesh_buffer_t** buffers, size_t nbuffers, uint8_t nmaterials, bbox_t bbox);
mesh_t* mesh_load(quark_t* quark, const char* path);
mesh_t* mesh_copy(quark_t* quark, const mesh_t* original);
void mesh_free(quark_t* quark, mesh_t* mesh);
void mesh_deform(
    quark_t* quark, mesh_t* mesh, vec3 position, vec3 point, vec3 normal, float radius, float distance);

texture_t* texture_new(quark_t* quark, int width, int height, uint8_t* data,
    texture_format_t format, texture_filter_t filter);
texture_t* texture_load(quark_t* quark, const char* path, texture_filter_t filter);
texture_t* texture_load_cubemap(quark_t* quark, const char* paths[6]);
void texture_free(quark_t* quark, texture_t* texture);

material_t* material_load(quark_t* quark,
    const char* name,
    const char* albedo_path,
    const char* roughness_path,
    const char* normal_path,
    float tilex,
    float tiley,
    bool is_translucent,
    int phys_mat);
void material_free(quark_t* quark, material_t* material);

framebuffer_t* framebuffer_new(quark_t* quark);
void framebuffer_add_texture(
    quark_t* quark, framebuffer_t* framebuffer, int width, int height, texture_format_t format);
void framebuffer_add_depth_buffer(quark_t* quark, framebuffer_t* framebuffer, int width, int height);
void framebuffer_finish(quark_t* quark, framebuffer_t* framebuffer);
void framebuffer_free(framebuffer_t* framebuffer);

void ui_init(quark_t* quark, ui_t* ui);

void ui_draw_texture_ex(quark_t* quark, ui_t* ui,
    texture_t* texture,
    vec2 dest_pos, vec2 dest_size,
    vec2 src_pos, vec2 src_size,
    vec4 color);
void ui_draw_texture(
    quark_t* quark, ui_t* ui, texture_t* texture, vec2 pos, vec2 size, vec4 color);

void ui_draw_text(
    quark_t* quark, ui_t* ui, const char* message, vec2 position, float size, vec4 color);
void ui_draw_text_bg(
    quark_t* quark, ui_t* ui, const char* message, vec2 position, float size, vec4 color);
void ui_draw_text_thick(
    quark_t* quark, ui_t* ui, const char* message, vec2 position, float size, int w, vec4 color);
float ui_measure_text(const char* message, float size);

bool ui_draw_button(
    quark_t* quark, ui_t* ui, const char* message, vec2 position, vec2 size);
void ui_render(quark_t* quark, ui_t* ui, renderer_t* renderer);

void r_init(quark_t* quark, renderer_t* renderer);
void r_free(quark_t* quark, renderer_t* renderer);
void r_tick(quark_t* quark, renderer_t* renderer);

void r_reload(quark_t* quark, renderer_t* renderer);
void r_on_resize(quark_t* quark);
void r_on_toggle_fullscreen(quark_t* quark);

void r_add_line(quark_t* quark,
    renderer_t* renderer, vec3 start, vec3 end, vec4 color, float decay_time);
void r_add_line_box(quark_t* quark,
    renderer_t* renderer, const bbox_t* box, vec4 color, float decay_time);
void r_render_lines(quark_t* quark, renderer_t* renderer);

void r_add_partfx_shoot_hit(
    quark_t* quark, renderer_t* renderer, trace_result_t trace);
void r_add_partfx_shoot_hit_water(
    quark_t* quark, renderer_t* renderer, trace_result_t trace);
void r_add_partfx_shoot_beam(
    quark_t* quark, renderer_t* renderer, vec3 start, vec3 dir, float distance);
void r_add_partfx_projectile_trail(
    quark_t* quark, renderer_t* renderer, vec3 position);
void r_add_partfx_hit_ground(
    quark_t* quark, renderer_t* renderer, vec3 position, material_t* material);
void r_add_partfx_enter_water(
    quark_t* quark, renderer_t* renderer, vec3 position, vec3 velocity);
void r_add_partfx_step_water(
    quark_t* quark, renderer_t* renderer, vec3 position, vec3 velocity);
void r_add_partfx_explosion(
    quark_t* quark, renderer_t* renderer, vec3 position, float radius);
void r_add_partfx_pickup_coin(quark_t* quark, renderer_t* renderer, vec3 position);

particle_t* r_add_particle(
    quark_t* quark,
    renderer_t* renderer,
    vec3 position,
    vec3 velocity,
    texture_t* texture,
    vec3 color,
    float alpha,
    float size,
    float lifetime,
    uint32_t flags);
void r_tick_particles(quark_t* quark, renderer_t* renderer);
void r_render_particles(quark_t* quark, renderer_t* renderer);
int r_get_particle_count(quark_t* quark, renderer_t* renderer);

void r_add_drawcall(renderer_t* renderer, drawcall_t drawcall);
void r_clear_drawcalls(renderer_t* renderer);

void r_set_shader(renderer_t* renderer, shader_t* shader);
void r_set_texture(
    quark_t* quark, renderer_t* renderer, const char* name, texture_t* texture, int slot);
void r_set_material(quark_t* quark, renderer_t* renderer, const material_t* material, int slot);
void r_set_framebuffer(renderer_t* renderer, framebuffer_t* framebuffer);

void r_set_int(quark_t* quark, renderer_t* renderer, const char* name, int i);
void r_set_float(quark_t* quark, renderer_t* renderer, const char* name, float f);
void r_set_vec2(quark_t* quark, renderer_t* renderer, const char* name, vec2 v);
void r_set_vec3(quark_t* quark, renderer_t* renderer, const char* name, vec3 v);
void r_set_vec4(quark_t* quark, renderer_t* renderer, const char* name, vec4 v);
void r_set_mat4(quark_t* quark, renderer_t* renderer, const char* name, mat4 m);

void r_draw_mesh(renderer_t* renderer, const mesh_t* mesh);
void r_render(quark_t* quark, renderer_t* renderer);
void r_reset_stats(quark_t* quark, renderer_t* renderer);

#endif
