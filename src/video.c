#include "video.h"
#include "sbox.h"

#include <stdlib.h>
#include <stdio.h>

#include "../include/gl.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "../include/tinyobj_loader_c.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

#define PITCH_LIMIT 89.99

void camera_init(camera_t* camera) {
    glm_vec3_copy((vec3)GLM_VEC3_ZERO_INIT, camera->position);
    vec3 angles = {0.0f, 90.0f, 0.0f};
    glm_vec3_copy(angles, camera->angles);
    glm_vec3_copy(X_AXIS, camera->right);
    glm_vec3_copy(Y_AXIS, camera->up);
    glm_vec3_copy(Z_AXIS, camera->forward);
    camera->fov = 75.0f;
    camera->near = 0.001f;
    camera->far = 100.0f;
}

void camera_tick(sbox_t* sbox, camera_t* camera) {
    camera->forward[0] = cos(rad(camera->angles[1])) * cos(rad(camera->angles[0]));
    camera->forward[1] = sin(rad(camera->angles[0]));
    camera->forward[2] = sin(rad(camera->angles[1])) * cos(rad(camera->angles[0]));

    glm_cross(Y_AXIS, camera->forward, camera->right);
    glm_normalize(camera->right);

    glm_cross(camera->forward, camera->right, camera->up);
}

void camera_add_pitch(camera_t* camera, float pitch) {
    camera->angles[0] += pitch;
    camera->angles[0] = clip(camera->angles[0], -PITCH_LIMIT, PITCH_LIMIT);        
}

void camera_add_yaw(camera_t* camera, float yaw) {
    camera->angles[1] += yaw;
}

void camera_add_roll(camera_t* camera, float roll) {
    camera->angles[2] += roll;
}

void camera_get_projection_matrix(camera_t* camera, int width, int height, mat4 proj) {
    glm_perspective(rad(camera->fov), (float)width / height, camera->near, camera->far, proj);
}

void camera_get_view_matrix(camera_t* camera, mat4 view) {
    vec3 center;
    glm_vec3_add(camera->position, camera->forward, center);
    glm_lookat(camera->position, center, Y_AXIS, view);
}

static int64_t compile_shader(sbox_t* sbox, const char* src, const char* name, int type) {
    info(sbox, "compiling shader '%s'", name);

    uint32_t id = glCreateShader(type);
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);

    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char buffer[512];
        glGetShaderInfoLog(id, 512, NULL, buffer);
        error(sbox, "failed to compile shader '%s': %s", name, buffer);
        return -1;
    }

    return id;
}

shader_t* shader_new(sbox_t* sbox, const char* vs, const char* vname, const char* fs, const char* fname) {
    uint32_t vertex_shader = compile_shader(sbox, vs, vname, GL_VERTEX_SHADER);
    uint32_t fragment_shader = compile_shader(sbox, fs, fname, GL_FRAGMENT_SHADER);

    uint32_t id = glCreateProgram();
    glAttachShader(id, vertex_shader);
    glAttachShader(id, fragment_shader);
    glLinkProgram(id);

    int success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        char buffer[512];
        glGetProgramInfoLog(id, 512, NULL, buffer);
        error(sbox, "failed to link shader: %s", buffer);
        return NULL;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    shader_t* shader = malloc(sizeof(shader_t));
    shader->id = id;

    shader->next = sbox->shaders;
    sbox->shaders = shader;
    return shader;
}

shader_t* shader_load(sbox_t* sbox, const char* vpath, const char* fpath) {
    info(sbox, "loading shaders '%s', '%s'", vpath, fpath);
    const char* vs = load_file(sbox, vpath);
    const char* fs = load_file(sbox, fpath);
    return shader_new(sbox, vs, vpath, fs, fpath);
}

void shader_set_int(shader_t* shader, const char* name, int i) {
    glUniform1i(glGetUniformLocation(shader->id, name), i);
}

void shader_set_float(shader_t* shader, const char* name, float f) {
    glUniform1f(glGetUniformLocation(shader->id, name), f);
}

void shader_set_vec2(shader_t* shader, const char* name, vec2 v) {
    glUniform2fv(glGetUniformLocation(shader->id, name), 1, &v[0]);
}

void shader_set_vec3(shader_t* shader, const char* name, vec3 v) {
    glUniform3fv(glGetUniformLocation(shader->id, name), 1, &v[0]);
}

void shader_set_vec4(shader_t* shader, const char* name, vec4 v) {
    glUniform4fv(glGetUniformLocation(shader->id, name), 1, &v[0]);
}

void shader_set_mat4(shader_t* shader, const char* name, mat4 m) {
    glUniformMatrix4fv(glGetUniformLocation(shader->id, name), 1, GL_FALSE, &m[0][0]);
}

void shader_free(sbox_t* sbox, shader_t* shader) {
    glDeleteProgram(shader->id);
    free(shader);
}

mesh_t* mesh_new(sbox_t* sbox,
    float* vertices, size_t nvertices,
    uint32_t* indices, size_t nindices,
    bbox_t bbox)
{
    uint32_t vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * nvertices, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * nindices, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    mesh_t* mesh = malloc(sizeof(mesh_t));
    mesh->vao = vao;
    mesh->vbo = vbo;
    mesh->ebo = ebo;
    mesh->ntris = nindices;
    mesh->bbox = bbox;

    mesh->next = sbox->meshes;
    sbox->meshes = mesh;
    return mesh;
}

static void file_callback(void *ctx,
    const char *filename,
    int is_mtl,
    const char *obj_filename,
    char **buf,
    size_t *len)
{
    *buf = load_file(ctx, filename);
    *len = strlen(*buf);
}

mesh_t* mesh_load(sbox_t* sbox, const char* path) {
    info(sbox, "loading mesh '%s'", path);

    tinyobj_attrib_t attrib;
    tinyobj_shape_t* shapes = NULL;
    size_t num_shapes = 0;
    tinyobj_material_t* materials = NULL;
    size_t num_materials = 0;

    int result = tinyobj_parse_obj(
        &attrib,
        &shapes, &num_shapes,
        &materials, &num_materials,
        path,
        file_callback,
        sbox,
        TINYOBJ_FLAG_TRIANGULATE);

    if (result != TINYOBJ_SUCCESS) {
        const char* msg;
        switch (result) {
        case TINYOBJ_ERROR_EMPTY: msg = "file is empty"; break;
        case TINYOBJ_ERROR_INVALID_PARAMETER: msg = "invalid parameter"; break;
        case TINYOBJ_ERROR_FILE_OPERATION: msg = "invalid file operation"; break;
        default: msg = "unknown"; break;
        }

        error(sbox, "failed to load mesh '%s': %s", path, msg);
        return NULL;
    }

    int stride = 14;
    int num_vertices = attrib.num_faces * stride;
    float* vertices = malloc(num_vertices * sizeof(float));
    int num_indices = attrib.num_faces;
    uint32_t* indices = malloc(num_indices * sizeof(uint32_t));

    bbox_t bbox = {0};

    int len = 0;
    for (int i = 0; i < num_indices; i++) {
        tinyobj_vertex_index_t index = attrib.faces[i];

        if (index.v_idx != TINYOBJ_INVALID_INDEX) {
            size_t base = index.v_idx * 3;
            float x = attrib.vertices[base + 0];
            float y = attrib.vertices[base + 1];
            float z = attrib.vertices[base + 2];

            vertices[len++] = x;
            vertices[len++] = y;
            vertices[len++] = z;

            bbox.min[0] = min(bbox.min[0], x);
            bbox.min[1] = min(bbox.min[1], y);
            bbox.min[2] = min(bbox.min[2], z);

            bbox.max[0] = max(bbox.max[0], x);
            bbox.max[1] = max(bbox.max[1], y);
            bbox.max[2] = max(bbox.max[2], z);
        }

        if (index.vn_idx != TINYOBJ_INVALID_INDEX) {
            size_t base = index.vn_idx * 3;
            vertices[len++] = attrib.normals[base + 0];
            vertices[len++] = attrib.normals[base + 1];
            vertices[len++] = attrib.normals[base + 2];
        }

        if (index.vt_idx != TINYOBJ_INVALID_INDEX) {
            size_t base = index.vt_idx * 2;
            vertices[len++] = attrib.texcoords[base + 0];
            vertices[len++] = attrib.texcoords[base + 1];
        }

        if (num_materials == 0 || num_materials == 1)
            vertices[len++] = 0.0f;
        else
            vertices[len++] = attrib.material_ids[i];

        indices[i] = i;
    }

    tinyobj_attrib_free(&attrib);
    return mesh_new(sbox, vertices, num_vertices, indices, num_indices, bbox);
}

void mesh_free(sbox_t* sbox, mesh_t* mesh) {
    glDeleteVertexArrays(1, &mesh->vao);
    glDeleteBuffers(1, &mesh->vbo);
    glDeleteBuffers(1, &mesh->ebo);
    free(mesh);
}

texture_t* texture_new(sbox_t* sbox, int width, int height, uint8_t* data) {
    uint32_t id;
    glGenTextures(1, &id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    texture_t* texture = malloc(sizeof(texture_t));
    texture->id = id;
    texture->width = width;
    texture->height = height;

    texture->next = sbox->textures;
    sbox->textures = texture;
    return texture;
}

texture_t* texture_load(sbox_t* sbox, const char* path) {
    info(sbox, "loading texture '%s'", path);

    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
    if (!data) {
        error(sbox, "failed to load texture '%s'", path);
        return NULL;
    }

    texture_t* texture = texture_new(sbox, width, height, data);
    stbi_image_free(data);
    return texture;
}

void texture_free(sbox_t* sbox, texture_t* texture) {
    glDeleteTextures(1, &texture->id);
    free(texture);
}

material_t* material_load(sbox_t* sbox,
    const char* albedo_path,
    const char* roughness_path,
    const char* normal_path,
    float tilex,
    float tiley,
    bool is_translucent)
{
    material_t* material = malloc(sizeof(material_t));
    material->albedo = texture_load(sbox, albedo_path);
    material->roughness = texture_load(sbox, roughness_path);
    material->normal = texture_load(sbox, normal_path);
    material->tilex = tilex;
    material->tiley = tiley;
    material->is_translucent = is_translucent;
    
    material->next = sbox->materials;
    sbox->materials = material;
    return material;
}

void material_free(sbox_t* sbox, material_t* material) {
    free(material);
}

void video_set_shader(shader_t* shader) {
    glUseProgram(shader->id);
}

void video_set_texture(texture_t* texture, int slot) {
    if (!texture) return;
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture->id);
}

void video_set_material(shader_t* shader, material_t* material, int slot) {
    if (!material) return;

    char slot_name[32];
    snprintf(slot_name, 32, "materials[%d].albedo", slot);
    shader_set_int(shader, slot_name, 3 * slot + 0);
    video_set_texture(material->albedo, 3 * slot + 0);
    
    snprintf(slot_name, 32, "materials[%d].roughness", slot);
    shader_set_int(shader, slot_name, 3 * slot + 1);
    video_set_texture(material->roughness, 3 * slot + 1);

    snprintf(slot_name, 32, "materials[%d].normal", slot);
    shader_set_int(shader, slot_name, 3 * slot + 2);
    video_set_texture(material->normal, 3 * slot + 2);

    snprintf(slot_name, 32, "materials[%d].tilex", slot);
    shader_set_float(shader, slot_name, material->tilex);

    snprintf(slot_name, 32, "materials[%d].tiley", slot);
    shader_set_float(shader, slot_name, material->tiley);
}

void video_draw_mesh(mesh_t* mesh) {
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->ntris, GL_UNSIGNED_INT, 0);
}
