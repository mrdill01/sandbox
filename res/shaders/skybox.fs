#version 330 core

layout (location = 0) out vec3 g_position;
layout (location = 1) out vec4 g_albedo_roughness;
layout (location = 2) out vec3 g_normal;
layout (location = 3) out vec3 g_depth;

#define GAMMA 2.2

in vec3 vs_frag_position;
in vec3 vs_sample_dir;
in vec2 vs_uv;

uniform samplerCube cubemap;

void main() {
    vec3 color = texture(cubemap, vs_sample_dir).rgb;
    color = pow(color, vec3(GAMMA));
    g_position = vs_frag_position;
    g_albedo_roughness.rgb = color;
    g_albedo_roughness.a = 1.0f;
    g_normal = -vs_sample_dir;
    g_depth.r = gl_FragCoord.z;
}
