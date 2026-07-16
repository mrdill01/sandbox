#version 330 core

layout (location = 0) in vec3 a_position;

uniform mat4 model;
uniform mat4 light_space;

void main() {
    gl_Position = light_space * model * vec4(a_position, 1.0);
}
