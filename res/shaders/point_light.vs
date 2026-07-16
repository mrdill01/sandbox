#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in float a_mat;

out vec2 vs_uv;

void main() {
    gl_Position =  vec4(a_position, 1.0);
    vs_uv = a_position.xy * 0.5f + 0.5f;
}
