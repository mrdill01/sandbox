#version 330 core

in vec2 vs_uv;

uniform sampler2D albedo;
uniform float tilex;
uniform float tiley;

void main() {
    if (texture(albedo, vs_uv * vec2(tilex, tiley)).a < 0.001f)
        discard;
}
