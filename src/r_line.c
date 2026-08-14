#include "render.h"
#include "quark.h"

#include "../include/gl.h"

void r_add_line(quark_t* quark,
    renderer_t* renderer, vec3 start, vec3 end, vec4 color, float decay_time)
{
    line_t* line = NULL;
    for (int i = 0; i < MAX_LINES; i++) {
        line = &renderer->lines[i];
        if (line->is_free) {
            line = &renderer->lines[i];
            line->is_free = false;
            break;
        }
    }

    if (!line)
        line = &renderer->lines[0];

    float vertices[] = {
        start[0], start[1], start[2], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        end[0], end[1], end[2], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    };
    uint32_t indices[] = {
        0, 1, 2,
        3, 4, 5
    };

    size_t nvertices = sizeof(vertices) / sizeof(vertices[0]);
    size_t nindices = sizeof(indices) / sizeof(indices[0]);

    mesh_buffer_t** buffers = malloc(sizeof(mesh_buffer_t*) * 1);
    buffers[0] = mesh_buffer_new(quark, nvertices, nindices);
    memcpy(buffers[0]->vertices, vertices, nvertices * sizeof(float));
    memcpy(buffers[0]->indices, indices, nindices * sizeof(uint32_t));

    bbox_t bbox = {0};
    if (line->mesh)
        mesh_free(quark, line->mesh);
    
    line->mesh = mesh_new(quark, buffers, 1, 0, bbox);
    glm_vec4_copy(color, line->color);
    line->spawn_time = quark->time;
    line->decay_time = decay_time;
}

void r_add_line_box(quark_t* quark, renderer_t* renderer, const bbox_t* bbox, vec4 color, float decay_time) {
    vec3 pairs[] = {
        {bbox->min[0], bbox->max[1], bbox->max[2]}, {bbox->max[0], bbox->max[1], bbox->max[2]},
        {bbox->min[0], bbox->min[1], bbox->max[2]}, {bbox->max[0], bbox->min[1], bbox->max[2]},
        {bbox->min[0], bbox->max[1], bbox->min[2]}, {bbox->max[0], bbox->max[1], bbox->min[2]},
        {bbox->min[0], bbox->min[1], bbox->min[2]}, {bbox->max[0], bbox->min[1], bbox->min[2]},

        {bbox->min[0], bbox->min[1], bbox->min[2]}, {bbox->min[0], bbox->max[1], bbox->min[2]},
        {bbox->max[0], bbox->min[1], bbox->min[2]}, {bbox->max[0], bbox->max[1], bbox->min[2]},
        {bbox->max[0], bbox->min[1], bbox->max[2]}, {bbox->max[0], bbox->max[1], bbox->max[2]},
        {bbox->min[0], bbox->min[1], bbox->max[2]}, {bbox->min[0], bbox->max[1], bbox->max[2]},

        {bbox->min[0], bbox->min[1], bbox->min[2]}, {bbox->min[0], bbox->min[1], bbox->max[2]},
        {bbox->max[0], bbox->min[1], bbox->min[2]}, {bbox->max[0], bbox->min[1], bbox->max[2]},
        {bbox->max[0], bbox->max[1], bbox->min[2]}, {bbox->max[0], bbox->max[1], bbox->max[2]},
        {bbox->min[0], bbox->max[1], bbox->min[2]}, {bbox->min[0], bbox->max[1], bbox->max[2]},
    };

    size_t len = sizeof(pairs) / sizeof(pairs[0]);
    for (size_t i = 0; i < len; i += 2) {
        r_add_line(quark, renderer, pairs[i], pairs[i + 1], color, decay_time);
    }
}

void r_render_lines(quark_t* quark, renderer_t* renderer) {
    r_set_shader(renderer, renderer->line_shader);
    
    r_set_mat4(quark, renderer, "view", renderer->view);
    r_set_mat4(quark, renderer, "projection", renderer->projection);
    
    for (int i = 0; i < MAX_LINES; i++) {
        line_t* line = &renderer->lines[i];
        if (line->is_free) continue;
        if (quark->time - line->spawn_time >= line->decay_time) {
            line->is_free = true;
        }

        r_set_vec4(quark, renderer, "color", line->color);
        glBindVertexArray(line->mesh->buffers[0]->vao);
        glDrawArrays(GL_LINES, 0, 3);   
    }
}
