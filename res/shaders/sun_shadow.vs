#version 330 core

layout (location = 0) in vec3 a_position;

uniform mat4 model;
uniform mat4 matrix;

void main() {
    gl_Position = matrix * model * vec4(a_position, 1.0);
}
