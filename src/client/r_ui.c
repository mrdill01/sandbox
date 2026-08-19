#include "render.h"
#include "../shared/quark.h"

#include <malloc.h>

#include "../../include/gl.h"

#define FONT_TILE_SIZE 32.0f
#define FONT_SPACING 0.38f

void ui_init(quark_t* quark, ui_t* ui) {
    info(quark, "ui_init()...");
    ui->show_msgbox = false;
    ui->msgbox_message[0] = '\0';
    
    ui->shader = shader_load(quark, "ui", "res/shaders/ui.vs", "res/shaders/ui.fs");
    ui->font = texture_load(quark, "res/textures/ui/font.png", TEX_FILTER_NEAREST);
    ui->button = texture_load(quark, "res/textures/ui/button.png", TEX_FILTER_NEAREST);
    ui->button_pressed = texture_load(quark, "res/textures/ui/button_pressed.png", TEX_FILTER_NEAREST);
    ui->item_slot = texture_load(quark, "res/textures/ui/item_slot.png", TEX_FILTER_NEAREST);
    ui->item_slot_active = texture_load(quark,
        "res/textures/ui/item_slot_active.png", TEX_FILTER_NEAREST);
    ui->crosshair = texture_load(quark, "res/textures/ui/crosshair.png", TEX_FILTER_NEAREST);
    ui->pixel = texture_load(quark, "res/textures/ui/pixel.png", TEX_FILTER_NEAREST);
    ui->quad = mesh_load(quark, "res/meshes/quad.obj");
    glm_mat4_identity(ui->projection);
}

void ui_draw_texture_ex(quark_t* quark, ui_t* ui,
    texture_t* texture,
    vec2 dest_pos, vec2 dest_size,
    vec2 src_pos, vec2 src_size,
    vec4 color)
{
    renderer_t* renderer = &quark->renderer;

    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, (vec3){dest_pos[0], dest_pos[1], 0.0f});
    glm_scale(model, (vec3){dest_size[0], dest_size[1], 0.0f});
    r_set_mat4(quark, renderer, "model", model);

    r_set_vec2(quark, renderer, "src_pos", src_pos);
    r_set_vec2(quark, renderer, "src_size", src_size);
    r_set_vec4(quark, renderer, "color", color);
    r_set_texture(quark, renderer, "sprite", texture, 0);

    r_draw_mesh(renderer, ui->quad);
}

void ui_draw_texture(
    quark_t* quark, ui_t* ui, texture_t* texture, vec2 pos, vec2 size, vec4 color)
{
    ui_draw_texture_ex(quark, ui, texture, pos, size, (vec2){0.0f, 0.0f}, (vec2){1.0f, 1.0f}, color);
}

void ui_draw_text(
    quark_t* quark, ui_t* ui, const char* message, vec2 position, float size, vec4 color)
{
    float x_size = FONT_TILE_SIZE / ui->font->width;
    float y_size = FONT_TILE_SIZE / ui->font->height;
    int chars_per_row = ui->font->width / FONT_TILE_SIZE;

    int lines = 0;
    for (int j = 0; j < strlen(message); j++) {
        int char_index = message[j] - ' ';
        if (message[j] == '\n')
            lines++;
        
        int x = char_index % chars_per_row;
        int y = char_index / chars_per_row;

        vec2 new_position;
        new_position[0] = position[0] + j * size * FONT_SPACING;
        new_position[1] = position[1] + lines * size;
        
        ui_draw_texture_ex(quark, ui, ui->font,
            new_position,
            (vec2){size, size},
            (vec2){x * x_size, y * y_size},
            (vec2){x_size, y_size},
            color);
    }
}

void ui_draw_text_shadow(
    quark_t* quark, ui_t* ui, const char* message, vec2 position, float size, vec4 color)
{
    vec2 new_position;
    glm_vec2_copy(position, new_position);
    new_position[0] += 1;
    new_position[1] += 1;
    ui_draw_text(quark, ui, message, new_position, size, COLOR_BLACK);
    ui_draw_text(quark, ui, message, position, size, color);
}

void ui_draw_text_thick(
    quark_t* quark, ui_t* ui, const char* message, vec2 position, float size, int w, vec4 color)
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
        ui_draw_text(quark, ui, message, new_position, size, new_color);
    }
}

float ui_measure_text(const char* message, float size) {
    return (strlen(message) + 1) * size * FONT_SPACING;
}

bool ui_draw_button(
    quark_t* quark, ui_t* ui, const char* message, vec2 position, vec2 size)
{
    bool is_hovered = quark->mx >= position[0] &&
        quark->my >= position[1] &&
        quark->mx < position[0] + size[0] &&
        quark->my < position[1] + size[1];
    bool is_pressed = is_hovered && quark->buttons[SDL_BUTTON_LEFT];

    vec2 position_copy;
    vec2 size_copy;
    glm_vec2_copy(position, position_copy);
    glm_vec2_copy(size, size_copy);

    vec4 text_color;
    glm_vec4_copy(COLOR_GRAY, text_color);

    if (is_pressed) {
        position_copy[0] += 8.0f;
        position_copy[1] += 8.0f;
        size_copy[0] -= 16.0f;
        size_copy[1] -= 16.0f;
    } else if (is_hovered) {
        position_copy[0] += 4.0f;
        position_copy[1] += 4.0f;
        size_copy[0] -= 8.0f;
        size_copy[1] -= 8.0f;
        glm_vec4_copy(COLOR_WHITE, text_color);
    }

    texture_t* texture = (is_pressed) ? ui->button_pressed : ui->button;
    ui_draw_texture(quark, ui, texture, position_copy, size_copy, COLOR_WHITE);

    const float font_size = (is_hovered) ? 30.0f : 34.0f;
    vec2 text_position = {
        position_copy[0] + size_copy[0] / 2.0f - ui_measure_text(message, font_size) / 2.0f,
        position_copy[1] + size_copy[1] / 2.0f - font_size / 2.0f};
    ui_draw_text_thick(quark, ui, message, text_position, font_size, 4, text_color);

    return is_hovered && quark->prev_buttons[SDL_BUTTON_LEFT] && !quark->buttons[SDL_BUTTON_LEFT];
}

static void draw_main_menu(quark_t* quark, ui_t* ui) {
    ui_draw_texture(quark,
        ui,
        ui->pixel,
        (vec2){0.0f, 0.0f},
        (vec2){r_width.value, r_height.value},
        COLOR_LIGHT_BLUE);
    
    vec2 button_size = {256.0f, 48.0f};
    vec2 position = {
        r_width.value / 2.0f - (button_size[0] / 2.0f) * 2.0f,
        r_height.value / 2.0f - (button_size[1] / 2.0f) * 2.0f};

    vec2 text_position;
    glm_vec2_copy(position, text_position);
    text_position[0] += button_size[0] + 16.0f;
    ui_draw_text_thick(quark, ui, "MAIN MENU", text_position, 64.0f, 12, COLOR_WHITE);

    if (ui_draw_button(quark, ui, "START GAME", position, button_size)) {
        cmd_run(quark, "host", NULL, 0);

        const char* args[] = {"127.0.0.1", "25565"};
        cmd_run(quark, "connect", args, 2);
    }

    position[1] += button_size[1];
    ui_draw_button(quark, ui, "SETTINGS", position, button_size);

    position[1] += button_size[1];
    if (ui_draw_button(quark, ui, "QUIT", position, button_size))
        cmd_run(quark, "quit", NULL, 0);

    int font_size = 30.0f;
    float width = ui_measure_text(QUARK_VERSION, font_size);
    ui_draw_text_shadow(quark, ui, QUARK_VERSION,
        (vec2){r_width.value - width - 10.0f, r_height.value - font_size}, font_size, COLOR_WHITE);
}

static void draw_loading_screen(quark_t* quark, ui_t* ui) {
    ui_draw_texture(quark,
        ui, ui->pixel,
        (vec2){0.0f, 0.0f}, (vec2){r_width.value, r_height.value}, COLOR_LIGHT_BLUE);

    float font_size = 64.0f;
    const char* text = "LOADING...";
    vec2 position = {
        r_width.value / 2.0f - ui_measure_text(text, font_size) / 2.0f,
        r_height.value / 2.0f - font_size / 2.0f};
    ui_draw_text_thick(quark, ui, text, position, font_size, 4, COLOR_WHITE);

    font_size = 32.0f;
    text = quark->progress_text;
    ui_draw_texture(quark,
        ui, ui->pixel,
        (vec2){
            r_width.value / 2.0f - ui_measure_text(text, font_size) / 2.0f,
            r_height.value / 2.0f + 64.0f + 4.0f},
        (vec2){ui_measure_text(text, font_size), 24.0f},
        (vec4){0.0f, 0.0f, 0.0f, 1.0f});

    glm_vec2_copy((vec2){
        r_width.value / 2.0f - ui_measure_text(text, font_size) / 2.0f,
        r_height.value / 2.0f + 64.0f},
        position);
    ui_draw_text_thick(quark, ui, text, position, font_size, 4, COLOR_WHITE);
}

static void draw_debug_menu(quark_t* quark, renderer_t* renderer, ui_t* ui) {
    if (!r_debug_menu.value || !quark->player) return;

    float font_size = 30.0f;
    float spacing = font_size * 0.65f;

    char text[64];

    vec2 position = {0.0f, 0.0f};
    sprintf(text, "%d FPS", (int)quark->renderer.fps);
    ui_draw_text_shadow(quark, ui, text, position, font_size, COLOR_WHITE);

    position[1] += spacing;
    sprintf(text, "x %.3g, y %.3g, z %.3g",
        quark->player->position[0],
        quark->player->position[1],
        quark->player->position[2]);
    ui_draw_text_shadow(quark, ui, text, position, font_size, COLOR_WHITE);

    position[1] += spacing;
    sprintf(text, "rot x %.3g, y %.3g, z %.3g",
        quark->renderer.camera.angles[0],
        quark->renderer.camera.angles[1],
        quark->renderer.camera.angles[2]);
    ui_draw_text_shadow(quark, ui, text, position, font_size, COLOR_WHITE);

    vec3 velocity = {quark->player->velocity[0], 0.0f, quark->player->velocity[2]};
    position[1] += spacing;
    sprintf(text, "speed: %.2g", glm_vec3_dot(velocity, velocity));
    ui_draw_text_shadow(quark, ui, text, position, font_size, COLOR_WHITE);

    position[1] += spacing;
    sprintf(text, "on ground: %d, water level: %g",
        quark->player->is_grounded,
        quark->player->water_level);
    ui_draw_text_shadow(quark, ui, text, position, font_size, COLOR_WHITE);

    position[1] += spacing;
    sprintf(text, "server: %s", (quark->server.is_running) ? "running" : "stopped");
    ui_draw_text_shadow(quark, ui, text, position, font_size, COLOR_WHITE);

    position[1] += spacing;
    if (cl_is_connected(quark, &quark->client))
        sprintf(text, "client: connected (%d ms)", cl_get_ping(quark, &quark->client));
    else
        sprintf(text, "client: disconnected");
    ui_draw_text_shadow(quark, ui, text, position, font_size, COLOR_WHITE);

    position[1] += spacing;
    sprintf(text, "draw calls: %d", renderer->stats.drawcalls);
    ui_draw_text_shadow(quark, ui, text, position, font_size, COLOR_WHITE);

    position[1] += spacing;
    sprintf(text, "meshes: %d", renderer->stats.meshes);
    ui_draw_text_shadow(quark, ui, text, position, font_size, COLOR_WHITE);

    position[1] += spacing;
    sprintf(text, "triangles: %d", renderer->stats.tris);
    ui_draw_text_shadow(quark, ui, text, position, font_size, COLOR_WHITE);

    position[1] += spacing;
    sprintf(text, "textures: %d", renderer->stats.textures);
    ui_draw_text_shadow(quark, ui, text, position, font_size, COLOR_WHITE);

    position[1] += spacing;
    sprintf(text, "materials: %d", renderer->stats.materials);
    ui_draw_text_shadow(quark, ui, text, position, font_size, COLOR_WHITE);

    position[1] += spacing;
    sprintf(text, "particles: %d", r_get_particle_count(quark, &quark->renderer));
    ui_draw_text_shadow(quark, ui, text, position, font_size, COLOR_WHITE);

    position[1] += spacing;
    sprintf(text, "audio sources: %d/%d",
        quark->audio.sounds_playing,
        a_get_max_source_count(quark, &quark->audio));
    ui_draw_text_shadow(quark, ui, text, position, font_size, COLOR_WHITE);
}

static void draw_hotbar(quark_t* quark, ui_t* ui) {
    if (quark->player->vehicle != -1 || edit_mode.value) return;

    const inventory_t* inventory = &quark->player->inventory;
    vec2 size = {48.0f, 48.0f};
    vec2 position = {0.0f, r_height.value - (size[1] * HOTBAR_SLOTS)};

    for (int i = 0; i < HOTBAR_SLOTS; i++) {
        item_t* item = inventory->items[i];
        texture_t* texture = (inventory->item_slot == i) ?
            ui->item_slot_active : ui->item_slot;
        ui_draw_texture(quark, ui, texture, position, size, COLOR_WHITE);

        if (item) {
            vec2 icon_position = {position[0] + 4.0f, position[1] + 4.0f};
            vec2 icon_size = {size[0] - 8.0f, size[1] - 8.0f};
            ui_draw_texture(quark, ui,
                quark->renderer.item_fbos[i]->textures[0],
                icon_position,
                icon_size,
                COLOR_WHITE);
        }

        char text[2];
        sprintf(text, "%d", i + 1);
        vec4 color;
        glm_vec4_copy((inventory->item_slot == i) ? COLOR_BLACK : COLOR_WHITE, color);
        ui_draw_text_thick(quark, ui, text, position, 24.0f, 4, color);

        if (inventory->item_slot == i && quark->time - inventory->last_switch < 4.0f) {
            vec2 new_position;
            new_position[0] = position[0] + size[0];
            new_position[1] = position[1];

            vec2 new_size = {200.0f, 48.0f};
            float alpha = lerp(0.4f, 0.0f,
                clamp((quark->time - inventory->last_switch) - 3.0f, 0.0f, 1.0f));
            
            ui_draw_texture(quark, ui, ui->pixel,
                new_position, new_size, (vec4){0.0f, 0.0f, 0.0f, alpha});

            alpha = lerp(1.0f, 0.0f,
                clamp((quark->time - inventory->last_switch) - 3.0f, 0.0f, 1.0f));
            vec4 new_color = {1.0f, 1.0f, 1.0f, alpha};
            ui_draw_text_thick(quark, ui,
                (item) ? item->name : "none", new_position, 24.0f, 4, new_color);
        }

        position[1] += size[1];
    }
}

static void draw_inventory(quark_t* quark, ui_t* ui) {
    if (quark->player->vehicle != -1|| edit_mode.value) return;

    const inventory_t* inventory = &quark->player->inventory;
    if (!inventory->is_open) return;

    vec2 size = {48.0f, 48.0f};
    vec2 start = {size[0], r_height.value - (size[1] * 4.0f)};

    for (int x = 0; x < INVENTORY_WIDTH; x++) {
        for (int y = 0; y < INVENTORY_HEIGHT; y++) {
            vec2 position = {
                start[0] + x * size[0],
                start[1] + y * size[1]};
            ui_draw_texture(quark, ui, ui->item_slot, position, size, COLOR_WHITE);
            position[1] += size[1];
        }
    }
}

static void draw_hud(quark_t* quark, ui_t* ui, player_t* player) {
    if (!r_hud.value) return;
    
    if (!(player->buttons & PLAYER_BUTTON_AIM) && player->move_mode != MOVE_SPRINT) {
        vec2 size = {20.0f, 20.0f};
        vec2 position = {
            r_width.value / 2.0f - size[0] / 2.0f,
            r_height.value / 2.0f - size[1] / 2.0f};
        ui_draw_texture(quark, ui, ui->crosshair, position, size, COLOR_WHITE);
    }

    item_t* item = inventory_get_item(quark, &quark->player->inventory);
    if (item && !edit_mode.value) {
        weapon_t* weapon = &item->data.weapon;
        if (weapon && quark->player->vehicle == -1) {
            char text[32];
            if (sv_inf_ammo.value)
                sprintf(text, "ammo %d/INF", weapon->ammo_loaded);
            else
                sprintf(text, "ammo %d/%d", weapon->ammo_loaded, weapon->ammo_unloaded);
            float font_size = 64.0f;
            ui_draw_text_thick(quark, ui, text,
                (vec2){64.0f + 10.0f, r_height.value - font_size},
                font_size, 5, (weapon->ammo_loaded == 0) ? COLOR_RED : COLOR_WHITE);

            if (weapon->is_reloading) {
                sprintf(text, "RELOADING");
                float font_size = 32.0f;
                float width = ui_measure_text(text, font_size);
                ui_draw_text_thick(quark, ui, text,
                    (vec2){r_width.value / 2.0f - width / 2.0f, r_height.value / 2.0f + 100.0f},
                    font_size, 2, COLOR_GREEN);
            }
        }
    }
    
    if (quark->player->vehicle == -1 && !edit_mode.value) {
        char text[32];
        sprintf(text, "health %d", (int)ceilf(quark->player->health));
        float font_size = 64.0f;
        float width = ui_measure_text(text, font_size);
        ui_draw_text_thick(quark, ui, text,
            (vec2){r_width.value - width - 32.0f, r_height.value - font_size},
            font_size, 5, (quark->player->health <= 30.0f) ? COLOR_RED : COLOR_WHITE);
    }
    
    if (!edit_mode.value) {
        char text[32];
        float font_size = 32.0f;
        if (quark->player->inventory.coins >= quark->map.coins) {
            sprintf(text, "ALL COINS COLLECTED!");
            int width = ui_measure_text(text, font_size);
            ui_draw_text_shadow(quark, ui, text,
                (vec2){r_width.value - width - 5.0f, 0.0f},
                font_size, COLOR_YELLOW);
            
        } else {
            sprintf(text, "COINS: %d/%d", quark->player->inventory.coins, quark->map.coins);
            int width = ui_measure_text(text, font_size);
            ui_draw_text_shadow(quark, ui, text,
                (vec2){r_width.value - width - 5.0f, 0.0f},
                font_size, COLOR_YELLOW);
        }

        char timer[32];
        int seconds = (int)(quark->gm.timer) / 60;
        int minutes = (int)(quark->gm.timer) % 60;
        sprintf(timer, "%02d:%02d", seconds, minutes);

        font_size = 32.0f;
        int width = ui_measure_text(timer, font_size);
        ui_draw_text_shadow(quark, ui, timer,
            (vec2){r_width.value / 2.0f - width / 2.0f, 0.0f}, font_size, COLOR_WHITE);

        width = ui_measure_text(quark->gm.state_name, font_size);
        ui_draw_text_shadow(quark, ui, quark->gm.state_name,
            (vec2){r_width.value / 2.0f - width / 2.0f, font_size * 0.65f}, font_size, COLOR_WHITE);
    }
    
    draw_hotbar(quark, ui);
    draw_inventory(quark, ui);
}

static void draw_edit_mode(quark_t* quark, ui_t* ui) {
    if (!edit_mode.value) return;

    float font_size = 32.0f;
    const char* text = "Edit Mode";
    float width = ui_measure_text(text, font_size);
    ui_draw_text(quark, ui, text,
        (vec2){r_width.value / 2.0f - width / 2.0f, 0.0f}, font_size, COLOR_WHITE);

    if (quark->player->editor.selection && quark->player->editor.selection->name) {
        char text[64];
        sprintf(text, "Selected: %s", quark->player->editor.selection->name);
        float width = ui_measure_text(text, font_size);
        ui_draw_text(quark, ui, text,
            (vec2){r_width.value / 2.0f - width / 2.0f, font_size}, font_size, COLOR_WHITE);
    }
}

static void draw_pause_menu(quark_t* quark, ui_t* ui) {
    ui_draw_texture(quark,
        ui,
        ui->pixel,
        (vec2){0.0f, 0.0f},
        (vec2){r_width.value, r_height.value},
        (vec4){0.0f, 0.0f, 0.0f, 0.4f});

    vec2 button_size = {256.0f, 48.0f};
    vec2 position = {
        r_width.value / 2.0f - (button_size[0] / 2.0f) * 2.0f,
        r_height.value / 2.0f - (button_size[1] / 2.0f) * 2.0f};

    vec2 text_position;
    glm_vec2_copy(position, text_position);
    text_position[0] += button_size[0] + 16.0f;
    ui_draw_text_thick(quark, ui, "GAME PAUSED", text_position, 64.0f, 12, COLOR_WHITE);

    if (ui_draw_button(quark, ui, "RESUME", position, button_size))
        quark->ui_state = UI_STATE_IN_GAME;

    position[1] += button_size[1];
    ui_draw_button(quark, ui, "SETTINGS", position, button_size);

    position[1] += button_size[1];
    if (ui_draw_button(quark, ui, "DISCONNECT", position, button_size))
        cmd_run(quark, "disconnect", NULL, 0);

    int font_size = 30.0f;
    float width = ui_measure_text(QUARK_VERSION, font_size);
    ui_draw_text_shadow(quark, ui, QUARK_VERSION,
        (vec2){r_width.value - width - 10.0f, r_height.value - font_size}, font_size, COLOR_WHITE);
}

static void draw_death_screen(quark_t* quark, ui_t* ui) {
    int width = 400.0f;
    int height = 200.0f;

    ui_draw_texture(quark,
        ui,
        ui->pixel,
        (vec2){r_width.value / 2.0f - width / 2.0f, r_height.value / 2.0f - height / 2.0f},
        (vec2){width, height},
        (vec4){0.0f, 0.0f, 0.0f, 0.4f});

    float font_size = 64.0f;
    const char* text = "GAME OVER";
    float text_width = ui_measure_text(text, font_size);
    ui_draw_text_thick(quark, ui,
        text,
        (vec2){r_width.value / 2.0f - text_width / 2.0f, r_height.value / 2.0f - font_size / 2.0f},
        font_size, 16, COLOR_RED);

    font_size = 32.0f;
    char buffer[64];

    if (quark->time - quark->player->death_time >= sv_respawn_time.value) {
        strcpy(buffer, "Press [SPACE] to respawn");
    
    } else {
        sprintf(buffer, "Respawn in %.2f...",
            sv_respawn_time.value - (quark->time - quark->player->death_time));
    }

    text_width = ui_measure_text(buffer, font_size);
    ui_draw_text_thick(quark, ui,
        buffer,
        (vec2){r_width.value / 2.0f - text_width / 2.0f, r_height.value / 2.0f + font_size / 2.0f},
        font_size, 8, COLOR_WHITE);
}

static void draw_console(quark_t* quark, ui_t* ui, console_t* con) {
    float font_size = 30.0f;
    float spacing = font_size * 0.65f;

    int width = 800.0f;
    int height = CON_LINES_PER_PAGE * spacing + 10.0f;
    int title_height = 40.0f;

    ui_draw_texture(quark,
        ui,
        ui->pixel,
        (vec2){r_width.value / 2.0f - width / 2.0f, 0.0f},
        (vec2){width, title_height},
        (vec4){0.0f, 0.0f, 0.35f, 0.8f});

    ui_draw_text_thick(quark, ui,
        "Console", (vec2){r_width.value / 2.0f - width / 2.0f, 0.0f}, 48.0f, 4, COLOR_WHITE);

    ui_draw_texture(quark,
        ui, ui->pixel,
        (vec2){r_width.value / 2.0f - width / 2.0f, title_height},
        (vec2){width, height},
        (vec4){0.0f, 0.0f, 0.2f, 0.8f});

    for (int i = 0; i < CON_LINES_PER_PAGE; i++) {
        ui_draw_text_shadow(quark, ui,
            con->history[i + con->history_len + con->scroll - CON_LINES_PER_PAGE],
            (vec2){r_width.value / 2.0f - width / 2.0f, title_height + spacing * i},
            font_size, COLOR_WHITE);
    }

    ui_draw_texture(quark,
        ui, ui->pixel,
        (vec2){r_width.value / 2.0f - width / 2.0f, title_height + height},
        (vec2){width, font_size},
        (vec4){0.0f, 0.0f, 0.35f, 0.8f});

    ui_draw_texture(quark,
        ui, ui->pixel,
        (vec2){r_width.value / 2.0f - width / 2.0f + 4.0f, title_height + height + 4.0f},
        (vec2){width - 8.0f, font_size - 8.0f},
        (vec4){0.0f, 0.0f, 0.0f, 1.0f});

    ui_draw_text_shadow(quark, ui,
        con->input,
        (vec2){r_width.value / 2.0f - width / 2.0f, title_height + height},
        font_size, COLOR_WHITE);
}

static void draw_profiler(quark_t* quark, ui_t* ui, profiler_t* prof) {
    for (int i = 0; i < MAX_PROFILER_FUNCS; i++) {
        profiler_entry_t* entry = &prof->entries[i];
        if (!entry->name) continue;

        float font_size = 30.0f;
        float spacing = font_size * 0.65f;
        char text[64];

        sprintf(text, "%s: %f, %d%%", entry->name, entry->time, (int)(entry->percentage * 100.0f));

        float width = ui_measure_text(text, font_size);
        ui_draw_text_shadow(quark, ui,
            text,
            (vec2){r_width.value - width, (i + 2) * spacing},
            font_size, COLOR_WHITE);
    }
}

static void draw_msgbox(quark_t* quark, ui_t* ui) {
    int width = 400.0f;
    int height = 200.0f;

    ui_draw_texture(quark,
        ui, ui->pixel,
        (vec2){r_width.value / 2.0f - width / 2.0f, r_height.value / 2.0f - height / 2.0f},
        (vec2){width, height},
        (vec4){0.0f, 0.0f, 0.0f, 0.4f});

    float font_size = 40.0f;
    float text_width = ui_measure_text(ui->msgbox_message, font_size);
    ui_draw_text_shadow(quark, ui,
        ui->msgbox_message,
        (vec2){r_width.value / 2.0f - text_width / 2.0f, r_height.value / 2.0f - font_size / 2.0f},
        font_size, COLOR_WHITE);

    vec2 button_size = {width, 48.0f};
    vec2 position = {
        r_width.value / 2.0f - button_size[0] / 2.0f,
        r_height.value / 2.0f + height / 2.0f};

    if (ui_draw_button(quark, ui, "OKAY", position, button_size)) {
        ui->show_msgbox = false;
        ui->msgbox_message[0] = '\0';
    }
}

void ui_render(quark_t* quark, ui_t* ui, renderer_t* renderer) {
    prof_start(quark, &quark->prof);

    r_set_shader(renderer, ui->shader);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    glm_ortho(0.0f, r_width.value, r_height.value, 0.0f, -1.0f, 1.0f, ui->projection);
    r_set_mat4(quark, renderer, "projection", ui->projection);

    switch (quark->ui_state) {
    case UI_STATE_MAIN_MENU: {
        draw_main_menu(quark, ui);
        break;
    }
    case UI_STATE_LOADING: {
        draw_loading_screen(quark, ui);
        break;
    }
    case UI_STATE_IN_GAME: {
        if (quark->player) {
            draw_debug_menu(quark, renderer, ui);
            draw_hud(quark, ui, quark->player);
            draw_edit_mode(quark, ui);
        }
        break;
    }
    case UI_STATE_PAUSE_MENU: {
        draw_pause_menu(quark, ui);
        break;
    }
    case UI_STATE_DEAD: {
        draw_death_screen(quark, ui);
        break;
    }
    default: unreachable(quark);
    }

    if (console.value)
        draw_console(quark, ui, &quark->console);

    prof_end(quark, &quark->prof);

    if (profiler.value)
        draw_profiler(quark, ui, &quark->prof);
    
    if (ui->show_msgbox)
        draw_msgbox(quark, ui);

    glDisable(GL_BLEND);
}
