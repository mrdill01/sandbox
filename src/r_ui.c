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
    ui->button_pressed = texture_load(sbox, "res/textures/ui/button_pressed.png");
    ui->item_slot = texture_load(sbox, "res/textures/ui/item_slot.png");
    ui->item_slot_active = texture_load(sbox, "res/textures/ui/item_slot_active.png");
    ui->crosshair = texture_load(sbox, "res/textures/ui/crosshair.png");
    ui->pixel = texture_load(sbox, "res/textures/ui/pixel.png");
    ui->quad = mesh_load(sbox, "res/meshes/quad.obj");
    glm_mat4_identity(ui->projection);
}

void ui_draw_texture_ex(sbox_t* sbox, ui_t* ui,
    const texture_t* texture,
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

    r_draw_mesh(renderer, ui->quad);
}

void ui_draw_texture(
    sbox_t* sbox, ui_t* ui, const texture_t* texture, vec2 pos, vec2 size, vec4 color)
{
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
    vec2 new_position;
    glm_vec2_copy(position, new_position);
    new_position[0] += 1;
    new_position[1] += 1;
    ui_draw_text(sbox, ui, message, new_position, size, COLOR_BLACK);
    ui_draw_text(sbox, ui, message, new_position, size, color);
}

void ui_draw_text_thick(
    sbox_t* sbox, ui_t* ui, const char* message, vec2 position, float size, int w, vec4 color)
{
    vec4 bg_color = {0.0f, 0.0f, 0.0f, 0.1f};
    for (int i = w; i >= 0; i--) {
        vec2 new_position;
        glm_vec2_copy(position, new_position);
        new_position[0] += i;
        new_position[1] += i;

        vec4 new_color;
        new_color[0] = lerp(color[0], bg_color[0], i / (float)w);
        new_color[1] = lerp(color[1], bg_color[1], i / (float)w);
        new_color[2] = lerp(color[2], bg_color[2], i / (float)w);
        new_color[3] = lerp(color[3], bg_color[3], i / (float)w);
        ui_draw_text(sbox, ui, message, new_position, size, new_color);
    }
}

float ui_measure_text(const char* message, float size) {
    return strlen(message) * size * FONT_SPACING;
}

bool ui_draw_button(
    sbox_t* sbox, ui_t* ui, const char* message, vec2 position, vec2 size)
{
    bool is_hovered = sbox->mx >= position[0] &&
        sbox->my >= position[1] &&
        sbox->mx < position[0] + size[0] &&
        sbox->my < position[1] + size[1];
    bool is_pressed = is_hovered && sbox->buttons[SDL_BUTTON_LEFT];

    vec2 position_copy;
    vec2 size_copy;
    glm_vec2_copy(position, position_copy);
    glm_vec2_copy(size, size_copy);

    if (is_hovered) {
        position_copy[0] += 4.0f;
        position_copy[1] += 4.0f;
        size_copy[0] -= 8.0f;
        size_copy[1] -= 8.0f;
    }

    const texture_t* texture = (is_pressed) ? ui->button_pressed : ui->button;
    ui_draw_texture(sbox, ui, texture, position_copy, size_copy, COLOR_WHITE);

    const float font_size = 40.0f;
    vec2 text_position = {
        position_copy[0] + size_copy[0] / 2.0f - ui_measure_text(message, font_size) / 2.0f,
        position_copy[1] + size_copy[1] / 2.0f - font_size / 2.0f};
    ui_draw_text_thick(sbox, ui, message, text_position, font_size, 4, COLOR_WHITE);

    return is_hovered && sbox->prev_buttons[SDL_BUTTON_LEFT] && !sbox->buttons[SDL_BUTTON_LEFT];
}

static void draw_debug_menu(sbox_t* sbox, renderer_t* renderer, ui_t* ui) {
    float font_size = 30.0f;
    float spacing = font_size * 0.65f;

    char text[64];

    vec2 position = {0.0f, 0.0f};
    sprintf(text, "%d FPS", (int)sbox->renderer.fps);
    ui_draw_text_shadow(sbox, ui, text, position, font_size, COLOR_WHITE);

    position[1] += spacing;
    sprintf(text, "x %.3g, y %.3g, z %.3g",
        sbox->player.position[0],
        sbox->player.position[1],
        sbox->player.position[2]);
    ui_draw_text_shadow(sbox, ui, text, position, font_size, COLOR_WHITE);

    position[1] += spacing;
    sprintf(text, "draw calls: %d", renderer->stats.draw_calls);
    ui_draw_text_shadow(sbox, ui, text, position, font_size, COLOR_WHITE);

    position[1] += spacing;
    sprintf(text, "triangles: %d", renderer->stats.tris);
    ui_draw_text_shadow(sbox, ui, text, position, font_size, COLOR_WHITE);

    position[1] += spacing;
    sprintf(text, "textures: %d", renderer->stats.textures);
    ui_draw_text_shadow(sbox, ui, text, position, font_size, COLOR_WHITE);

    position[1] += spacing;
    sprintf(text, "materials: %d", renderer->stats.materials);
    ui_draw_text_shadow(sbox, ui, text, position, font_size, COLOR_WHITE);
}

static void draw_hotbar(sbox_t* sbox, ui_t* ui) {
    const inventory_t* inventory = &sbox->player.inventory;
    vec2 size = {48.0f, 48.0f};
    vec2 position = {0.0f, r_height.value / 2.0f - size[1] / 2.0f};

    for (int i = 0; i < HOTBAR_SLOTS; i++) {
        const texture_t* texture = (inventory->item_slot == i) ?
            ui->item_slot_active : ui->item_slot;
        ui_draw_texture(sbox, ui, texture, position, size, COLOR_WHITE);

        char text[2];
        sprintf(text, "%d", i + 1);
        vec4 color;
        glm_vec4_copy((inventory->item_slot == i) ? COLOR_BLACK : COLOR_WHITE, color);
        ui_draw_text_thick(sbox, ui, text, position, 24.0f, 4, color);

        position[1] += size[1];
    }
}

static void draw_inventory(sbox_t* sbox, ui_t* ui) {
    const inventory_t* inventory = &sbox->player.inventory;
    if (!inventory->is_open) return;

    vec2 size = {48.0f, 48.0f};
    vec2 start = {size[0], r_height.value / 2.0f - size[1] / 2.0f};

    for (int x = 0; x < INVENTORY_WIDTH; x++) {
        for (int y = 0; y < INVENTORY_HEIGHT; y++) {
            vec2 position = {
                start[0] + x * size[0],
                start[1] + y * size[1]};
            ui_draw_texture(sbox, ui, ui->item_slot, position, size, COLOR_WHITE);
            position[1] += size[1];
        }
    }
}

static void draw_hud(sbox_t* sbox, ui_t* ui) {
    vec2 position = {r_width.value / 2.0f - 20.0f / 2.0f, r_height.value / 2.0f - 20.0f / 2.0f};
    vec2 size = {24.0f, 24.0f};
    ui_draw_texture(sbox, ui, ui->crosshair, position, size, COLOR_WHITE);

    draw_hotbar(sbox, ui);
    draw_inventory(sbox, ui);
}

static void draw_pause_menu(sbox_t* sbox, ui_t* ui) {
    ui_draw_texture(sbox,
        ui,
        ui->pixel,
        (vec2){0.0f, 0.0f},
        (vec2){r_width.value, r_height.value},
        (vec4){0.0f, 0.0f, 0.0f, 0.4f});

    vec2 button_size = {256.0f, 64.0f};
    vec2 position = {
        r_width.value / 2.0f - (button_size[0] / 2.0f) * 2.0f,
        r_height.value / 2.0f - (button_size[1] / 2.0f) * 2.0f};

    vec2 text_position;
    glm_vec2_copy(position, text_position);
    text_position[0] += button_size[0] + 16.0f;
    ui_draw_text_thick(sbox, ui, "GAME PAUSED", text_position, 64.0f, 12, COLOR_WHITE);

    if (ui_draw_button(sbox, ui, "RESUME", position, button_size))
        sbox->ui_state = UI_STATE_IN_GAME;

    position[1] += button_size[1];
    ui_draw_button(sbox, ui, "SETTINGS", position, button_size);

    position[1] += button_size[1];
    if (ui_draw_button(sbox, ui, "QUIT", position, button_size))
        sbox->running = false;
}

void ui_render(sbox_t* sbox, ui_t* ui, renderer_t* renderer) {
    r_set_shader(renderer, ui->shader);
    glEnable(GL_BLEND);

    glm_ortho(0.0f, r_width.value, r_height.value, 0.0f, -1.0f, 1.0f, ui->projection);
    r_set_mat4(sbox, renderer, "projection", ui->projection);

    switch (sbox->ui_state) {
    case UI_STATE_IN_GAME: {
        draw_debug_menu(sbox, renderer, ui);
        draw_hud(sbox, ui);
        break;
    }
    case UI_STATE_PAUSE_MENU: {
        draw_pause_menu(sbox, ui);
        break;
    }
    default: unreachable(sbox);
    }
    
    glDisable(GL_BLEND);
}
