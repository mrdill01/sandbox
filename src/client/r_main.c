#include "render.h"
#include "../shared/quark.h"
#include "item.h"

#include "../../include/gl.h"

void r_init(quark_t* quark, renderer_t* renderer) {
    info(quark, "r_init()...");

    renderer->init = false;
    camera_init(quark, &renderer->camera);

    renderer->ndrawcalls = 0;
    renderer->drawcalls = NULL;
    renderer->ntranslucent_drawcalls = 0;
    renderer->translucent_drawcalls = NULL;

    renderer->gbuffer_shader = shader_load(quark,
        "gbuffer", "res/shaders/gbuffer.vs", "res/shaders/gbuffer.fs");
    renderer->ssao_shader = shader_load(quark,
        "ssao", "res/shaders/ssao.vs", "res/shaders/ssao.fs"); 
    renderer->ambient_light_shader = shader_load(quark,
        "ambient_light", "res/shaders/ambient_light.vs", "res/shaders/ambient_light.fs"); 
    renderer->sun_light_shader = shader_load(quark,
        "sun_light", "res/shaders/sun_light.vs", "res/shaders/sun_light.fs");
    renderer->sun_shadow_shader = shader_load(quark,
        "sun_shadow", "res/shaders/sun_shadow.vs", "res/shaders/sun_shadow.fs");
    renderer->point_light_shader = shader_load(quark,
        "point_light", "res/shaders/point_light.vs", "res/shaders/point_light.fs"); 
    renderer->forward_shader = shader_load(quark,
        "forward", "res/shaders/forward.vs", "res/shaders/forward.fs");
    renderer->skybox_shader = shader_load(quark,
        "skybox", "res/shaders/skybox.vs", "res/shaders/skybox.fs"); 
    renderer->partfx_shader = shader_load(quark,
        "partfx", "res/shaders/partfx.vs", "res/shaders/partfx.fs"); 
    renderer->screen_shader = shader_load(quark,
        "screen", "res/shaders/screen.vs", "res/shaders/screen.fs");
    renderer->line_shader = shader_load(quark,
        "line", "res/shaders/line.vs", "res/shaders/line.fs");
    renderer->item_shader = shader_load(quark,
        "item", "res/shaders/item.vs", "res/shaders/item.fs");
    renderer->active_shader = NULL;
    
    renderer->quad_mesh = mesh_load(quark, "res/meshes/quad.obj");
    renderer->sphere_mesh = mesh_load(quark, "res/meshes/sphere.obj");

    for (int i = 0; i < MAX_TEXTURES; i++)
        renderer->bound_textures[i] = NULL;
    
    renderer->default_material = material_load(quark,
        "default",
        "res/textures/materials/default.png",
        "res/textures/materials/default_r.png",
        "res/textures/materials/default_n.png",
        1, 1, false, PHYS_MAT_METAL);

    renderer->gbuffer = NULL;
    renderer->ssao_framebuffer = NULL;
    renderer->screen_buffer = NULL;
    renderer->sun_shadow_buffer = NULL;
    for (int i = 0; i < INVENTORY_SLOTS; i++) {
        renderer->item_fbos[i] = framebuffer_new(quark);
        framebuffer_add_texture(quark,
            renderer->item_fbos[i], ITEM_PREVIEW_RES, ITEM_PREVIEW_RES, TEX_FORMAT_RGBA);
        framebuffer_finish(quark, renderer->item_fbos[i]);
    }
    r_on_resize(quark);

    glm_mat4_identity(renderer->projection);
    glm_mat4_identity(renderer->view);

    ui_init(quark, &renderer->ui);

    for (int i = 0; i < MAX_LINES; i++) {
        line_t* line = &renderer->lines[i];
        line->is_free = true;
        line->mesh = NULL;
        glm_vec3_zero(line->start);
        glm_vec3_zero(line->end);
        glm_vec4_zero(line->color);
        line->spawn_time = 0.0f;
        line->decay_time = 0.0f;
    }

    renderer->p_fire = texture_load(quark, "res/textures/particles/p_fire.png", TEX_FILTER_NEAREST);
    renderer->p_cross = texture_load(quark, "res/textures/particles/p_cross.png", TEX_FILTER_NEAREST);
    renderer->p_corona =
        texture_load(quark, "res/textures/particles/p_corona.png", TEX_FILTER_NEAREST);
    renderer->p_smoke = texture_load(quark, "res/textures/particles/p_smoke.png", TEX_FILTER_NEAREST);
    renderer->p_steam[0] =
        texture_load(quark, "res/textures/particles/p_steam.png", TEX_FILTER_NEAREST);
    renderer->p_steam[1] =
        texture_load(quark, "res/textures/particles/p_steam2.png", TEX_FILTER_NEAREST);
    renderer->p_steam[2] =
        texture_load(quark, "res/textures/particles/p_steam3.png", TEX_FILTER_NEAREST);
    renderer->p_steam[3] =
        texture_load(quark, "res/textures/particles/p_steam4.png", TEX_FILTER_NEAREST);
    renderer->p_bullet_hole = texture_load(quark,
        "res/textures/particles/p_bullet_hole.png", TEX_FILTER_NEAREST);
    renderer->p_water = texture_load(quark,
        "res/textures/particles/p_water.png", TEX_FILTER_NEAREST);
    renderer->p_blood = texture_load(quark,
        "res/textures/particles/p_blood.png", TEX_FILTER_NEAREST);
    renderer->p_coin = texture_load(quark,
        "res/textures/particles/p_coin.png", TEX_FILTER_NEAREST);

    for (int i = 0; i < MAX_PARTICLES; i++) {
        particle_t* particle = &renderer->particles[i];
        particle->is_free = true;
    }

    for (int i = 0; i < SSAO_KERNEL_SIZE; i++) {
        vec3 sample = {
            random(0.0f, 1.0f) * 2.0f - 1.0f,
            random(0.0f, 1.0f) * 2.0f - 1.0f,
            random(0.0f, 1.0f),
        };

        glm_vec3_normalize(sample);
        glm_vec3_scale(sample, random(0.0f, 1.0f), sample);

        float scale = (float)i / SSAO_KERNEL_SIZE; 
        scale = lerp(0.1f, 1.0f, scale * scale);
        glm_vec3_scale(sample, scale, sample);

        glm_vec3_copy(sample, renderer->ssao_kernel[i]);
    }

    vec3 ssao_noise[16];
    for (int i = 0; i < 16; i++) {
        vec3 noise = {
            random(0.0f, 1.0f) * 2.0f - 1.0f,
            random(0.0f, 1.0f) * 2.0f - 1.0f,
            0.0f
        };
        glm_vec3_copy(noise, ssao_noise[i]);
    }

    renderer->ssao_noise_texture = texture_new(quark, 4, 4, NULL,
        TEX_FORMAT_RGBA_F16, TEX_FILTER_NEAREST);
    r_set_texture(quark, &quark->renderer, NULL, renderer->ssao_noise_texture, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssao_noise[0]);

    r_reset_stats(quark, renderer);
    renderer->init = true;
    info(quark, "renderer initialized!");
}

void r_free(quark_t* quark, renderer_t* renderer) {
    info(quark, "r_free()...");

    framebuffer_free(renderer->gbuffer);
    framebuffer_free(renderer->screen_buffer);
    framebuffer_free(renderer->sun_shadow_buffer);

    int n = 0;
    shader_t* shader = quark->shaders;
    while (shader) {
        shader_t* next = shader->next;
        shader_free(quark, shader);
        shader = next;
        n++;
    }
    quark->shaders = NULL;

    info(quark, "released %d shaders", n);

    n = 0;
    mesh_t* mesh = quark->meshes;
    while (mesh) {
        mesh_t* next = mesh->next;
        mesh_free(quark, mesh);
        mesh = next;
        n++;
    }
    quark->meshes = NULL;

    info(quark, "released %d meshes", n);

    n = 0;
    texture_t* texture = quark->textures;
    while (texture) {
        texture_t* next = texture->next;
        texture_free(quark, texture);
        texture = next;
        n++;
    }
    quark->textures = NULL;

    info(quark, "released %d textures", n);

    n = 0;
    material_t* material = quark->materials;
    while (material) {
        material_t* next = material->next;
        material_free(quark, material);
        material = next;
        n++;
    }
    quark->materials = NULL;

    info(quark, "released %d materials", n);

    info(quark, "renderer shut down!");
}

static int sort_opaque(const void* a_ptr, const void* b_ptr) {
	drawcall_t* a = (drawcall_t*)a_ptr;
	drawcall_t* b = (drawcall_t*)b_ptr;
    if (!a || !b) return 0;
	if (a->distance_to_camera > b->distance_to_camera) return 1;
	return -1;
}

static int sort_translucent(const void* a_ptr, const void* b_ptr) {
	drawcall_t* a = (drawcall_t*)a_ptr;
	drawcall_t* b = (drawcall_t*)b_ptr;
    if (!a || !b) return 0;
	if (a->distance_to_camera < b->distance_to_camera) return 1;
	return -1;
}

void r_tick(quark_t* quark, renderer_t* renderer) {
    SDL_GL_SetSwapInterval(r_vsync.value);

    if (renderer->nfps_samples < FPS_SAMPLE_RATE) {
        renderer->fps_samples[renderer->nfps_samples++] = 1.0f / quark->dt;
    } else {
        renderer->fps = 0.0f;
        for (int i = 0; i < FPS_SAMPLE_RATE; i++) {
            renderer->fps += renderer->fps_samples[i];
        }
        renderer->fps /= FPS_SAMPLE_RATE;
        renderer->nfps_samples = 0;
    }

    r_tick_particles(quark, renderer);

    for (size_t i = 0; i < renderer->ndrawcalls; i++) {
        drawcall_t* drawcall = &renderer->drawcalls[i];

		vec3 center;
		bbox_get_center(&drawcall->world_bbox, center);

		drawcall->distance_to_camera = glm_vec3_distance(
            center, quark->renderer.camera.position);
	}

	qsort(renderer->drawcalls, renderer->ndrawcalls, sizeof(drawcall_t), sort_opaque);

    for (size_t i = 0; i < renderer->ntranslucent_drawcalls; i++) {
        drawcall_t* drawcall = &renderer->translucent_drawcalls[i];

		vec3 center;
		bbox_get_center(&drawcall->world_bbox, center);

		drawcall->distance_to_camera = glm_vec3_distance(
            center, quark->renderer.camera.position);
	}

	qsort(renderer->translucent_drawcalls, renderer->ntranslucent_drawcalls,
        sizeof(drawcall_t), sort_translucent);

    for (int i = 0; i < renderer->ndrawcalls; i++) {
        drawcall_t* drawcall = &renderer->drawcalls[i];
        for (int i = 0; i < MAX_MATERIALS; i++) {
            material_t* material = drawcall->materials[i];
            if (!material) continue;
            if (strcmp(material->name, "water") == 0) {
                material->scrollx += material->scroll_speed * quark->dt;
                material->scrolly += material->scroll_speed * quark->dt;
            }
        }
    }

    for (int i = 0; i < renderer->ntranslucent_drawcalls; i++) {
        drawcall_t* drawcall = &renderer->translucent_drawcalls[i];
        for (int i = 0; i < MAX_MATERIALS; i++) {
            material_t* material = drawcall->materials[i];
            if (!material) continue;
            if (strcmp(material->name, "water") == 0) {
                material->scrollx += material->scroll_speed * quark->dt;
                material->scrolly += material->scroll_speed * quark->dt;
            }
        }
    }
}

void r_on_resize(quark_t* quark) {
    glViewport(0, 0, r_width.value, r_height.value);
    renderer_t* renderer = &quark->renderer;

    framebuffer_free(renderer->gbuffer);
    framebuffer_free(renderer->screen_buffer);
    framebuffer_free(renderer->ssao_framebuffer);
    framebuffer_free(renderer->sun_shadow_buffer);

    int width = r_width.value * r_scale.value;
    int height = r_height.value * r_scale.value;
    
    renderer->gbuffer = framebuffer_new(quark);
    framebuffer_add_texture(quark, renderer->gbuffer, width, height, TEX_FORMAT_RGBA_F16);
    framebuffer_add_texture(quark, renderer->gbuffer, width, height, TEX_FORMAT_RGBA_F16);
    framebuffer_add_texture(quark, renderer->gbuffer, width, height, TEX_FORMAT_RGBA);
    framebuffer_add_texture(quark, renderer->gbuffer, width, height, TEX_FORMAT_RGBA_F16);
    framebuffer_add_depth_buffer(quark, renderer->gbuffer, width, height);
    framebuffer_finish(quark, renderer->gbuffer);

    renderer->screen_buffer = framebuffer_new(quark);
    framebuffer_add_texture(quark, renderer->screen_buffer, width, height, TEX_FORMAT_RGBA_F16);
    framebuffer_add_depth_buffer(quark, renderer->screen_buffer, width, height);
    framebuffer_finish(quark, renderer->screen_buffer);

    renderer->ssao_framebuffer = framebuffer_new(quark);
    framebuffer_add_texture(quark, renderer->ssao_framebuffer, width, height, TEX_FORMAT_RGBA_F16);
    framebuffer_add_depth_buffer(quark, renderer->ssao_framebuffer, width, height);
    framebuffer_finish(quark, renderer->ssao_framebuffer);

    renderer->sun_shadow_buffer = framebuffer_new(quark);
    framebuffer_add_texture(quark, renderer->sun_shadow_buffer,
        r_shadow_res.value, r_shadow_res.value, TEX_FORMAT_DEPTH);
    r_set_texture(quark, &quark->renderer, NULL, renderer->sun_shadow_buffer->textures[0], 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    r_set_texture(quark, &quark->renderer, NULL, NULL, 0);
    framebuffer_finish(quark, renderer->sun_shadow_buffer);
}

void r_on_toggle_fullscreen(quark_t* quark) {
    SDL_SetWindowFullscreen(quark->window, (r_fullscreen.value) ? SDL_WINDOW_FULLSCREEN : 0);
}

void r_add_drawcall(renderer_t* renderer, drawcall_t drawcall) {
    if (drawcall.is_translucent) {
        renderer->translucent_drawcalls = realloc(renderer->translucent_drawcalls,
            sizeof(drawcall_t) * (renderer->ntranslucent_drawcalls + 1));
        renderer->translucent_drawcalls[renderer->ntranslucent_drawcalls++] = drawcall;
        return;
    }

    renderer->drawcalls = realloc(renderer->drawcalls,
        sizeof(drawcall_t) * (renderer->ndrawcalls + 1));
    renderer->drawcalls[renderer->ndrawcalls++] = drawcall;
}

void r_clear_drawcalls(renderer_t* renderer) {
    for (int i = 0; i < renderer->ndrawcalls; i++)
        free(renderer->drawcalls[i].entity);
    renderer->ndrawcalls = 0;
    free(renderer->drawcalls);
    renderer->drawcalls = NULL;

    for (int i = 0; i < renderer->ntranslucent_drawcalls; i++)
        free(renderer->translucent_drawcalls[i].entity);
    renderer->ntranslucent_drawcalls = 0;
    free(renderer->translucent_drawcalls);
    renderer->translucent_drawcalls = NULL;
}

void r_set_shader(renderer_t* renderer, shader_t* shader) {
    glUseProgram((shader) ? shader->id : 0);
    renderer->active_shader = shader;
}

void r_set_texture(
    quark_t* quark, renderer_t* renderer, const char* name, texture_t* texture, int slot)
{
    if (!texture) return;

    if (slot > MAX_TEXTURES) {
        error(quark, "max texture limit reached (%d)", MAX_TEXTURES);
        return;
    }

    if (name)
        r_set_int(quark, renderer, name, slot);

    if (renderer->bound_textures[slot] && renderer->bound_textures[slot]->id == texture->id)
        return;
    
    renderer->bound_textures[slot] = texture;
    renderer->stats.textures++;
    glActiveTexture(GL_TEXTURE0 + slot);

    int type = GL_TEXTURE_2D;
    if (texture->type == TEX_CUBE)
        type = GL_TEXTURE_CUBE_MAP;
    
    glBindTexture(type, texture->id);
}

void r_set_material(quark_t* quark, renderer_t* renderer, const material_t* material, int slot) {
    if (!material) {
        r_set_material(quark, renderer, renderer->default_material, slot);
        return;
    }

    renderer->stats.materials++;
    const int nmaterial_textures = 3;

    char slot_name[32];
    snprintf(slot_name, 32, "materials[%d].albedo", slot);
    r_set_texture(quark, renderer,
        slot_name,
        (material->albedo) ?
            material->albedo :
            renderer->default_material->albedo,
        nmaterial_textures * slot + 0);
    
    snprintf(slot_name, 32, "materials[%d].roughness", slot);
    r_set_texture(quark, renderer,
        slot_name,
        (material->roughness) ?
            material->roughness :
            renderer->default_material->roughness,
        nmaterial_textures * slot + 1);

    snprintf(slot_name, 32, "materials[%d].normal", slot);
    r_set_texture(quark, renderer,
        slot_name,
        (material->normal) ?
            material->normal :
            renderer->default_material->normal,
        nmaterial_textures * slot + 2);

    snprintf(slot_name, 32, "materials[%d].wind_factor", slot);
    r_set_float(quark, renderer, slot_name, material->wind_factor);

    snprintf(slot_name, 32, "materials[%d].tilex", slot);
    r_set_float(quark, renderer, slot_name, material->tilex);

    snprintf(slot_name, 32, "materials[%d].tiley", slot);
    r_set_float(quark, renderer, slot_name, material->tiley);

    snprintf(slot_name, 32, "materials[%d].scrollx", slot);
    r_set_float(quark, renderer, slot_name, material->scrollx);

    snprintf(slot_name, 32, "materials[%d].scrolly", slot);
    r_set_float(quark, renderer, slot_name, material->scrolly);
}

void r_set_framebuffer(renderer_t* renderer, framebuffer_t* framebuffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, (framebuffer) ? framebuffer->id : 0);
}

static int get_uniform(quark_t* quark, renderer_t* renderer, const char* name) {
    GLint location = glGetUniformLocation(renderer->active_shader->id, name);
    if (location == -1) {
        //info(quark, "[%s] glGetUniformLocation returned -1 for %s",
        //    renderer->active_shader->name, name);
        return -1;
    }

    return location;
}

void r_set_int(quark_t* quark, renderer_t* renderer, const char* name, int i) {
    glUniform1i(get_uniform(quark, renderer, name), i);
}

void r_set_float(quark_t* quark, renderer_t* renderer, const char* name, float f) {
    glUniform1f(get_uniform(quark, renderer, name), f);
}

void r_set_vec2(quark_t* quark, renderer_t* renderer, const char* name, vec2 v) {
    glUniform2fv(get_uniform(quark, renderer, name), 1, &v[0]);
}

void r_set_vec3(quark_t* quark, renderer_t* renderer, const char* name, vec3 v) {
    glUniform3fv(get_uniform(quark, renderer, name), 1, &v[0]);
}

void r_set_vec4(quark_t* quark, renderer_t* renderer, const char* name, vec4 v) {
    glUniform4fv(get_uniform(quark, renderer, name), 1, &v[0]);
}

void r_set_mat4(quark_t* quark, renderer_t* renderer, const char* name, mat4 m) {
    glUniformMatrix4fv(get_uniform(quark, renderer, name), 1, GL_FALSE, &m[0][0]);
}

void r_draw_mesh(renderer_t* renderer, const mesh_t* mesh) {
    renderer->stats.drawcalls++;

    for (size_t i = 0; i < mesh->nbuffers; i++) {
        mesh_buffer_t* buffer = mesh->buffers[i];
        if (!buffer) continue;
        glBindVertexArray(buffer->vao);
        glDrawElements(GL_TRIANGLES, buffer->nindices, GL_UNSIGNED_INT, 0);
        renderer->stats.meshes++;
        renderer->stats.tris += buffer->nindices / 3;
    }
}

void r_reset_stats(quark_t* quark, renderer_t* renderer) {
    renderer->stats.drawcalls = 0;
    renderer->stats.meshes = 0;
    renderer->stats.tris = 0;
    renderer->stats.textures = 0;
    renderer->stats.materials = 0;
}
