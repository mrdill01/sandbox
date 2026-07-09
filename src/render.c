#include "render.h"
#include "sbox.h"

#include <stdlib.h>
#include <stdio.h>

#include "../include/gl.h"



void r_init(sbox_t* sbox, renderer_t* renderer) {
    camera_init(&renderer->camera);

    renderer->ndrawcalls = 0;
    renderer->drawcalls = NULL;

    renderer->world_shader = shader_load(sbox,
        "res/shaders/world.vs", "res/shaders/world.fs");
    renderer->viewmodel_shader = shader_load(sbox,
        "res/shaders/viewmodel.vs", "res/shaders/viewmodel.fs"); 
    renderer->lighting_shader = shader_load(sbox,
        "res/shaders/lighting.vs", "res/shaders/lighting.fs");   
    renderer->ui_shader = shader_load(sbox,
        "res/shaders/ui.vs", "res/shaders/ui.fs");
    renderer->quad_mesh = mesh_load(sbox, "res/meshes/quad.obj");
    renderer->default_material = material_load(sbox,
        "res/textures/default.png",
        "res/textures/default_r.png",
        "res/textures/default_n.png",
        1, 1, false);

    renderer->gbuffer = framebuffer_new(sbox);
    framebuffer_add_texture(sbox, renderer->gbuffer);
    framebuffer_add_texture(sbox, renderer->gbuffer);
    framebuffer_add_texture(sbox, renderer->gbuffer);
    framebuffer_add_depth_buffer(sbox, renderer->gbuffer);
    framebuffer_finish(sbox, renderer->gbuffer);

    glm_mat4_identity(renderer->projection);
    glm_mat4_identity(renderer->view);

    renderer->ui.font = texture_load(sbox, "res/textures/font.png");
}

void r_free(sbox_t* sbox, renderer_t* renderer) {
    framebuffer_free(renderer->gbuffer);

    int n = 0;
    shader_t* shader = sbox->shaders;
    while (shader) {
        shader_t* next = shader->next;
        shader_free(sbox, shader);
        shader = next;
        n++;
    }

    info(sbox, "released %d shader(s)", n);

    n = 0;
    mesh_t* mesh = sbox->meshes;
    while (mesh) {
        mesh_t* next = mesh->next;
        mesh_free(sbox, mesh);
        mesh = next;
        n++;
    }

    info(sbox, "released %d meshes(s)", n);

    n = 0;
    texture_t* texture = sbox->textures;
    while (texture) {
        texture_t* next = texture->next;
        texture_free(sbox, texture);
        texture = next;
        n++;
    }

    info(sbox, "released %d texture(s)", n);

    n = 0;
    material_t* material = sbox->materials;
    while (material) {
        material_t* next = material->next;
        material_free(sbox, material);
        material = next;
        n++;
    }

    info(sbox, "released %d material(s)", n);
}

void r_add_drawcall(renderer_t* renderer, drawcall_t drawcall) {
    renderer->drawcalls = realloc(renderer->drawcalls,
        sizeof(drawcall_t) * (renderer->ndrawcalls + 1));
    renderer->drawcalls[renderer->ndrawcalls++] = drawcall;
}

void r_set_shader(shader_t* shader) {
    glUseProgram(shader->id);
}

void r_set_texture(texture_t* texture, int slot) {
    if (!texture) return;
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture->id);
}

void r_set_material(renderer_t* renderer, shader_t* shader, material_t* material, int slot) {
    if (!material) return;

    char slot_name[32];
    snprintf(slot_name, 32, "materials[%d].albedo", slot);
    shader_set_int(shader, slot_name, 3 * slot + 0);
    r_set_texture(
        (material->albedo) ?
            material->albedo :
            renderer->default_material->albedo, 3 * slot + 0);
    
    snprintf(slot_name, 32, "materials[%d].roughness", slot);
    shader_set_int(shader, slot_name, 3 * slot + 1);
    r_set_texture(
        (material->roughness) ?
            material->roughness :
            renderer->default_material->roughness, 3 * slot + 1);

    snprintf(slot_name, 32, "materials[%d].normal", slot);
    shader_set_int(shader, slot_name, 3 * slot + 2);
    r_set_texture(
        (material->normal) ?
            material->normal :
            renderer->default_material->normal, 3 * slot + 2);

    snprintf(slot_name, 32, "materials[%d].tilex", slot);
    shader_set_float(shader, slot_name, material->tilex);

    snprintf(slot_name, 32, "materials[%d].tiley", slot);
    shader_set_float(shader, slot_name, material->tiley);
}

void r_set_framebuffer(renderer_t* renderer, framebuffer_t* framebuffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, (framebuffer) ? framebuffer->id : 0);
}

void r_draw_mesh(mesh_t* mesh) {
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->ntris, GL_UNSIGNED_INT, 0);
}

static void render_world(sbox_t* sbox, renderer_t* renderer) {
    r_set_shader(renderer->world_shader);
    r_set_framebuffer(renderer, renderer->gbuffer);

    glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
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

    for (int i = 0; i < sbox->entlist.len; i++) {
        entity_t* entity = sbox->entlist.ents[i];
        if (entity->is_viewmodel) continue;

        mat4 model;
        glm_mat4_identity(model);
        glm_translate(model, entity->position);
        glm_quat_rotate(model, entity->rotation, model);
        shader_set_mat4(renderer->world_shader, "model", model);

        for (int i = 0; i < entity->nmaterials; i++)
            r_set_material(renderer, renderer->world_shader, entity->materials[i], i);
        r_draw_mesh(entity->mesh);
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

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    r_set_framebuffer(renderer, NULL);
}

static void render_lighting(sbox_t* sbox, renderer_t* renderer) {
    r_set_shader(renderer->lighting_shader);

    shader_set_int(renderer->lighting_shader, "gbuffer.position", 0);
    shader_set_int(renderer->lighting_shader, "gbuffer.normal", 1);
    shader_set_int(renderer->lighting_shader, "gbuffer.albedo_roughness", 2);

    r_set_texture(renderer->gbuffer->textures[0], 0);
    r_set_texture(renderer->gbuffer->textures[1], 1);
    r_set_texture(renderer->gbuffer->textures[2], 2);

    shader_set_vec3(renderer->lighting_shader, "view_position", renderer->camera.position);

    vec3 light_position = {1.5f, 1.5f, -1.5f};
    vec3 light_color = {4.0f, 4.0f, 4.0f};
    shader_set_vec3(renderer->lighting_shader, "light.position", light_position);
    shader_set_vec3(renderer->lighting_shader, "light.color", light_color);

    r_draw_mesh(renderer->quad_mesh);
}

void r_render(sbox_t* sbox, renderer_t* renderer) {
    render_world(sbox, renderer);
    //render_viewmodel(sbox, renderer);
    render_lighting(sbox, renderer);  
    SDL_GL_SwapWindow(sbox->window);
}
