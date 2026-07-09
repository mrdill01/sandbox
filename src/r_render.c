#include "render.h"
#include "sbox.h"

#include "../include/gl.h"

static void render_world(sbox_t* sbox, renderer_t* renderer) {
    r_set_shader(renderer->world_shader);
    r_set_framebuffer(renderer, renderer->gbuffer);
    glViewport(0, 0, sbox->cfg.r_width * sbox->cfg.r_scale, sbox->cfg.r_height * sbox->cfg.r_scale);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    camera_get_projection_matrix(&renderer->camera, sbox->cfg.r_width, sbox->cfg.r_height,
        renderer->projection);
    shader_set_mat4(renderer->world_shader, "projection", renderer->projection);

    camera_get_view_matrix(&renderer->camera, renderer->view);
    shader_set_mat4(renderer->world_shader, "view", renderer->view);

    for (int i = 0; i < renderer->ndrawcalls; i++) {
        drawcall_t* drawcall = &renderer->drawcalls[i];
        shader_set_mat4(renderer->world_shader, "model", drawcall->model);

        for (int i = 0; i < drawcall->nmaterials; i++)
            r_set_material(renderer, renderer->world_shader, drawcall->materials[i], i);
        r_draw_mesh(drawcall->mesh);
    }

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    r_set_framebuffer(renderer, NULL);
}

static void render_viewmodel(sbox_t* sbox, renderer_t* renderer) {
    entity_t* entity = entlist_find_by_name(sbox, &sbox->entlist, "tommy gun");
    if (!entity) return;

    r_set_framebuffer(renderer, renderer->gbuffer);
    glClear(GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    r_set_shader(renderer->viewmodel_shader);
    shader_set_mat4(renderer->viewmodel_shader, "projection", renderer->projection);
    shader_set_mat4(renderer->viewmodel_shader, "view", renderer->view);
    
    shader_set_vec3(renderer->viewmodel_shader, "view_position", renderer->camera.position);

    mat4 model;
    glm_mat4_identity(model);

    glm_vec3_copy(renderer->camera.position, entity->position);

    vec3 forward;
    glm_vec3_copy(renderer->camera.forward, forward);
    glm_vec3_scale(forward, 0.35f, forward);
    glm_vec3_add(entity->position, forward, entity->position);

    vec3 right;
    glm_vec3_copy(renderer->camera.right, right);
    glm_vec3_scale(right, -0.15f, right);
    glm_vec3_add(entity->position, right, entity->position);

    vec3 up;
    glm_vec3_copy(renderer->camera.up, up);
    glm_vec3_scale(up, -0.1f, up);
    glm_vec3_add(entity->position, up, entity->position);
    
    glm_translate(model, entity->position);

    glm_quat(entity->rotation, rad(renderer->camera.angles[0]), 1.0f, 0.0f, 0.0f);
    glm_quat(entity->rotation, rad(-renderer->camera.angles[1] + 90.0f), 0.0f, 1.0f, 0.0f);
    glm_quat_rotate(model, entity->rotation, model);

    shader_set_mat4(renderer->viewmodel_shader, "model", model);

    for (int i = 0; i < entity->nmaterials; i++)
        r_set_material(renderer, renderer->viewmodel_shader, entity->materials[i], i);
    r_draw_mesh(entity->mesh);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    r_set_framebuffer(renderer, NULL);
}

static void render_lighting(sbox_t* sbox, renderer_t* renderer) {
    r_set_shader(renderer->lighting_shader);
    glViewport(0, 0, sbox->cfg.r_width, sbox->cfg.r_height);

    shader_set_int(renderer->lighting_shader, "gbuffer.position", 0);
    shader_set_int(renderer->lighting_shader, "gbuffer.normal", 1);
    shader_set_int(renderer->lighting_shader, "gbuffer.albedo_roughness", 2);

    r_set_texture(renderer->gbuffer->textures[0], 0);
    r_set_texture(renderer->gbuffer->textures[1], 1);
    r_set_texture(renderer->gbuffer->textures[2], 2);

    shader_set_vec3(renderer->lighting_shader, "view_position", renderer->camera.position);

    vec3 light_position = {1.5f, 1.5f, -1.5f};
    vec3 light_color = {8.0f, 8.0f, 8.0f};
    shader_set_vec3(renderer->lighting_shader, "light.position", light_position);
    shader_set_vec3(renderer->lighting_shader, "light.color", light_color);

    r_draw_mesh(renderer->quad_mesh);
}

static void copy_depth(sbox_t* sbox, renderer_t* renderer) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer->gbuffer->id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(
        0, 0, sbox->cfg.r_width * sbox->cfg.r_scale, sbox->cfg.r_height * sbox->cfg.r_scale,
        0, 0, sbox->cfg.r_width * sbox->cfg.r_scale, sbox->cfg.r_height * sbox->cfg.r_scale,
        GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void render_ui(sbox_t* sbox, renderer_t* renderer) {
    r_set_shader(renderer->ui_shader);

    drawcall_t drawcall;
    drawcall.nmaterials = 0;
    drawcall.mesh = renderer->quad_mesh;
    for (int i = 0; i < 32; i++) {
        glm_mat4_identity(drawcall.model);
        
        vec3 position = {i * 1.0f, 0.0f, 0.0f};
        glm_translate(drawcall.model, position);

        r_draw_mesh(renderer->quad_mesh);
    }
}

void r_render(sbox_t* sbox, renderer_t* renderer) {
    render_world(sbox, renderer);
    //render_viewmodel(sbox, renderer);
    render_lighting(sbox, renderer);
    copy_depth(sbox, renderer);
    render_ui(sbox, renderer);

    r_clear_drawcalls(renderer);
    SDL_GL_SwapWindow(sbox->window);
}
