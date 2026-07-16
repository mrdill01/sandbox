#include "render.h"
#include "sbox.h"

#include "../include/gl.h"

static void render_shadows(sbox_t* sbox, renderer_t* renderer) {
    r_set_shader(renderer, renderer->sun_shadow_shader);
    r_set_framebuffer(renderer, renderer->sun_shadow_buffer);
    glViewport(0, 0, r_shadow_res.value, r_shadow_res.value);
    glClear(GL_DEPTH_BUFFER_BIT);

    float near = 1.0f;
    float far = 10.0f;
    float frustrum_size = 10.0f;
    mat4 projection;
    glm_ortho(-frustrum_size, frustrum_size, -frustrum_size, frustrum_size, near, far, projection);

    entity_t* sun = entlist_find_by_name(sbox, &sbox->map.entlist, "sun");
    if (!sun) return;

    vec3 sun_position;
    glm_vec3_copy(renderer->camera.position, sun_position);
    
    vec3 offset;
    glm_vec3_copy(sun->data.sun_light.direction, offset);
    glm_vec3_scale(offset, 5.0f, offset);
    glm_vec3_add(sun_position, offset, sun_position);

    mat4 view;
    glm_lookat(sun_position, (vec3){0.0f, 0.0f, 0.0f}, Y_AXIS, view);

    mat4 light_space_matrix;
    glm_mat4_identity(light_space_matrix);
    glm_mat4_mul(light_space_matrix, projection, light_space_matrix);
    glm_mat4_mul(light_space_matrix, view, light_space_matrix);

    r_set_mat4(renderer, "light_space", light_space_matrix);

    for (int i = 0; i < renderer->ndrawcalls; i++) {
        drawcall_t* drawcall = &renderer->drawcalls[i];
        r_set_mat4(renderer, "model", drawcall->model);

        if (drawcall->mesh)
            r_draw_mesh(drawcall->mesh);
    }

    r_set_framebuffer(renderer, NULL);
}

static void render_world(sbox_t* sbox, renderer_t* renderer) {
    r_set_shader(renderer, renderer->world_shader);
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
    camera_get_view_matrix(&renderer->camera, renderer->view);
        
    r_set_mat4(renderer, "projection", renderer->projection);
    r_set_mat4(renderer, "view", renderer->view);

    for (int i = 0; i < renderer->ndrawcalls; i++) {
        drawcall_t* drawcall = &renderer->drawcalls[i];
        r_set_mat4(renderer, "model", drawcall->model);

        for (int i = 0; i < MAX_MATERIALS; i++) {
            const material_t* material = drawcall->materials[i];
            if (!material) continue;
            r_set_material(renderer, material, i);
        }

        if (drawcall->mesh)
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

    r_set_shader(renderer, renderer->viewmodel_shader);
    r_set_mat4(renderer, "projection", renderer->projection);
    r_set_mat4(renderer, "view", renderer->view);
    
    r_set_vec3(renderer, "position", renderer->camera.position);

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

    r_set_mat4(renderer, "model", model);

    for (int i = 0; i < MAX_MATERIALS; i++) {
        const material_t* material = entity->data.prop.materials[i];
        if (!material) continue;
        r_set_material(renderer, material, i);
    }
    
    if (entity->data.prop.mesh)
        r_draw_mesh(entity->data.prop.mesh);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    r_set_framebuffer(renderer, NULL);
}

static void render_ambient_light(sbox_t* sbox, renderer_t* renderer) {
    r_set_framebuffer(renderer, renderer->screen_buffer);
    r_set_shader(renderer, renderer->ambient_light_shader);
    glViewport(0, 0, r_width.value, r_height.value);

    r_set_int(renderer, "gbuffer.position", 0);
    r_set_int(renderer, "gbuffer.albedo_roughness", 1);
    r_set_int(renderer, "gbuffer.normal", 2);
    r_set_int(renderer, "gbuffer.depth", 3);
    r_set_texture(renderer, renderer->gbuffer->textures[0], 0);
    r_set_texture(renderer, renderer->gbuffer->textures[1], 1);
    r_set_texture(renderer, renderer->gbuffer->textures[2], 2);
    r_set_texture(renderer, renderer->gbuffer->textures[3], 3);

    r_set_vec3(renderer, "view_position", renderer->camera.position);

    r_draw_mesh(renderer->quad_mesh);
    r_set_framebuffer(renderer, NULL);
}

static void render_sun_lights(sbox_t* sbox, renderer_t* renderer) {
    r_set_framebuffer(renderer, renderer->screen_buffer);
    r_set_shader(renderer, renderer->sun_light_shader);
    glViewport(0, 0, r_width.value, r_height.value);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    r_set_int(renderer, "gbuffer.position", 0);
    r_set_int(renderer, "gbuffer.albedo_roughness", 1);
    r_set_int(renderer, "gbuffer.normal", 2);
    r_set_int(renderer, "gbuffer.depth", 3);
    r_set_texture(renderer, renderer->gbuffer->textures[0], 0);
    r_set_texture(renderer, renderer->gbuffer->textures[1], 1);
    r_set_texture(renderer, renderer->gbuffer->textures[2], 2);
    r_set_texture(renderer, renderer->gbuffer->textures[3], 3);

    r_set_vec3(renderer, "view_position", renderer->camera.position);

    glm_mat4_copy(GLM_MAT4_IDENTITY, renderer->projection);
    glm_mat4_copy(GLM_MAT4_IDENTITY, renderer->view);

    r_set_mat4(renderer, "projection", renderer->projection);
    r_set_mat4(renderer, "view", renderer->view);

    for (size_t i = 0; i < sbox->map.entlist.len; i++) {
        entity_t* entity = sbox->map.entlist.ents[i];
        if (entity->type != ENTITY_DIR_LIGHT) continue;
        entity_sun_light_t* sun_light = &entity->data.sun_light;

        r_set_vec3(renderer, "light.direction", sun_light->direction);
        r_set_vec3(renderer, "light.color", sun_light->color);

        mat4 model;
        glm_mat4_identity(model);
        r_set_mat4(renderer, "model", model);

        r_draw_mesh(renderer->quad_mesh);
    }

    /*glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);*/
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    r_set_framebuffer(renderer, NULL);
}

static void render_point_lights(sbox_t* sbox, renderer_t* renderer) {
    r_set_framebuffer(renderer, renderer->screen_buffer);
    r_set_shader(renderer, renderer->point_light_shader);
    glViewport(0, 0, r_width.value, r_height.value);

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);
    //glEnable(GL_BLEND);
    /*glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);*/
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    r_set_int(renderer, "gbuffer.position", 0);
    r_set_int(renderer, "gbuffer.albedo_roughness", 1);
    r_set_int(renderer, "gbuffer.normal", 2);
    r_set_int(renderer, "gbuffer.depth", 3);
    r_set_texture(renderer, renderer->gbuffer->textures[0], 0);
    r_set_texture(renderer, renderer->gbuffer->textures[1], 1);
    r_set_texture(renderer, renderer->gbuffer->textures[2], 2);
    r_set_texture(renderer, renderer->gbuffer->textures[3], 3);

    r_set_vec3(renderer, "view_position", renderer->camera.position);

    glm_mat4_copy(GLM_MAT4_IDENTITY, renderer->projection);
    glm_mat4_copy(GLM_MAT4_IDENTITY, renderer->view);

    r_set_mat4(renderer, "projection", renderer->projection);
    r_set_mat4(renderer, "view", renderer->view);

    for (size_t i = 0; i < sbox->map.entlist.len; i++) {
        entity_t* entity = sbox->map.entlist.ents[i];
        if (entity->type != ENTITY_POINT_LIGHT) continue;
        entity_point_light_t* point_light = &entity->data.point_light;

        r_set_vec3(renderer, "light.position", entity->position);
        r_set_vec3(renderer, "light.color", entity->data.point_light.color);

        float scale = 3.0f;
        
        mat4 model;
        glm_mat4_identity(model);
        glm_scale(model, (vec3){scale, scale, scale});
        glm_translate_make(model, entity->position);
        r_set_mat4(renderer, "model", model);

        r_draw_mesh(renderer->quad_mesh);
    }

    /*glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);*/
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    r_set_framebuffer(renderer, NULL);
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
    r_set_framebuffer(renderer, renderer->screen_buffer);
    r_set_shader(renderer, renderer->skybox_shader);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    mat4 projection;
    glm_mat4_copy(renderer->projection, projection);
    glm_mat4_inv(projection, projection);
    r_set_mat4(renderer, "inv_projection", projection);

    mat4 view;
    glm_mat4_copy(renderer->view, view);
    glm_mat4_inv(view, view);
    r_set_mat4(renderer, "inv_view", view);

    r_set_int(renderer, "cubemap", 0);
    r_set_texture(renderer, sbox->map.skybox, 0);

    r_set_int(renderer, "depth", 1);
    r_set_texture(renderer, renderer->gbuffer->textures[3], 1);

    r_draw_mesh(renderer->quad_mesh);

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    r_set_framebuffer(renderer, NULL);
}

static void render_screen(sbox_t* sbox, renderer_t* renderer) {
    r_set_shader(renderer, renderer->screen_shader);

    r_set_int(renderer, "screen", 0);
    r_set_int(renderer, "depth", 1);
    r_set_texture(renderer, renderer->screen_buffer->textures[0], 0);
    r_set_texture(renderer, renderer->gbuffer->textures[3], 1);

    r_set_vec3(renderer, "view_direction", renderer->camera.forward);

    vec3 sun_direction = {0.0f, 0.0f, 0.0f};
    for (size_t i = 0; i < sbox->map.entlist.len; i++) {
        entity_t* entity = sbox->map.entlist.ents[i];
        if (entity->type != ENTITY_DIR_LIGHT) continue;
        glm_vec3_copy(entity->data.sun_light.direction, sun_direction);
    }
    r_set_vec3(renderer, "sun_direction", sun_direction);

    r_draw_mesh(renderer->quad_mesh);
}

void r_render(sbox_t* sbox, renderer_t* renderer) {
    render_world(sbox, renderer);
    //render_viewmodel(sbox, renderer);
    render_ambient_light(sbox, renderer);
    render_sun_lights(sbox, renderer);
    render_point_lights(sbox, renderer);
    //copy_depth(sbox, renderer);
    //render_skybox(sbox, renderer);
    r_set_framebuffer(renderer, renderer->screen_buffer);
    ui_render(sbox, &renderer->ui, renderer);
    r_set_framebuffer(renderer, NULL);

    render_screen(sbox, renderer);

    r_clear_drawcalls(renderer);
    SDL_GL_SwapWindow(sbox->window);
}
