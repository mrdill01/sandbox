#include "render.h"
#include "sbox.h"

#include "../include/gl.h"

void r_init(sbox_t* sbox, renderer_t* renderer) {
    info(sbox, "initializing renderer...");

    camera_init(sbox, &renderer->camera);

    renderer->ndrawcalls = 0;
    renderer->drawcalls = NULL;
    renderer->ntranslucent_drawcalls = 0;
    renderer->translucent_drawcalls = NULL;

    renderer->world_shader = shader_load(sbox,
        "res/shaders/world.vs", "res/shaders/world.fs");
    renderer->viewmodel_shader = shader_load(sbox,
        "res/shaders/viewmodel.vs", "res/shaders/viewmodel.fs"); 
    renderer->ambient_light_shader = shader_load(sbox,
        "res/shaders/ambient_light.vs", "res/shaders/ambient_light.fs"); 
    renderer->direct_light_shader = shader_load(sbox,
        "res/shaders/direct_light.vs", "res/shaders/direct_light.fs"); 
    renderer->skybox_shader = shader_load(sbox,
        "res/shaders/skybox.vs", "res/shaders/skybox.fs");  
    renderer->screen_shader = shader_load(sbox,
        "res/shaders/screen.vs", "res/shaders/screen.fs");
    renderer->ui_shader = shader_load(sbox,
        "res/shaders/ui.vs", "res/shaders/ui.fs");
    renderer->quad_mesh = mesh_load(sbox, "res/meshes/quad.obj");
    renderer->sphere_mesh = mesh_load(sbox, "res/meshes/sphere.obj");
    renderer->default_material = material_load(sbox,
        "res/textures/default.png",
        "res/textures/default_r.png",
        "res/textures/default_n.png",
        1, 1, false, PHYSMAT_METAL);

    renderer->gbuffer = NULL;
    renderer->screen_buffer = NULL;
    r_on_resize(sbox);

    glm_mat4_identity(renderer->projection);
    glm_mat4_identity(renderer->view);

    renderer->ui.font = texture_load(sbox, "res/textures/font.png");

    info(sbox, "renderer initialized!");
}

void r_free(sbox_t* sbox, renderer_t* renderer) {
    info(sbox, "shutting down renderer...");

    framebuffer_free(renderer->gbuffer);
    framebuffer_free(renderer->screen_buffer);

    int n = 0;
    shader_t* shader = sbox->shaders;
    while (shader) {
        shader_t* next = shader->next;
        shader_free(sbox, shader);
        shader = next;
        n++;
    }

    info(sbox, "released %d shaders", n);

    n = 0;
    mesh_t* mesh = sbox->meshes;
    while (mesh) {
        mesh_t* next = mesh->next;
        mesh_free(sbox, mesh);
        mesh = next;
        n++;
    }

    info(sbox, "released %d meshes", n);

    n = 0;
    texture_t* texture = sbox->textures;
    while (texture) {
        texture_t* next = texture->next;
        texture_free(sbox, texture);
        texture = next;
        n++;
    }

    info(sbox, "released %d textures", n);

    n = 0;
    material_t* material = sbox->materials;
    while (material) {
        material_t* next = material->next;
        material_free(sbox, material);
        material = next;
        n++;
    }

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
}

void r_on_resize(sbox_t* sbox) {
    glViewport(0, 0, r_width.value, r_height.value);
    renderer_t* renderer = &sbox->renderer;

    framebuffer_free(renderer->gbuffer);
    framebuffer_free(renderer->screen_buffer);

    int width = r_width.value * r_scale.value;
    int height = r_height.value * r_scale.value;
    
    renderer->gbuffer = framebuffer_new(sbox);
    framebuffer_add_texture(sbox, renderer->gbuffer, width, height);
    framebuffer_add_texture(sbox, renderer->gbuffer, width, height);
    framebuffer_add_texture(sbox, renderer->gbuffer, width, height);
    framebuffer_add_texture(sbox, renderer->gbuffer, width, height);
    framebuffer_add_depth_buffer(sbox, renderer->gbuffer, width, height);
    framebuffer_finish(sbox, renderer->gbuffer);

    renderer->screen_buffer = framebuffer_new(sbox);
    framebuffer_add_texture(sbox, renderer->screen_buffer, width, height);
    framebuffer_add_depth_buffer(sbox, renderer->screen_buffer, width, height);
    framebuffer_finish(sbox, renderer->screen_buffer);
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
}

void r_set_shader(shader_t* shader) {
    glUseProgram(shader->id);
}

void r_set_texture(texture_t* texture, int slot) {
    if (!texture) return;
    glActiveTexture(GL_TEXTURE0 + slot);

    int type = GL_TEXTURE_2D;
    if (texture->type == TEX_CUBE)
        type = GL_TEXTURE_CUBE_MAP;
    
    glBindTexture(type, texture->id);
}

void r_set_material(renderer_t* renderer, shader_t* shader, const material_t* material, int slot) {
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

void r_draw_mesh(const mesh_t* mesh) {
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->ntris, GL_UNSIGNED_INT, 0);
}
