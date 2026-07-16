#version 330 core

out vec4 frag_color;

in vec2 vs_uv;

uniform vec2 src_pos;
uniform vec2 src_size;
uniform sampler2D sprite;
uniform vec4 color;

void main() {
    frag_color = texture(sprite, src_pos + vs_uv * src_size) * color;
}
