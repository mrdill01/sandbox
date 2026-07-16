#include "render.h"
#include "sbox.h"

#include "../include/gl.h"

void ui_init(sbox_t* sbox, ui_t* ui) {
    info(sbox, "ui_init()...");
    ui->shader = shader_load(sbox, "ui", "res/shaders/ui.vs", "res/shaders/ui.fs");
    ui->font = texture_load(sbox, "res/textures/ui/font.png");
    ui->quad = mesh_load(sbox, "res/meshes/quad.obj");
    glm_mat4_identity(ui->projection);
}

void ui_render_tex(sbox_t* sbox, ui_t* ui, texture_t* tex, vec2 pos, vec2 size, vec3 color) {
    renderer_t* renderer = &sbox->renderer;
    r_set_shader(renderer, ui->shader);

    mat4 model;
    glm_mat4_identity(model);
    glm_scale(model, (vec3){size[0], size[1], 0.0f});
    glm_translate(model, (vec3){pos[0], pos[1], 0.0f});
    r_set_mat4(sbox, renderer, "model", model);

    r_draw_mesh(ui->quad);
}

void ui_render(sbox_t* sbox, ui_t* ui, renderer_t* renderer) {
    glm_mat4_identity(ui->projection);
    glm_ortho(0.0f, r_width.value, r_height.value, 0.0f, -1.0f, 1.0f, ui->projection);
    r_set_mat4(sbox, renderer, "projection", ui->projection);

    ui_render_tex(sbox, ui, ui->font, (vec2){0.0f, 0.0f}, (vec2){100.0f, 100.0f},
        (vec3){1.0f, 0.0f, 1.0f});
}
