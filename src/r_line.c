#include "render.h"
#include "sbox.h"

#include "../include/gl.h"

void r_add_line(sbox_t* sbox,
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

    bbox_t bbox = {0};
    if (line->mesh)
        mesh_free(sbox, line->mesh);
    
    line->mesh = mesh_new(sbox,
        vertices, sizeof(vertices) / sizeof(vertices[0]),
        indices, sizeof(indices) / sizeof(indices[0]),
        0, bbox);
    glm_vec4_copy(color, line->color);
    line->spawn_time = sbox->time;
    line->decay_time = decay_time;
}

void r_add_line_box(sbox_t* sbox, renderer_t* renderer, const bbox_t* bbox, vec4 color, float decay_time) {
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
        r_add_line(sbox, renderer, pairs[i], pairs[i + 1], color, decay_time);
    }
}

void r_render_lines(sbox_t* sbox, renderer_t* renderer) {
    r_set_shader(renderer, renderer->line_shader);
    
    r_set_mat4(sbox, renderer, "view", renderer->view);
    r_set_mat4(sbox, renderer, "projection", renderer->projection);
    
    for (int i = 0; i < MAX_LINES; i++) {
        line_t* line = &renderer->lines[i];
        if (line->is_free) continue;
        if (sbox->time - line->spawn_time >= line->decay_time) {
            line->is_free = true;
        }

        r_set_vec4(sbox, renderer, "color", line->color);
        glBindVertexArray(line->mesh->vao);
        glDrawArrays(GL_LINES, 0, 3);   
    }
}
