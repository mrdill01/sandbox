#version 330 core

const float GAMMA = 2.2f;
const float PI = 3.14159265359f;

#define VISUALIZE_POSITION 0
#define VISUALIZE_ALBEDO 1
#define VISUALIZE_NORMALS 2
#define VISUALIZE_ROUGHNESS 3
#define VISUALIZE_DEPTH_BUFFER 4
#define VISUALIZE_BUFFER -1

out vec4 frag_color;

in vec2 vs_uv;

struct GBuffer {
    sampler2D position;
    sampler2D normal;
    sampler2D albedo_roughness;
    sampler2D depth;
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

    vec3 ambient = vec3(0.4) * sample.albedo * sample.ao;
    
    frag_color = vec4(ambient, 1.0f);

    #if VISUALIZE == -1
    return;
    #endif

    #if VISUALIZE_BUFFER == VISUALIZE_POSITION
    frag_color = vec4(sample.position, 1.0f);
    #elif VISUALIZE_BUFFER == VISUALIZE_NORMALS
    frag_color = vec4(sample.normal * 0.5f + 0.5f, 1.0f);
    #elif VISUALIZE_BUFFER == VISUALIZE_ALBEDO
    frag_color = vec4(sample.albedo, 1.0f);
    #elif VISUALIZE_BUFFER == VISUALIZE_ROUGHNESS
    frag_color = vec4(vec3(sample.roughness), 1.0f);
    #elif VISUALIZE_BUFFER == VISUALIZE_DEPTH_BUFFER
    frag_color = vec4(texture(gbuffer.depth, vs_uv).rrr, 1.0f);
    #endif
}
