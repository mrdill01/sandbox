#version 330 core

const float GAMMA = 2.2f;
const float PI = 3.14159265359f;

out vec4 frag_color;

in vec2 vs_uv;

struct GBuffer {
    sampler2D position;
    sampler2D normal;
    sampler2D albedo_roughness;
};

struct GBufferSample {
    vec3 position;
    vec3 normal;
    vec3 albedo;
    float roughness;
    float metallic;
    float ao;
};

uniform GBuffer gbuffer;
uniform vec3 view_position;

void main() {
    GBufferSample sample;
    sample.position = texture(gbuffer.position, vs_uv).rgb;
    sample.normal = texture(gbuffer.normal, vs_uv).rgb;
    sample.albedo = pow(texture(gbuffer.albedo_roughness, vs_uv).rgb, vec3(GAMMA));
    sample.roughness = texture(gbuffer.albedo_roughness, vs_uv).a;
    sample.metallic = 0.0f;
    sample.ao = 1.0f;
    vec3 view_dir = normalize(view_position - sample.position);

    vec3 ambient = vec3(0.05) * sample.albedo * sample.ao;
    
    vec3 color = ambient;
    color = color / (color + vec3(1.0f));
    color = pow(color, vec3(1.0f / 2.2f)); 
    frag_color = vec4(color, 1.0f);
}
