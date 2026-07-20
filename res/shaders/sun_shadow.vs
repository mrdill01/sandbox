#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 _a_normal;
layout (location = 2) in vec2 a_uv;

out vec2 vs_uv;

uniform mat4 model;
uniform mat4 matrix;

void main() {
    gl_Position = matrix * model * vec4(a_position, 1.0);
    vs_uv = a_uv;
}
