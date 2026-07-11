#include "render.h"
#include "sbox.h"

#include "../include/gl.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "../include/tinyobj_loader_c.h"

mesh_t* mesh_new(sbox_t* sbox,
    float* vertices, size_t nvertices,
    uint32_t* indices, size_t nindices,
    uint8_t nmaterials, bbox_t bbox)
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
    mesh->nmaterials = nmaterials;
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
    info(sbox, "loading mesh %s", path);

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
        0);

    if (result != TINYOBJ_SUCCESS) {
        const char* msg;
        switch (result) {
        case TINYOBJ_ERROR_EMPTY: msg = "file is empty"; break;
        case TINYOBJ_ERROR_INVALID_PARAMETER: msg = "invalid parameter"; break;
        case TINYOBJ_ERROR_FILE_OPERATION: msg = "invalid file operation"; break;
        default: msg = "unknown"; break;
        }

        error(sbox, "failed to load mesh %s: %s", path, msg);
        return NULL;
    }

    int stride = 9;
    int num_vertices = attrib.num_faces * stride;
    float* vertices = malloc(num_vertices * sizeof(float));
    int num_indices = attrib.num_faces;
    uint32_t* indices = malloc(num_indices * sizeof(uint32_t));

    bbox_t bbox = {0};

    int len = 0;
    for (int i = 0; i < attrib.num_faces; i++) {
        tinyobj_vertex_index_t face = attrib.faces[i];

        if (face.v_idx != TINYOBJ_INVALID_INDEX) {
            int base = face.v_idx * 3;
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

        if (face.vn_idx != TINYOBJ_INVALID_INDEX) {
            int base = face.vn_idx * 3;
            vertices[len++] = attrib.normals[base + 0];
            vertices[len++] = attrib.normals[base + 1];
            vertices[len++] = attrib.normals[base + 2];
        }

        if (face.vt_idx != TINYOBJ_INVALID_INDEX) {
            int base = face.vt_idx * 2;
            vertices[len++] = attrib.texcoords[base + 0];
            vertices[len++] = attrib.texcoords[base + 1];
        }

        if (num_materials == 0 || num_materials == 1)
            vertices[len++] = 0.0f;
        else
            vertices[len++] = attrib.material_ids[i];

        indices[i] = i;
    }

    if (num_materials == 0)
        num_materials = 1;

    tinyobj_attrib_free(&attrib);
    return mesh_new(sbox, vertices, num_vertices, indices, num_indices, num_materials, bbox);
}

void mesh_free(sbox_t* sbox, mesh_t* mesh) {
    if (!mesh) return;
    glDeleteVertexArrays(1, &mesh->vao);
    glDeleteBuffers(1, &mesh->vbo);
    glDeleteBuffers(1, &mesh->ebo);
    free(mesh);
}
