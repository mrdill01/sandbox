#include "render.h"
#include "sbox.h"

#include "../include/gl.h"

void r_init(sbox_t* sbox, renderer_t* renderer) {
    info(sbox, "r_init()...");

    camera_init(sbox, &renderer->camera);

    renderer->ndrawcalls = 0;
    renderer->drawcalls = NULL;
    renderer->ntranslucent_drawcalls = 0;
    renderer->translucent_drawcalls = NULL;

    renderer->gbuffer_shader = shader_load(sbox,
        "gbuffer", "res/shaders/gbuffer.vs", "res/shaders/gbuffer.fs");
    renderer->viewmodel_shader = shader_load(sbox,
        "viewmodel", "res/shaders/viewmodel.vs", "res/shaders/viewmodel.fs"); 
    renderer->ambient_light_shader = shader_load(sbox,
        "ambient_light", "res/shaders/ambient_light.vs", "res/shaders/ambient_light.fs"); 
    renderer->sun_light_shader = shader_load(sbox,
        "sun_light", "res/shaders/sun_light.vs", "res/shaders/sun_light.fs");
    renderer->sun_shadow_shader = shader_load(sbox,
        "sun_shadow", "res/shaders/sun_shadow.vs", "res/shaders/sun_shadow.fs");
    renderer->point_light_shader = shader_load(sbox,
        "point_light", "res/shaders/point_light.vs", "res/shaders/point_light.fs"); 
    renderer->translucent_shader = shader_load(sbox,
        "translucent", "res/shaders/translucent.vs", "res/shaders/translucent.fs");
    renderer->skybox_shader = shader_load(sbox,
        "skybox", "res/shaders/skybox.vs", "res/shaders/skybox.fs");  
    renderer->screen_shader = shader_load(sbox,
        "screen", "res/shaders/screen.vs", "res/shaders/screen.fs");
    renderer->active_shader = NULL;
    
    renderer->quad_mesh = mesh_load(sbox, "res/meshes/quad.obj");
    renderer->sphere_mesh = mesh_load(sbox, "res/meshes/sphere.obj");
    renderer->default_material = material_load(sbox,
        "default",
        "res/textures/materials/default.png",
        "res/textures/materials/default_r.png",
        "res/textures/materials/default_n.png",
        1, 1, false, PHYSMAT_METAL);

    renderer->gbuffer = NULL;
    renderer->screen_buffer = NULL;
    renderer->sun_shadow_buffer = NULL;
    r_on_resize(sbox);

    glm_mat4_identity(renderer->projection);
    glm_mat4_identity(renderer->view);

    ui_init(sbox, &renderer->ui);
    info(sbox, "renderer initialized!");
}

void r_free(sbox_t* sbox, renderer_t* renderer) {
    info(sbox, "r_free()...");

    framebuffer_free(renderer->gbuffer);
    framebuffer_free(renderer->screen_buffer);
    framebuffer_free(renderer->sun_shadow_buffer);

    int n = 0;
    shader_t* shader = sbox->shaders;
    while (shader) {
        shader_t* next = shader->next;
        shader_free(sbox, shader);
        shader = next;
        n++;
    }
    sbox->shaders = NULL;

    info(sbox, "released %d shaders", n);

    n = 0;
    mesh_t* mesh = sbox->meshes;
    while (mesh) {
        mesh_t* next = mesh->next;
        mesh_free(sbox, mesh);
        mesh = next;
        n++;
    }
    sbox->meshes = NULL;

    info(sbox, "released %d meshes", n);

    n = 0;
    texture_t* texture = sbox->textures;
    while (texture) {
        texture_t* next = texture->next;
        texture_free(sbox, texture);
        texture = next;
        n++;
    }
    sbox->textures = NULL;

    info(sbox, "released %d textures", n);

    n = 0;
    material_t* material = sbox->materials;
    while (material) {
        material_t* next = material->next;
        material_free(sbox, material);
        material = next;
        n++;
    }
    sbox->materials = NULL;

    info(sbox, "released %d materials", n);

    info(sbox, "renderer shut down!");
}

static int comp_distance(const void* a_ptr, const void* b_ptr) {
	drawcall_t* a = (drawcall_t*)a_ptr;
	drawcall_t* b = (drawcall_t*)b_ptr;
	if (a->dist_to_camera == -1.0f) return -1;
	if (b->dist_to_camera == -1.0f) return 1;
	if (a->dist_to_camera < b->dist_to_camera) return 1;
	return -1;
}

void r_tick(sbox_t* sbox, renderer_t* renderer) {
    for (size_t i = 0; i < renderer->ndrawcalls; i++) {
        drawcall_t* drawcall = &renderer->drawcalls[i];

		vec3 center;
		bbox_get_center(&drawcall->mesh->bbox, center);

		vec3 tmp;
		glm_vec3_sub(center, sbox->renderer.camera.position, tmp);
		drawcall->dist_to_camera = glm_vec3_norm2(tmp);
	}

	qsort(renderer->drawcalls, renderer->ndrawcalls, sizeof(drawcall_t), comp_distance);

    for (int i = 0; i < renderer->ndrawcalls; i++) {
        drawcall_t* drawcall = &renderer->drawcalls[i];
        material_t* material = drawcall->materials[0];
        if (strcmp(material->name, "water") == 0) {
            material->scrollx += material->scroll_speed * sbox->time;
            material->scrolly += material->scroll_speed * sbox->time;
        }
    }

    for (int i = 0; i < renderer->ntranslucent_drawcalls; i++) {
        drawcall_t* drawcall = &renderer->translucent_drawcalls[i];
        material_t* material = drawcall->materials[0];
        if (strcmp(material->name, "water") == 0) {
            material->scrollx += material->scroll_speed * sbox->time;
            material->scrolly += material->scroll_speed * sbox->time;
        }
    }
}

void r_on_resize(sbox_t* sbox) {
    glViewport(0, 0, r_width.value, r_height.value);
    renderer_t* renderer = &sbox->renderer;

    framebuffer_free(renderer->gbuffer);
    framebuffer_free(renderer->screen_buffer);

    int width = r_width.value * r_scale.value;
    int height = r_height.value * r_scale.value;
    
    renderer->gbuffer = framebuffer_new(sbox);
    framebuffer_add_texture(sbox, renderer->gbuffer, width, height, TEX_FORMAT_RGBA_F16);
    framebuffer_add_texture(sbox, renderer->gbuffer, width, height, TEX_FORMAT_RGBA_F16);
    framebuffer_add_texture(sbox, renderer->gbuffer, width, height, TEX_FORMAT_RGBA_F16);
    framebuffer_add_texture(sbox, renderer->gbuffer, width, height, TEX_FORMAT_RGBA_F16);
    framebuffer_add_depth_buffer(sbox, renderer->gbuffer, width, height);
    framebuffer_finish(sbox, renderer->gbuffer);

    renderer->screen_buffer = framebuffer_new(sbox);
    framebuffer_add_texture(sbox, renderer->screen_buffer, width, height, TEX_FORMAT_RGBA_F16);
    framebuffer_add_depth_buffer(sbox, renderer->screen_buffer, width, height);
    framebuffer_finish(sbox, renderer->screen_buffer);

    renderer->sun_shadow_buffer = framebuffer_new(sbox);
    framebuffer_add_texture(sbox, renderer->sun_shadow_buffer, width, height, TEX_FORMAT_DEPTH);
    framebuffer_finish(sbox, renderer->sun_shadow_buffer);
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
    renderer->ndrawcalls = 0;
    free(renderer->drawcalls);
    renderer->drawcalls = NULL;

    renderer->ntranslucent_drawcalls = 0;
    free(renderer->translucent_drawcalls);
    renderer->translucent_drawcalls = NULL;
}

void r_set_shader(renderer_t* renderer, shader_t* shader) {
    glUseProgram((shader) ? shader->id : 0);
    renderer->active_shader = shader;
}

void r_set_texture(renderer_t* renderer, texture_t* texture, int slot) {
    if (!texture) return;
    glActiveTexture(GL_TEXTURE0 + slot);

    int type = GL_TEXTURE_2D;
    if (texture->type == TEX_CUBE)
        type = GL_TEXTURE_CUBE_MAP;
    
    glBindTexture(type, texture->id);
}

void r_set_material(sbox_t* sbox, renderer_t* renderer, const material_t* material, int slot) {
    if (!material) return;
    const int nmaterial_textures = 3;

    char slot_name[32];
    snprintf(slot_name, 32, "materials[%d].albedo", slot);
    r_set_int(sbox, renderer, slot_name, nmaterial_textures * slot + 0);
    r_set_texture(renderer,
        (material->albedo) ?
            material->albedo :
            renderer->default_material->albedo, nmaterial_textures * slot + 0);
    
    snprintf(slot_name, 32, "materials[%d].roughness", slot);
    r_set_int(sbox, renderer, slot_name, nmaterial_textures * slot + 1);
    r_set_texture(renderer,
        (material->roughness) ?
            material->roughness :
            renderer->default_material->roughness, nmaterial_textures * slot + 1);

    snprintf(slot_name, 32, "materials[%d].normal", slot);
    r_set_int(sbox, renderer, slot_name, nmaterial_textures * slot + 2);
    r_set_texture(renderer,
        (material->normal) ?
            material->normal :
            renderer->default_material->normal, nmaterial_textures * slot + 2);

    snprintf(slot_name, 32, "materials[%d].tilex", slot);
    r_set_float(sbox, renderer, slot_name, material->tilex);

    snprintf(slot_name, 32, "materials[%d].tiley", slot);
    r_set_float(sbox, renderer, slot_name, material->tiley);

    snprintf(slot_name, 32, "materials[%d].scrollx", slot);
    r_set_float(sbox, renderer, slot_name, material->scrollx);

    snprintf(slot_name, 32, "materials[%d].scrolly", slot);
    r_set_float(sbox, renderer, slot_name, material->scrolly);
}

void r_set_framebuffer(renderer_t* renderer, framebuffer_t* framebuffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, (framebuffer) ? framebuffer->id : 0);
}

static int get_uniform(sbox_t* sbox, renderer_t* renderer, const char* name) {
    GLint location = glGetUniformLocation(renderer->active_shader->id, name);
    if (location == -1) {
        info(sbox, "[%s] glGetUniformLocation returned -1 for %s",
            renderer->active_shader->name, name);
        return -1;
    }

    return location;
}

void r_set_int(sbox_t* sbox, renderer_t* renderer, const char* name, int i) {
    glUniform1i(get_uniform(sbox, renderer, name), i);
}

void r_set_float(sbox_t* sbox, renderer_t* renderer, const char* name, float f) {
    glUniform1f(get_uniform(sbox, renderer, name), f);
}

void r_set_vec2(sbox_t* sbox, renderer_t* renderer, const char* name, vec2 v) {
    glUniform2fv(get_uniform(sbox, renderer, name), 1, &v[0]);
}

void r_set_vec3(sbox_t* sbox, renderer_t* renderer, const char* name, vec3 v) {
    glUniform3fv(get_uniform(sbox, renderer, name), 1, &v[0]);
}

void r_set_vec4(sbox_t* sbox, renderer_t* renderer, const char* name, vec4 v) {
    glUniform4fv(get_uniform(sbox, renderer, name), 1, &v[0]);
}

void r_set_mat4(sbox_t* sbox, renderer_t* renderer, const char* name, mat4 m) {
    glUniformMatrix4fv(get_uniform(sbox, renderer, name), 1, GL_FALSE, &m[0][0]);
}

void r_draw_mesh(const mesh_t* mesh) {
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->ntris, GL_UNSIGNED_INT, 0);
}
