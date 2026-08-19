#include "render.h"
#include "../shared/quark.h"

#include "../../include/gl.h"

static void render_items(quark_t* quark, renderer_t* renderer) {
    prof_start(quark, &quark->prof);
    if (!quark->player) {
        prof_end(quark, &quark->prof);
        return;
    }

    r_set_shader(renderer, renderer->item_shader);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    inventory_t* inventory = &quark->player->inventory;
    for (int i = 0; i < INVENTORY_SLOTS; i++) {
        item_t* item = inventory->items[i];
        if (!item) continue;

        r_set_framebuffer(renderer, renderer->item_fbos[i]);
        glViewport(0, 0, ITEM_PREVIEW_RES, ITEM_PREVIEW_RES);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera_t camera;
        camera_init(quark, &camera);

        vec3 center;
        bbox_get_center(&item->mesh->bbox, center);
        glm_vec3_copy(center, camera.position);

        vec3 size;
        bbox_get_size(&item->mesh->bbox, size);

        camera.position[2] = -size[2] * 1.4f;
        camera.position[1] += 0.2f;

        mat4 view;
        mat4 projection;
        camera_get_view_matrix(&camera, view);
        camera_get_projection_matrix(&camera, ITEM_PREVIEW_RES, ITEM_PREVIEW_RES, projection);
            
        r_set_mat4(quark, renderer, "view", view);
        r_set_mat4(quark, renderer, "projection", projection);

        r_set_vec3(quark, renderer, "view_position", camera.position);

        mat4 model;
        glm_mat4_identity(model);

        quat yaw;
        glm_quat(yaw, rad(item->yaw), 0.0f, 1.0f, 0.0f);
        item->yaw += quark->dt * 90.0f;

        quat roll;
        glm_quat(roll, rad(180.0f), 0.0f, 0.0f, 1.0f);

        glm_quat_rotate(model, yaw, model);
        glm_quat_rotate(model, roll, model);

        r_set_mat4(quark, renderer, "model", model);

        for (int i = 0; i < MAX_MATERIALS; i++) {
            const material_t* material = item->materials[i];
            if (!material) continue;
            r_set_material(quark, renderer, material, i);
        }

        r_draw_mesh(renderer, item->mesh);

        r_set_framebuffer(renderer, NULL);
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    prof_end(quark, &quark->prof);
}

static void render_shadows(quark_t* quark, renderer_t* renderer) {
    prof_start(quark, &quark->prof);

    r_set_shader(renderer, renderer->sun_shadow_shader);
    r_set_framebuffer(renderer, renderer->sun_shadow_buffer);
    glViewport(0, 0, r_shadow_res.value, r_shadow_res.value);
    glClear(GL_DEPTH_BUFFER_BIT);

    if (!r_shadows.value) {
        r_set_framebuffer(renderer, NULL);
        prof_end(quark, &quark->prof);
        return;
    }
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    entity_t* sun_entity = entlist_find_by_name(quark, &quark->map.entlist, "sun");
    if (!sun_entity || sun_entity->type != ENTITY_SUN_LIGHT) {
        error(quark, "sun light entity 'sun' not found");
        return;
    }

    entity_sun_light_t* sun_light = &sun_entity->data.sun_light;

    float near = 1.0f;
    float far = 32.0f;
    bbox_t frustum = bbox_new((vec3){-far, -far, near}, (vec3){far, far, far});
    
    //frustum = bbox_translate(&frustum, renderer->camera.position);
    mat4 projection;
    glm_ortho(frustum.min[0], frustum.max[0],
        frustum.min[1], frustum.max[1],
        frustum.min[2], frustum.max[2],
        projection);

    vec3 center;
    bbox_get_center(&frustum, center);

    vec3 dir;
    glm_vec3_copy(sun_light->direction, dir);
    glm_vec3_scale(dir, -20.0f, dir);

    vec3 position;
    glm_vec3_copy(center, position);
    
    vec3 target;
    glm_vec3_copy(center, target);
    glm_vec3_add(target, dir, target);

    mat4 view;
    glm_lookat((vec3){5.0f, 20.0f, 18.0f}, (vec3){0.1f, 0.1f, 0.1f}, Y_AXIS, view);

    glm_mat4_identity(sun_light->matrix);
    glm_mat4_mul(sun_light->matrix, projection, sun_light->matrix);
    glm_mat4_mul(sun_light->matrix, view, sun_light->matrix);
    r_set_mat4(quark, renderer, "matrix", sun_light->matrix);

    for (int i = 0; i < renderer->ndrawcalls; i++) {
        drawcall_t* drawcall = &renderer->drawcalls[i];
        r_set_mat4(quark, renderer, "model", drawcall->model);
        if (drawcall->mesh)
            r_draw_mesh(renderer, drawcall->mesh);
    }

    for (int i = 0; i < renderer->ntranslucent_drawcalls; i++) {
        drawcall_t* drawcall = &renderer->translucent_drawcalls[i];
        if (!drawcall->materials[0]) continue;
        
        r_set_mat4(quark, renderer, "model", drawcall->model);
        r_set_texture(quark, renderer, "albedo", drawcall->materials[0]->albedo, 0);
        r_set_float(quark, renderer, "tilex", drawcall->materials[0]->tilex);
        r_set_float(quark, renderer, "tiley", drawcall->materials[0]->tiley);
        
        if (drawcall->mesh)
            r_draw_mesh(renderer, drawcall->mesh);
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    r_set_framebuffer(renderer, NULL);
    prof_end(quark, &quark->prof);
}

static void render_gbuffer(quark_t* quark, renderer_t* renderer) {
    prof_start(quark, &quark->prof);

    r_set_shader(renderer, renderer->gbuffer_shader);
    r_set_framebuffer(renderer, renderer->gbuffer);
    glViewport(0, 0, r_width.value * r_scale.value, r_height.value * r_scale.value);

    if (r_wireframe.value) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
        
    r_set_mat4(quark, renderer, "view", renderer->view);
    r_set_mat4(quark, renderer, "projection", renderer->projection);
    r_set_float(quark, renderer, "time", quark->time);
    r_set_vec3(quark, renderer, "camera.position", renderer->camera.position);
    r_set_float(quark, renderer, "camera.near", renderer->camera.near);
    r_set_float(quark, renderer, "camera.far", renderer->camera.far);

    for (int i = 0; i < renderer->ndrawcalls; i++) {
        drawcall_t* drawcall = &renderer->drawcalls[i];
        r_set_mat4(quark, renderer, "model", drawcall->model);

        for (int i = 0; i < MAX_MATERIALS; i++) {
            const material_t* material = drawcall->materials[i];
            if (!material) continue;
            r_set_material(quark, renderer, material, i);
        }

        r_set_float(quark, renderer, "hitbox_height", drawcall->local_bbox.max[1]);

        if (drawcall->mesh)
            r_draw_mesh(renderer, drawcall->mesh);

        if (r_debug_colliders.value)
            r_add_line_box(quark, renderer, &drawcall->world_bbox, COLOR_GREEN, 0.0f);
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    r_set_framebuffer(renderer, NULL);

    prof_end(quark, &quark->prof);
}

static void render_ssao(quark_t* quark, renderer_t* renderer) {
    if (!r_ssao.value) return;

    prof_start(quark, &quark->prof);

    r_set_framebuffer(renderer, renderer->ssao_framebuffer);
    r_set_shader(renderer, renderer->ssao_shader);
    glClear(GL_COLOR_BUFFER_BIT);

    r_set_texture(quark, renderer, "g_position", renderer->gbuffer->textures[0], 0);
    r_set_texture(quark, renderer, "g_normal", renderer->gbuffer->textures[2], 1);
    r_set_texture(quark, renderer, "noise", renderer->ssao_noise_texture, 2);
    r_set_mat4(quark, renderer, "view", renderer->view);
    r_set_mat4(quark, renderer, "projection", renderer->projection);
    r_set_vec2(quark, renderer, "screen_size",
        (vec2){r_width.value * r_scale.value, r_height.value * r_scale.value});

    for (int i = 0; i < SSAO_KERNEL_SIZE; i++) {
        char name[32];
        sprintf(name, "samples[%d]", i);
        r_set_vec3(quark, &quark->renderer, name, renderer->ssao_kernel[i]);
    }

    r_draw_mesh(renderer, renderer->quad_mesh);

    r_set_framebuffer(renderer, NULL);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    prof_end(quark, &quark->prof);
}

static void render_skybox(quark_t* quark, renderer_t* renderer) {
    prof_start(quark, &quark->prof);

    r_set_framebuffer(renderer, renderer->gbuffer);
    r_set_shader(renderer, renderer->skybox_shader);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    mat4 view;
    glm_mat4_copy(renderer->view, view);
    glm_mat4_inv(view, view);
    r_set_mat4(quark, renderer, "inv_view", view);

    mat4 projection;
    glm_mat4_copy(renderer->projection, projection);
    glm_mat4_inv(projection, projection);
    r_set_mat4(quark, renderer, "inv_projection", projection);

    r_set_texture(quark, renderer, "cubemap", quark->map.skybox, 0);

    r_draw_mesh(renderer, renderer->quad_mesh);

    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    r_set_framebuffer(renderer, NULL);

    prof_end(quark, &quark->prof);
}

static void render_ambient_light(quark_t* quark, renderer_t* renderer) {
    prof_start(quark, &quark->prof);

    r_set_framebuffer(renderer, renderer->screen_buffer);
    r_set_shader(renderer, renderer->ambient_light_shader);
    glViewport(0, 0, r_width.value * r_scale.value, r_height.value * r_scale.value);

    r_set_texture(quark, renderer, "gbuffer.position", renderer->gbuffer->textures[0], 0);
    r_set_texture(quark, renderer, "gbuffer.albedo_roughness", renderer->gbuffer->textures[1], 1);
    r_set_texture(quark, renderer, "gbuffer.normal", renderer->gbuffer->textures[2], 2);
    r_set_texture(quark, renderer, "gbuffer.depth", renderer->gbuffer->textures[3], 3);
    r_set_texture(quark, renderer, "ssao", (r_ssao.value) ?
        renderer->ssao_framebuffer->textures[0] : renderer->default_material->roughness, 4);

    r_draw_mesh(renderer, renderer->quad_mesh);
    r_set_framebuffer(renderer, NULL);

    prof_end(quark, &quark->prof);
}

static void render_sun_light(quark_t* quark, renderer_t* renderer) {
    prof_start(quark, &quark->prof);

    r_set_framebuffer(renderer, renderer->screen_buffer);
    r_set_shader(renderer, renderer->sun_light_shader);
    glViewport(0, 0, r_width.value * r_scale.value, r_height.value * r_scale.value);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    r_set_texture(quark, renderer, "gbuffer.position", renderer->gbuffer->textures[0], 0);
    r_set_texture(quark, renderer, "gbuffer.albedo_roughness", renderer->gbuffer->textures[1], 1);
    r_set_texture(quark, renderer, "gbuffer.normal", renderer->gbuffer->textures[2], 2);
    r_set_texture(quark, renderer, "gbuffer.depth", renderer->gbuffer->textures[3], 3);

    r_set_vec3(quark, renderer, "view_position", renderer->camera.position);

    for (size_t i = 0; i < quark->map.entlist.len; i++) {
        entity_t* entity = quark->map.entlist.ents[i];
        if (!entity || entity->type != ENTITY_SUN_LIGHT) continue;
        entity_sun_light_t* sun_light = &entity->data.sun_light;

        r_set_vec3(quark, renderer, "light.direction", sun_light->direction);
        r_set_vec3(quark, renderer, "light.color", sun_light->color);
        r_set_texture(quark, renderer, "light.shadow", renderer->sun_shadow_buffer->textures[0], 4);
        r_set_mat4(quark, renderer, "light.matrix",
            (r_shadows.value) ? sun_light->matrix : GLM_MAT4_IDENTITY);

        r_draw_mesh(renderer, renderer->quad_mesh);
    }

    /*glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);*/
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    r_set_framebuffer(renderer, NULL);

    prof_end(quark, &quark->prof);
}

static void render_point_lights(quark_t* quark, renderer_t* renderer) {
    prof_start(quark, &quark->prof);

    r_set_framebuffer(renderer, renderer->screen_buffer);
    r_set_shader(renderer, renderer->point_light_shader);
    glViewport(0, 0, r_width.value * r_scale.value, r_height.value * r_scale.value);

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);
    glEnable(GL_BLEND);
    /*glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);*/
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    r_set_texture(quark, renderer, "gbuffer.position", renderer->gbuffer->textures[0], 0);
    r_set_texture(quark, renderer, "gbuffer.albedo_roughness", renderer->gbuffer->textures[1], 1);
    r_set_texture(quark, renderer, "gbuffer.normal", renderer->gbuffer->textures[2], 2);
    r_set_texture(quark, renderer, "gbuffer.depth", renderer->gbuffer->textures[3], 3);

    r_set_vec3(quark, renderer, "view_position", renderer->camera.position);

    for (size_t i = 0; i < quark->map.entlist.len; i++) {
        entity_t* entity = quark->map.entlist.ents[i];
        if (!entity || entity->type != ENTITY_POINT_LIGHT) continue;
        entity_point_light_t* point_light = &entity->data.point_light;

        r_set_vec3(quark, renderer, "light.position", entity->position);
        r_set_vec3(quark, renderer, "light.color", point_light->color);

        float scale = 3.0f;
        
        mat4 model;
        glm_mat4_identity(model);
        glm_scale(model, (vec3){scale, scale, scale});
        glm_translate(model, entity->position);
        r_set_mat4(quark, renderer, "model", model);

        r_draw_mesh(renderer, renderer->quad_mesh);
    }

    /*glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);*/
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    r_set_framebuffer(renderer, NULL);

    prof_end(quark, &quark->prof);
}

static void copy_depth(quark_t* quark, renderer_t* renderer) {
    prof_start(quark, &quark->prof);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer->gbuffer->id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderer->screen_buffer->id);
    glBlitFramebuffer(
        0, 0, r_width.value * r_scale.value, r_height.value * r_scale.value,
        0, 0, r_width.value * r_scale.value, r_height.value * r_scale.value,
        GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    prof_end(quark, &quark->prof);
}

static void render_forward(quark_t* quark, renderer_t* renderer) {
    prof_start(quark, &quark->prof);

    r_set_framebuffer(renderer, renderer->screen_buffer);
    r_set_shader(renderer, renderer->forward_shader);
    glViewport(0, 0, r_width.value * r_scale.value, r_height.value * r_scale.value);

    if (r_wireframe.value) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    r_set_mat4(quark, renderer, "view", renderer->view);
    r_set_mat4(quark, renderer, "projection", renderer->projection);
    r_set_vec3(quark, renderer, "camera.position", renderer->camera.position);
    r_set_float(quark, renderer, "camera.near", renderer->camera.near);
    r_set_float(quark, renderer, "camera.far", renderer->camera.far);
    r_set_vec2(quark, renderer, "screen_size",
        (vec2){r_width.value * r_scale.value, r_height.value * r_scale.value});
    r_set_texture(quark, renderer, "g_depth", renderer->gbuffer->textures[3], 5);

    entity_t* sun_entity = entlist_find_by_name(quark, &quark->map.entlist, "sun");
    if (!sun_entity || sun_entity->type != ENTITY_SUN_LIGHT) {
        error(quark, "sun light entity 'sun' not found");
        return;
    }

    entity_sun_light_t* sun_light = &sun_entity->data.sun_light;
    r_set_vec3(quark, renderer, "sun_light.direction", sun_light->direction);
    r_set_vec3(quark, renderer, "sun_light.color", sun_light->color);
    r_set_texture(quark, renderer, "sun_light.shadow", renderer->sun_shadow_buffer->textures[0], 6);
    r_set_mat4(quark, renderer, "sun_light.matrix", sun_light->matrix);

    r_set_float(quark, renderer, "time", quark->time);

    for (int i = 0; i < renderer->ntranslucent_drawcalls; i++) {
        drawcall_t* drawcall = &renderer->translucent_drawcalls[i];
        r_set_mat4(quark, renderer, "model", drawcall->model);

        for (int i = 0; i < MAX_MATERIALS; i++) {
            const material_t* material = drawcall->materials[i];
            if (!material) continue;
            r_set_float(quark, renderer, "wind_factor", material->wind_factor);
            r_set_float(quark, renderer, "hitbox_height", drawcall->local_bbox.max[1]);
            r_set_int(quark, renderer, "is_water", strcmp(material->name, "water") == 0);
            r_set_material(quark, renderer, material, i);
        }

        if (drawcall->mesh)
            r_draw_mesh(renderer, drawcall->mesh);
    }

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    r_set_framebuffer(renderer, NULL);

    prof_end(quark, &quark->prof);
}

static void render_screen(quark_t* quark, renderer_t* renderer) {
    prof_start(quark, &quark->prof);

    r_set_shader(renderer, renderer->screen_shader);
    glViewport(0, 0, r_width.value, r_height.value);

    r_set_texture(quark, renderer, "screen", renderer->screen_buffer->textures[0], 0);
    r_set_texture(quark, renderer, "g_position", renderer->gbuffer->textures[0], 1);
    r_set_texture(quark, renderer, "g_albedo_roughness", renderer->gbuffer->textures[1], 2);
    r_set_texture(quark, renderer, "g_normal", renderer->gbuffer->textures[2], 3);
    r_set_texture(quark, renderer, "g_depth", renderer->gbuffer->textures[3], 4);
    r_set_texture(quark, renderer, "ssao", renderer->ssao_framebuffer->textures[0], 5);
    r_set_texture(quark, renderer, "sun_shadow", renderer->sun_shadow_buffer->textures[0], 6);
    r_set_int(quark, renderer, "debug_buffer", (int)r_debug_buffer.value);

    r_set_mat4(quark, renderer, "view", renderer->view);
    r_set_mat4(quark, renderer, "projection", renderer->projection);
    r_set_vec3(quark, renderer, "view_position", renderer->camera.position);
    r_set_vec3(quark, renderer, "view_direction", renderer->camera.forward);
    r_set_int(quark, renderer, "head_in_water",
        (quark->player) ? quark->player->head_in_water : 0);

    r_draw_mesh(renderer, renderer->quad_mesh);
    prof_end(quark, &quark->prof);
}

void r_render(quark_t* quark, renderer_t* renderer) {
    camera_get_projection_matrix(&renderer->camera,
        r_width.value, r_height.value, renderer->projection);
    camera_get_view_matrix(&renderer->camera, renderer->view);

    if (!quark->map.is_loaded) {
        ui_render(quark, &renderer->ui, renderer);
        r_reset_stats(quark, renderer);
        SDL_GL_SwapWindow(quark->window);
        return;
    }

    for (int i = 0; i < NET_MAX_PLAYERS; i++) {
        if (!quark->players[i]) continue;
        player_render(quark, quark->players[i], renderer);
    }

    render_items(quark, renderer);
    render_shadows(quark, renderer);
    render_gbuffer(quark, renderer);
    render_ssao(quark, renderer);
    render_skybox(quark, renderer);
    render_ambient_light(quark, renderer);
    render_sun_light(quark, renderer);
    render_point_lights(quark, renderer);
    copy_depth(quark, renderer);
    render_forward(quark, renderer);
    r_render_particles(quark, renderer);
    render_screen(quark, renderer);
    r_render_lines(quark, renderer);
    ui_render(quark, &renderer->ui, renderer);

    r_clear_drawcalls(renderer);
    r_reset_stats(quark, renderer);
    SDL_GL_SwapWindow(quark->window);
}
