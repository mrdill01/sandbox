#version 330 core

out vec4 frag_color;

in vec3 vs_sample_dir;
in vec2 vs_uv;

uniform samplerCube cubemap;
uniform sampler2D depth;

void main() {
    float depth = texture(depth, vs_uv).r;
    discard;
    frag_color = texture(cubemap, vs_sample_dir);
}
