#version 330 core

#define GAMMA 2.2

out vec4 frag_color;

in vec3 vs_sample_dir;
in vec2 vs_uv;

uniform samplerCube cubemap;
uniform sampler2D depth;

void main() {
    float depth = texture(depth, vs_uv).r;
    if (depth < 1.0f)
        discard;
    
    vec3 color = texture(cubemap, vs_sample_dir).rgb;
    color = pow(color, vec3(GAMMA));
    frag_color = vec4(color, 1.0f);
}
