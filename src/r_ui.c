#include "render.h"
#include "sbox.h"

#include "../include/gl.h"

#define FONT_TILE_SIZE 32.0f
#define FONT_SPACING 0.38f

void ui_init(sbox_t* sbox, ui_t* ui) {
    info(sbox, "ui_init()...");
    ui->shader = shader_load(sbox, "ui", "res/shaders/ui.vs", "res/shaders/ui.fs");
    ui->font = texture_load(sbox, "res/textures/ui/font.png");
    ui->button = texture_load(sbox, "res/textures/ui/button.png");
    ui->crosshair = texture_load(sbox, "res/textures/ui/crosshair.png");
    ui->quad = mesh_load(sbox, "res/meshes/quad.obj");
    glm_mat4_identity(ui->projection);
}

void ui_draw_texture_ex(sbox_t* sbox, ui_t* ui,
    texture_t* texture,
    vec2 dest_pos, vec2 dest_size,
    vec2 src_pos, vec2 src_size,
    vec4 color)
{
    renderer_t* renderer = &sbox->renderer;

    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, (vec3){dest_pos[0], dest_pos[1], 0.0f});
    glm_scale(model, (vec3){dest_size[0], dest_size[1], 0.0f});
    r_set_mat4(sbox, renderer, "model", model);

    r_set_vec2(sbox, renderer, "src_pos", src_pos);
    r_set_vec2(sbox, renderer, "src_size", src_size);
    r_set_vec4(sbox, renderer, "color", color);

    r_set_int(sbox, renderer, "sprite", 0);
    r_set_texture(renderer, texture, 0);

    r_draw_mesh(ui->quad);
}

void ui_draw_texture(sbox_t* sbox, ui_t* ui, texture_t* texture, vec2 pos, vec2 size, vec4 color) {
    ui_draw_texture_ex(sbox, ui, texture, pos, size, (vec2){0.0f, 0.0f}, (vec2){1.0f, 1.0f}, color);
}

void ui_draw_text(
    sbox_t* sbox, ui_t* ui, const char* message, vec2 position, float size, vec4 color)
{
    float x_size = FONT_TILE_SIZE / ui->font->width;
    float y_size = FONT_TILE_SIZE / ui->font->height;
    int chars_per_row = ui->font->width / FONT_TILE_SIZE;

    for (int i = 0; i < strlen(message); i++) {
        int char_index = message[i] - ' ';
        int x = char_index % chars_per_row;
        int y = char_index / chars_per_row;

        ui_draw_texture_ex(sbox, ui, ui->font,
            (vec2){position[0] + i * size * FONT_SPACING, position[1]}, (vec2){size, size},
            (vec2){x * x_size, y * y_size}, (vec2){x_size, y_size},
            color);
    }
}

void ui_draw_text_shadow(
    sbox_t* sbox, ui_t* ui, const char* message, vec2 position, float size, vec4 color)
{
    for (int i = 4; i >= 0; i--) {
        vec2 new_position;
        glm_vec2_copy(position, new_position);
        new_position[0] -= i;
        new_position[1] -= i;

        ui_draw_text(sbox, ui, message, new_position, size, color);
    }
}

static void draw_debug_menu(sbox_t* sbox, ui_t* ui) {
    float font_size = 30.0f;

    char text[64];
    sprintf(text, "FPS: %d", (int)sbox->renderer.fps);
    ui_draw_text(sbox, ui, text, (vec2){0.0f, 0.0f}, font_size, COLOR_WHITE);

    sprintf(text, "x: %g, y: %g, z: %g",
        sbox->player.position[0],
        sbox->player.position[1],
        sbox->player.position[2]);
    ui_draw_text(sbox, ui, text, (vec2){0.0f, font_size}, font_size, COLOR_WHITE);
}

static void draw_hud(sbox_t* sbox, ui_t* ui) {
    vec2 pos = {r_width.value / 2.0f - 20.0f / 2.0f, r_height.value / 2.0f - 20.0f / 2.0f};
    vec2 size = {20.0f, 20.0f};
    ui_draw_texture(sbox, ui, ui->crosshair, pos, size, COLOR_WHITE);
}

void ui_render(sbox_t* sbox, ui_t* ui, renderer_t* renderer) {
    r_set_shader(renderer, ui->shader);
    glEnable(GL_BLEND);

    glm_ortho(0.0f, r_width.value, r_height.value, 0.0f, -1.0f, 1.0f, ui->projection);
    r_set_mat4(sbox, renderer, "projection", ui->projection);

    draw_debug_menu(sbox, ui);
    draw_hud(sbox, ui);
    
    glDisable(GL_BLEND);
}
