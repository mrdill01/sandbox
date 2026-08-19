#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 _a_normal;
layout (location = 2) in vec2 _a_uv;
layout (location = 3) in float _a_mat;

out vec2 vs_uv;

void main() {
    gl_Position = vec4(a_position, 1.0f);
    vs_uv = a_position.xy * 0.5f + 0.5f;
}
