#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 _a_normal;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in float _a_mat;

out vec3 vs_sample_dir;
out vec2 vs_uv;

uniform mat4 inv_view;
uniform mat4 inv_projection;

void main() {
    gl_Position = vec4(((gl_VertexID & 1) << 2) - 1.0f, (gl_VertexID & 2) * 2.0f - 1.0f, 0.0f, 1.0f);
    vs_sample_dir = mat3(inv_view) * (inv_projection * gl_Position).xyz;
    vs_uv = a_uv;
}
