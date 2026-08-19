#version 330 core

const float GAMMA = 2.2f;
const float PI = 3.14159265359f;

out vec4 frag_color;

in vec2 vs_uv;

struct GBuffer {
    sampler2D position;
    sampler2D normal;
    sampler2D albedo_roughness;
    sampler2D depth;
};

struct MaterialSample {
    vec3 position;
    vec3 normal;
    vec3 albedo;
    float roughness;
    float metallic;
    float ao;
};

uniform GBuffer gbuffer;
uniform sampler2D ssao;

void main() {
    MaterialSample sample;
    sample.position = texture(gbuffer.position, vs_uv).rgb;
    sample.normal = texture(gbuffer.normal, vs_uv).rgb;
    sample.albedo = pow(texture(gbuffer.albedo_roughness, vs_uv).rgb, vec3(GAMMA));
    sample.roughness = texture(gbuffer.albedo_roughness, vs_uv).a;
    sample.metallic = 0.0f;
    sample.ao = texture(ssao, vs_uv).r;

    vec3 ambient = vec3(0.5f, 0.5f, 0.9f) * sample.albedo * sample.ao;
    frag_color = vec4(ambient, 1.0f);
}
