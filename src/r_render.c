#include "render.h"
#include "sbox.h"

#include "../include/gl.h"

static void render_world(sbox_t* sbox, renderer_t* renderer) {
    r_set_shader(renderer->world_shader);
    r_set_framebuffer(renderer, renderer->gbuffer);
    glViewport(0, 0, r_width.value * r_scale.value, r_height.value * r_scale.value);

    glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    camera_get_projection_matrix(&renderer->camera, r_width.value, r_height.value,
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
    entity_t* entity = entlist_find_by_name(sbox, &sbox->map.entlist, "tommy gun");
    if (!entity) return;

    r_set_framebuffer(renderer, renderer->gbuffer);
    //glClear(GL_DEPTH_BUFFER_BIT);

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

    for (int i = 0; i < entity->data.prop.nmaterials; i++)
        r_set_material(renderer, renderer->viewmodel_shader, entity->data.prop.materials[i], i);
    r_draw_mesh(entity->data.prop.mesh);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    r_set_framebuffer(renderer, NULL);
}

static void render_ambient_light(sbox_t* sbox, renderer_t* renderer) {
    r_set_shader(renderer->ambient_light_shader);
    glViewport(0, 0, r_width.value, r_height.value);

    shader_set_int(renderer->ambient_light_shader, "gbuffer.position", 0);
    shader_set_int(renderer->ambient_light_shader, "gbuffer.normal", 1);
    shader_set_int(renderer->ambient_light_shader, "gbuffer.albedo_roughness", 2);
    shader_set_int(renderer->ambient_light_shader, "gbuffer.depth", 3);
    r_set_texture(renderer->gbuffer->textures[0], 0);
    r_set_texture(renderer->gbuffer->textures[1], 1);
    r_set_texture(renderer->gbuffer->textures[2], 2);
    r_set_texture(renderer->gbuffer->textures[3], 3);

    shader_set_vec3(renderer->ambient_light_shader, "view_position", renderer->camera.position);

    r_draw_mesh(renderer->quad_mesh);
}

static void render_direct_light(sbox_t* sbox, renderer_t* renderer) {
    r_set_shader(renderer->direct_light_shader);
    glViewport(0, 0, r_width.value, r_height.value);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_BLEND);
    /*glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);*/

    shader_set_int(renderer->direct_light_shader, "gbuffer.position", 0);
    shader_set_int(renderer->direct_light_shader, "gbuffer.normal", 1);
    shader_set_int(renderer->direct_light_shader, "gbuffer.albedo_roughness", 2);
    shader_set_int(renderer->direct_light_shader, "gbuffer.depth", 3);
    r_set_texture(renderer->gbuffer->textures[0], 0);
    r_set_texture(renderer->gbuffer->textures[1], 1);
    r_set_texture(renderer->gbuffer->textures[2], 2);
    r_set_texture(renderer->gbuffer->textures[3], 3);

    shader_set_vec3(renderer->direct_light_shader, "view_position", renderer->camera.position);

    glm_mat4_copy(GLM_MAT4_IDENTITY, renderer->projection);
    glm_mat4_copy(GLM_MAT4_IDENTITY, renderer->view);

    shader_set_mat4(renderer->direct_light_shader, "projection", renderer->projection);
    shader_set_mat4(renderer->direct_light_shader, "view", renderer->view);

    for (size_t i = 0; i < sbox->map.entlist.len; i++) {
        entity_t* entity = sbox->map.entlist.ents[i];
        if (entity->type != ENT_LIGHT) continue;

        shader_set_vec3(renderer->direct_light_shader, "light.position", entity->position);
        shader_set_vec3(renderer->direct_light_shader, "light.color", entity->data.light.color);

        float scale = 3.0f;
        
        mat4 model;
        glm_mat4_identity(model);
        /*glm_translate_make(model, entity->position);
        glm_scale(model, (vec3){scale, scale, scale});*/
        shader_set_mat4(renderer->direct_light_shader, "model", model);

        r_draw_mesh(renderer->quad_mesh);
    }

    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}

static void copy_depth(sbox_t* sbox, renderer_t* renderer) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer->gbuffer->id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(
        0, 0, r_width.value * r_scale.value, r_height.value * r_scale.value,
        0, 0, r_width.value * r_scale.value, r_height.value * r_scale.value,
        GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void render_skybox(sbox_t* sbox, renderer_t* renderer) {
    r_set_shader(renderer->skybox_shader);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    //glDepthFunc(GL_LEQUAL);

    mat4 projection;
    glm_mat4_copy(renderer->projection, projection);
    glm_mat4_inv(projection, projection);
    shader_set_mat4(renderer->skybox_shader, "inv_projection", projection);

    mat4 view;
    glm_mat4_copy(renderer->view, view);
    glm_mat4_inv(view, view);
    shader_set_mat4(renderer->skybox_shader, "inv_view", view);

    shader_set_int(renderer->skybox_shader, "cubemap", 0);
    r_set_texture(sbox->map.skybox, 0);

    shader_set_int(renderer->skybox_shader, "depth", 1);
    r_set_texture(renderer->gbuffer->textures[3], 1);

    r_draw_mesh(renderer->quad_mesh);

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    //glDepthFunc(GL_LESS);
}

void r_render(sbox_t* sbox, renderer_t* renderer) {
    render_world(sbox, renderer);
    //render_viewmodel(sbox, renderer);
    render_ambient_light(sbox, renderer);
    render_direct_light(sbox, renderer);
    //copy_depth(sbox, renderer);
    render_skybox(sbox, renderer);

    r_clear_drawcalls(renderer);
    SDL_GL_SwapWindow(sbox->window);
}
