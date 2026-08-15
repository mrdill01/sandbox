#include "render.h"
#include "quark.h"
#include "math.h"

#include "../../include/gl.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "../../include/tinyobj_loader_c.h"

mesh_buffer_t* mesh_buffer_new(quark_t* quark, size_t nvertices, size_t nindices) {
    mesh_buffer_t* buffer = malloc(sizeof(mesh_buffer_t));
    glGenVertexArrays(1, &buffer->vao);
    glGenBuffers(1, &buffer->vbo);
    glGenBuffers(1, &buffer->ebo);
    buffer->nvertices = nvertices;
    buffer->vertices = malloc(nvertices * sizeof(float));
    buffer->nindices = nindices;
    buffer->indices = malloc(nindices * sizeof(uint32_t));
    return buffer;
}

void mesh_buffer_upload(quark_t* quark, mesh_buffer_t* buffer) {
    glBindVertexArray(buffer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(float) * buffer->nvertices, buffer->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        sizeof(uint32_t) * buffer->nindices, buffer->indices, GL_STATIC_DRAW);

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
}

mesh_t* mesh_new(
    quark_t* quark, mesh_buffer_t** buffers, size_t nbuffers, uint8_t nmaterials, bbox_t bbox)
{
    mesh_t* mesh = malloc(sizeof(mesh_t));
    mesh->buffers = buffers;
    mesh->nbuffers = nbuffers;
    for (size_t i = 0; i < nbuffers; i++)
        mesh_buffer_upload(quark, mesh->buffers[i]);

    mesh->nmaterials = nmaterials;
    mesh->bbox = bbox;
    mesh->next = quark->meshes;
    quark->meshes = mesh;
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

mesh_t* mesh_load(quark_t* quark, const char* path) {
    info(quark, "loading mesh %s", path);

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
        quark,
        0);

    if (result != TINYOBJ_SUCCESS) {
        const char* msg;
        switch (result) {
        case TINYOBJ_ERROR_EMPTY: msg = "file is empty"; break;
        case TINYOBJ_ERROR_INVALID_PARAMETER: msg = "invalid parameter"; break;
        case TINYOBJ_ERROR_FILE_OPERATION: msg = "invalid file operation"; break;
        default: msg = "unknown"; break;
        }

        error(quark, "failed to load mesh %s: %s", path, msg);
        return NULL;
    }

    size_t nbuffers = 1;
    mesh_buffer_t** buffers = malloc(sizeof(mesh_buffer_t*) * nbuffers);

    int stride = 9;
    buffers[0] = mesh_buffer_new(quark, attrib.num_faces * stride, attrib.num_faces);
    
    if (num_materials == 0)
        num_materials = 1;
    int last_material = 1;

    bbox_t bbox = {0};

    int len = 0;
    for (int i = 0; i < attrib.num_faces; i++) {
        tinyobj_vertex_index_t face = attrib.faces[i];
        mesh_buffer_t* buffer = buffers[0];

        if (face.v_idx != TINYOBJ_INVALID_INDEX) {
            int base = face.v_idx * 3;
            float x = attrib.vertices[base + 0];
            float y = attrib.vertices[base + 1];
            float z = attrib.vertices[base + 2];

            buffer->vertices[len++] = x;
            buffer->vertices[len++] = y;
            buffer->vertices[len++] = z;

            bbox.min[0] = min(bbox.min[0], x);
            bbox.min[1] = min(bbox.min[1], y);
            bbox.min[2] = min(bbox.min[2], z);

            bbox.max[0] = max(bbox.max[0], x);
            bbox.max[1] = max(bbox.max[1], y);
            bbox.max[2] = max(bbox.max[2], z);
        }

        if (face.vn_idx != TINYOBJ_INVALID_INDEX) {
            int base = face.vn_idx * 3;
            buffer->vertices[len++] = attrib.normals[base + 0];
            buffer->vertices[len++] = attrib.normals[base + 1];
            buffer->vertices[len++] = attrib.normals[base + 2];
        }

        if (face.vt_idx != TINYOBJ_INVALID_INDEX) {
            int base = face.vt_idx * 2;
            buffer->vertices[len++] = attrib.texcoords[base + 0];
            buffer->vertices[len++] = attrib.texcoords[base + 1];
        }

        if (num_materials == 1)
            buffer->vertices[len++] = 0.0f;
        else
            buffer->vertices[len++] = attrib.material_ids[i / 3];

        buffer->indices[i] = i;
    }

    tinyobj_attrib_free(&attrib);
    tinyobj_shapes_free(shapes, num_shapes);
    tinyobj_materials_free(materials, num_materials);
    return mesh_new(quark, buffers, nbuffers, num_materials, bbox);
}

mesh_t* mesh_copy(quark_t* quark, const mesh_t* original) {
    mesh_buffer_t** buffers = malloc(sizeof(mesh_buffer_t*) * original->nbuffers);
    for (size_t i = 0; i < original->nbuffers; i++) {
        mesh_buffer_t* original_buffer = original->buffers[i];
        if (!original_buffer) continue;

        buffers[i] = mesh_buffer_new(quark, original_buffer->nvertices, original_buffer->nindices);
        memcpy(buffers[i]->vertices,
            original_buffer->vertices, original_buffer->nvertices * sizeof(float));
        memcpy(buffers[i]->indices,
            original_buffer->indices, original_buffer->nindices * sizeof(uint32_t));
    }

    mesh_t* mesh = mesh_new(quark, buffers, original->nbuffers, original->nmaterials, original->bbox);
    return mesh;
}

void mesh_free(quark_t* quark, mesh_t* mesh) {
    if (!mesh) return;
    for (size_t i = 0; i < mesh->nbuffers; i++) {
        mesh_buffer_t* buffer = mesh->buffers[i];
        if (!buffer) continue;
        glDeleteVertexArrays(1, &buffer->vao);
        glDeleteBuffers(1, &buffer->vbo);
        glDeleteBuffers(1, &buffer->ebo);
        free(buffer->vertices);
        free(buffer->indices);
        free(buffer);
    }
    free(mesh->buffers);
    free(mesh);
}

void mesh_deform(
    quark_t* quark,
    mesh_t* mesh,
    vec3 position,
    vec3 point,
    vec3 direction,
    float radius,
    float distance)
{
    for (size_t i = 0; i < mesh->nbuffers; i++) {
        mesh_buffer_t* buffer = mesh->buffers[i];
        if (!buffer) continue;

        for (int v = 0; v < buffer->nvertices; v += 9) {
            vec3 vertex_ws = {
                position[0] + buffer->vertices[v + 0],
                position[1] + buffer->vertices[v + 1],
                position[2] + buffer->vertices[v + 2]};

            float falloff = radius / glm_vec3_distance(point, vertex_ws);
            falloff = pow(falloff, 50.0f);
            falloff = clamp(falloff, 0.0f, 1.0f);
            
            buffer->vertices[v + 0] += direction[0] * distance * falloff;
            buffer->vertices[v + 1] += direction[1] * distance * falloff;
            buffer->vertices[v + 2] += direction[2] * distance * falloff;

            mesh->bbox.min[0] = min(mesh->bbox.min[0], buffer->vertices[v + 0]);
            mesh->bbox.min[1] = min(mesh->bbox.min[1], buffer->vertices[v + 1]);
            mesh->bbox.min[2] = min(mesh->bbox.min[2], buffer->vertices[v + 2]);

            mesh->bbox.max[0] = max(mesh->bbox.max[0], buffer->vertices[v + 0]);
            mesh->bbox.max[1] = max(mesh->bbox.max[1], buffer->vertices[v + 1]);
            mesh->bbox.max[2] = max(mesh->bbox.max[2], buffer->vertices[v + 2]);
        }

        mesh_buffer_upload(quark, buffer);
    }
}
