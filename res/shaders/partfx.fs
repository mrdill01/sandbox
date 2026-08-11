#version 330 core

#define GAMMA 2.2

out vec4 frag_color;

in vec2 vs_uv;

uniform sampler2D particle;
uniform vec3 color;
uniform float alpha;

void main() {
    vec4 sample = texture(particle, vs_uv);
    sample.rgb = pow(sample.rgb * color, vec3(GAMMA));
    sample.a *= alpha;
    frag_color = sample;
}
