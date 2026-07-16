#include "render.h"
#include "sbox.h"

#include "../include/gl.h"

#define FONT_TILE_WIDTH 32.0f
#define FONT_SPACING 0.25f

void ui_init(sbox_t* sbox, ui_t* ui) {
    info(sbox, "ui_init()...");
    ui->shader = shader_load(sbox, "ui", "res/shaders/ui.vs", "res/shaders/ui.fs");
    ui->font = texture_load(sbox, "res/textures/ui/font.png");
    ui->quad = mesh_load(sbox, "res/meshes/quad.obj");
    glm_mat4_identity(ui->projection);
}

void ui_render_tex(sbox_t* sbox, ui_t* ui,
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

void ui_render(sbox_t* sbox, ui_t* ui, renderer_t* renderer) {
    r_set_shader(renderer, ui->shader);
    glEnable(GL_BLEND);

    glm_ortho(0.0f, r_width.value, r_height.value, 0.0f, -1.0f, 1.0f, ui->projection);
    r_set_mat4(sbox, renderer, "projection", ui->projection);

    float x_size = FONT_TILE_WIDTH / ui->font->width;
    float y_size = FONT_TILE_WIDTH / ui->font->height;
    int chars_per_row = ui->font->width / FONT_TILE_WIDTH;
    const char* text = " !\"#$%%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
        "abcdefghijklmnopqrstuvwxyz{|}~";
    text = "####";
    const int size = 48.0f;

    for (int i = 0; i < strlen(text); i++) {
        int pos = text[i] - ' ';
        int x = pos % chars_per_row;
        int y = pos / chars_per_row;
        ui_render_tex(sbox, ui, ui->font,
            (vec2){i * size * FONT_SPACING, 0.0f}, (vec2){size, size},
            (vec2){x_size * x, y_size * y}, (vec2){x_size, y_size},
            COLOR_WHITE);
        printf("%g\n", i * size * FONT_SPACING);
    }
    
    glDisable(GL_BLEND);
}
