#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in float a_mat;

#define MAX_MATERIALS 4

out vec3 vs_frag_position;
out vec3 vs_normal;
out vec2 vs_uv;
flat out int vs_mat;

struct Material {
    sampler2D albedo;
    sampler2D roughness;
    sampler2D normal;
    float wind_factor;
    float tilex;
    float tiley;
    float scrollx;
    float scrolly;
};

uniform Material materials[MAX_MATERIALS];
uniform float time;
uniform float hitbox_height;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec3 vertex_position = a_position;

    if (materials[0].wind_factor > 0.0f) {
        float height_scaling = a_position.y / hitbox_height;
        float speed = 1.5f;
        float amount = 0.125f;
        vertex_position.x += sin(time * speed) * cos(time * a_position.z * speed * 0.5f) *
            amount * height_scaling * materials[0].wind_factor;
        vertex_position.z += sin(time * speed * 0.5f) * cos(time * a_position.x * speed * 0.25f) *
            amount * height_scaling * materials[0].wind_factor;
    }

    gl_Position = projection * view * model * vec4(vertex_position, 1.0);
    vs_frag_position = vec3(model * vec4(vertex_position, 1.0));
    vs_normal = mat3(transpose(inverse(model))) * a_normal;
    vs_uv = a_uv;
    vs_mat = int(a_mat);
}
