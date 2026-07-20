#include "render.h"
#include "sbox.h"

#include "../include/gl.h"

static void render_shadows(sbox_t* sbox, renderer_t* renderer) {
    r_set_shader(renderer, renderer->sun_shadow_shader);
    r_set_framebuffer(renderer, renderer->sun_shadow_buffer);
    glViewport(0, 0, r_shadow_res.value, r_shadow_res.value);
    glClear(GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    entity_t* sun_entity = entlist_find_by_name(sbox, &sbox->map.entlist, "sun");
    if (!sun_entity || sun_entity->type != ENTITY_SUN_LIGHT) {
        error(sbox, "sun light entity 'sun' not found");
        return;
    }

    entity_sun_light_t* sun_light = &sun_entity->data.sun_light;

    float near = 1.0f;
    float far = 32.0f;
    bbox_t frustum = bbox_new((vec3){-far, -far, near}, (vec3){far, far, far});
    mat4 projection;
    glm_ortho(frustum.min[0], frustum.max[0],
        frustum.min[1], frustum.max[1],
        frustum.min[2], frustum.max[2],
        projection);

    vec3 position = {-2.0f, 4.0f, -1.0f};
    //bbox_get_center(&frustum, position);
    //position[1] += 20.0f;

    vec3 target = {0.0f, 0.0f, 0.0f};
    //glm_vec3_copy(position, target);

    //vec3 dir;
    //glm_vec3_copy(sun_light->direction, dir);
    //glm_vec3_inv(dir);

    //glm_vec3_add(target, dir, target);

    mat4 view;
    glm_lookat(position, target, Y_AXIS, view);

    glm_mat4_identity(sun_light->matrix);
    glm_mat4_mul(sun_light->matrix, projection, sun_light->matrix);
    glm_mat4_mul(sun_light->matrix, view, sun_light->matrix);

    r_set_mat4(sbox, renderer, "matrix", sun_light->matrix);

    for (int i = 0; i < renderer->ndrawcalls; i++) {
        drawcall_t* drawcall = &renderer->drawcalls[i];
        r_set_mat4(sbox, renderer, "model", drawcall->model);
        if (drawcall->mesh)
            r_draw_mesh(drawcall->mesh);
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    r_set_framebuffer(renderer, NULL);
}

static void render_gbuffer(sbox_t* sbox, renderer_t* renderer) {
    r_set_shader(renderer, renderer->gbuffer_shader);
    r_set_framebuffer(renderer, renderer->gbuffer);
    glViewport(0, 0, r_width.value * r_scale.value, r_height.value * r_scale.value);

    glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    camera_get_projection_matrix(&renderer->camera, r_width.value, r_height.value,
        renderer->projection);
    camera_get_view_matrix(&renderer->camera, renderer->view);
        
    r_set_mat4(sbox, renderer, "view", renderer->view);
    r_set_mat4(sbox, renderer, "projection", renderer->projection);

    for (int i = 0; i < renderer->ndrawcalls; i++) {
        drawcall_t* drawcall = &renderer->drawcalls[i];
        r_set_mat4(sbox, renderer, "model", drawcall->model);

        for (int i = 0; i < MAX_MATERIALS; i++) {
            const material_t* material = drawcall->materials[i];
            if (!material) continue;
            r_set_material(sbox, renderer, material, i);
        }

        if (drawcall->mesh)
            r_draw_mesh(drawcall->mesh);

        if (r_debug_draw_colliders.value) {
            bbox_t bbox = drawcall->mesh->bbox;
            bbox = bbox_rotate(&bbox, drawcall->rotation);
            bbox = bbox_translate(&bbox, drawcall->position);
            bbox = bbox_scale(&bbox, drawcall->scale);
            line_add_box(sbox, renderer, &bbox, COLOR_LIGHT_BLUE, 2.0f);
        }
    }

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
    r_set_mat4(sbox, renderer, "view", renderer->view);
    r_set_mat4(sbox, renderer, "projection", renderer->projection);
    
    r_set_vec3(sbox, renderer, "position", renderer->camera.position);

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

    r_set_mat4(sbox, renderer, "model", model);

    for (int i = 0; i < MAX_MATERIALS; i++) {
        const material_t* material = entity->data.prop.materials[i];
        if (!material) continue;
        r_set_material(sbox, renderer, material, i);
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

    r_set_int(sbox, renderer, "gbuffer.position", 0);
    r_set_int(sbox, renderer, "gbuffer.albedo_roughness", 1);
    r_set_int(sbox, renderer, "gbuffer.normal", 2);
    r_set_int(sbox, renderer, "gbuffer.depth", 3);
    r_set_texture(renderer, renderer->gbuffer->textures[0], 0);
    r_set_texture(renderer, renderer->gbuffer->textures[1], 1);
    r_set_texture(renderer, renderer->gbuffer->textures[2], 2);
    r_set_texture(renderer, renderer->gbuffer->textures[3], 3);

    r_draw_mesh(renderer->quad_mesh);
    r_set_framebuffer(renderer, NULL);
}

static void render_sun_lights(sbox_t* sbox, renderer_t* renderer) {
    r_set_framebuffer(renderer, renderer->screen_buffer);
    r_set_shader(renderer, renderer->sun_light_shader);
    glViewport(0, 0, r_width.value, r_height.value);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    r_set_int(sbox, renderer, "gbuffer.position", 0);
    r_set_int(sbox, renderer, "gbuffer.albedo_roughness", 1);
    r_set_int(sbox, renderer, "gbuffer.normal", 2);
    r_set_int(sbox, renderer, "gbuffer.depth", 3);
    r_set_texture(renderer, renderer->gbuffer->textures[0], 0);
    r_set_texture(renderer, renderer->gbuffer->textures[1], 1);
    r_set_texture(renderer, renderer->gbuffer->textures[2], 2);
    r_set_texture(renderer, renderer->gbuffer->textures[3], 3);

    r_set_vec3(sbox, renderer, "view_position", renderer->camera.position);

    for (size_t i = 0; i < sbox->map.entlist.len; i++) {
        entity_t* entity = sbox->map.entlist.ents[i];
        if (entity->type != ENTITY_SUN_LIGHT) continue;
        entity_sun_light_t* sun_light = &entity->data.sun_light;

        r_set_vec3(sbox, renderer, "light.direction", sun_light->direction);
        r_set_vec3(sbox, renderer, "light.color", sun_light->color);

        r_set_int(sbox, renderer, "light.shadow", 4);
        r_set_texture(renderer, renderer->sun_shadow_buffer->textures[0], 4);

        r_set_mat4(sbox, renderer, "light.matrix", sun_light->matrix);

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

    r_set_int(sbox, renderer, "gbuffer.position", 0);
    r_set_int(sbox, renderer, "gbuffer.albedo_roughness", 1);
    r_set_int(sbox, renderer, "gbuffer.normal", 2);
    r_set_int(sbox, renderer, "gbuffer.depth", 3);
    r_set_texture(renderer, renderer->gbuffer->textures[0], 0);
    r_set_texture(renderer, renderer->gbuffer->textures[1], 1);
    r_set_texture(renderer, renderer->gbuffer->textures[2], 2);
    r_set_texture(renderer, renderer->gbuffer->textures[3], 3);

    r_set_vec3(sbox, renderer, "view_position", renderer->camera.position);

    for (size_t i = 0; i < sbox->map.entlist.len; i++) {
        entity_t* entity = sbox->map.entlist.ents[i];
        if (entity->type != ENTITY_POINT_LIGHT) continue;
        entity_point_light_t* point_light = &entity->data.point_light;

        r_set_vec3(sbox, renderer, "light.position", entity->position);
        r_set_vec3(sbox, renderer, "light.color", entity->data.point_light.color);

        float scale = 3.0f;
        
        mat4 model;
        glm_mat4_identity(model);
        glm_scale(model, (vec3){scale, scale, scale});
        glm_translate_make(model, entity->position);
        r_set_mat4(sbox, renderer, "model", model);

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
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderer->screen_buffer->id);
    glBlitFramebuffer(
        0, 0, r_width.value * r_scale.value, r_height.value * r_scale.value,
        0, 0, r_width.value * r_scale.value, r_height.value * r_scale.value,
        GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void render_translucent(sbox_t* sbox, renderer_t* renderer) {
    r_set_framebuffer(renderer, renderer->screen_buffer);
    r_set_shader(renderer, renderer->forward_shader);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    r_set_mat4(sbox, renderer, "view", renderer->view);
    r_set_mat4(sbox, renderer, "projection", renderer->projection);
    r_set_vec3(sbox, renderer, "view_position", renderer->camera.position);

    entity_t* sun_entity = entlist_find_by_name(sbox, &sbox->map.entlist, "sun");
    if (!sun_entity || sun_entity->type != ENTITY_SUN_LIGHT) {
        error(sbox, "sun light entity 'sun' not found");
        return;
    }

    entity_sun_light_t* sun_light = &sun_entity->data.sun_light;
    r_set_vec3(sbox, renderer, "sun_light.direction", sun_light->direction);
    r_set_vec3(sbox, renderer, "sun_light.color", sun_light->color);

    for (int i = 0; i < renderer->ntranslucent_drawcalls; i++) {
        drawcall_t* drawcall = &renderer->translucent_drawcalls[i];
        r_set_mat4(sbox, renderer, "model", drawcall->model);

        for (int i = 0; i < MAX_MATERIALS; i++) {
            const material_t* material = drawcall->materials[i];
            if (!material) continue;
            r_set_material(sbox, renderer, material, i);
        }

        if (drawcall->mesh)
            r_draw_mesh(drawcall->mesh);
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    r_set_framebuffer(renderer, NULL);
}

static void render_skybox(sbox_t* sbox, renderer_t* renderer) {
    r_set_framebuffer(renderer, renderer->screen_buffer);
    r_set_shader(renderer, renderer->skybox_shader);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    mat4 projection;
    glm_mat4_copy(renderer->projection, projection);
    glm_mat4_inv(projection, projection);
    r_set_mat4(sbox, renderer, "inv_projection", projection);

    mat4 view;
    glm_mat4_copy(renderer->view, view);
    glm_mat4_inv(view, view);
    r_set_mat4(sbox, renderer, "inv_view", view);

    r_set_int(sbox, renderer, "cubemap", 0);
    r_set_int(sbox, renderer, "depth", 1);
    r_set_texture(renderer, sbox->map.skybox, 0);
    r_set_texture(renderer, renderer->gbuffer->textures[3], 1);

    r_draw_mesh(renderer->quad_mesh);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    r_set_framebuffer(renderer, NULL);
}

static void render_screen(sbox_t* sbox, renderer_t* renderer) {
    r_set_shader(renderer, renderer->screen_shader);

    r_set_int(sbox, renderer, "screen", 0);
    r_set_int(sbox, renderer, "depth", 1);
    r_set_int(sbox, renderer, "position", 2);
    r_set_int(sbox, renderer, "debug", 2);
    r_set_texture(renderer, renderer->screen_buffer->textures[0], 0);
    r_set_texture(renderer, renderer->gbuffer->textures[3], 1);
    r_set_texture(renderer, renderer->gbuffer->textures[0], 2);
    r_set_texture(renderer, renderer->sun_shadow_buffer->textures[0], 2);

    r_set_mat4(sbox, renderer, "view", renderer->view);
    r_set_mat4(sbox, renderer, "projection", renderer->projection);
    r_set_vec3(sbox, renderer, "view_position", renderer->camera.position);
    r_set_vec3(sbox, renderer, "view_direction", renderer->camera.forward);

    vec3 sun_direction = {0.0f, 0.0f, 0.0f};
    for (size_t i = 0; i < sbox->map.entlist.len; i++) {
        entity_t* entity = sbox->map.entlist.ents[i];
        if (entity->type != ENTITY_SUN_LIGHT) continue;
        glm_vec3_copy(entity->data.sun_light.direction, sun_direction);
    }
    r_set_vec3(sbox, renderer, "sun_direction", sun_direction);

    r_draw_mesh(renderer->quad_mesh);
}

void r_render(sbox_t* sbox, renderer_t* renderer) {
    render_shadows(sbox, renderer);
    render_gbuffer(sbox, renderer);
    render_ambient_light(sbox, renderer);
    render_sun_lights(sbox, renderer);
    render_point_lights(sbox, renderer);
    copy_depth(sbox, renderer);
    render_translucent(sbox, renderer);
    render_skybox(sbox, renderer);
    render_screen(sbox, renderer);

    line_render(sbox, renderer);
    ui_render(sbox, &renderer->ui, renderer);

    r_clear_drawcalls(renderer);
    SDL_GL_SwapWindow(sbox->window);
}
