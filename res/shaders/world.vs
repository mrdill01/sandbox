#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in float a_mat;

out vec3 vs_frag_pos;
out vec3 vs_normal;
out vec2 vs_uv;
flat out int vs_mat;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
    vs_frag_pos = vec3(model * vec4(a_pos, 1.0));
    vs_normal = mat3(transpose(inverse(model))) * a_normal;
    vs_uv = a_uv;
    vs_mat = int(a_mat);
}
