#version 330 core

#define GAMMA 2.2

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
};

struct Light {
    vec3 position;
    vec3 color;
};

uniform GBuffer gbuffer;
uniform Light light;
uniform vec3 view_position;

void main() {
    GBufferSample sample;
    sample.position = texture(gbuffer.position, vs_uv).rgb;
    sample.normal = texture(gbuffer.normal, vs_uv).rgb;
    sample.albedo = pow(texture(gbuffer.albedo_roughness, vs_uv).rgb, vec3(GAMMA));
    sample.roughness = texture(gbuffer.albedo_roughness, vs_uv).a;

    vec3 view_dir = normalize(view_position - sample.position);
    vec3 ambient = vec3(0.3, 0.3, 0.4) * sample.albedo;

    vec3 light_dir = normalize(light.position - sample.position);
    float diff = max(dot(sample.normal, light_dir), 0.0);
    vec3 diffuse = light.color * diff * sample.albedo.rgb;

    vec3 reflect_dir = reflect(-light_dir, sample.normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = light.color * spec * sample.roughness;

    frag_color = vec4(pow(ambient + diffuse + specular, vec3(1.0 / GAMMA)), 1.0f);
}
