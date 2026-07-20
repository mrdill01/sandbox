#version 330 core

in vec2 vs_uv;

uniform sampler2D albedo;

void main() {
    //if (texture(albedo, vs_uv).a < 0.001f)
    //    discard;
}
