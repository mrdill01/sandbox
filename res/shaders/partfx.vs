#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in float a_mat;

out vec2 vs_uv;

uniform mat4 view;
uniform mat4 projection;

uniform vec3 position;
uniform float size;
uniform vec3 camera_right;
uniform vec3 camera_up;

void main() {
    vec3 vertex_position_worldspace = position +
        camera_right * a_position.x * size +
        camera_up * a_position.y * size;

    gl_Position = projection * view * vec4(vertex_position_worldspace, 1.0);
    vs_uv = a_uv;
}
