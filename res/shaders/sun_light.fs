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
    float shadow;
};

struct SunLight {
    vec3 direction;
    vec3 color;
    sampler2D shadow;
    mat4 matrix;
};

uniform GBuffer gbuffer;
uniform SunLight light;
uniform vec3 view_position;
  
float distribution_ggx(vec3 normal, vec3 h, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float n_dot_h = max(dot(normal, h), 0.0f);
    float n_dot_h2 = n_dot_h * n_dot_h;
    float nom = a2;
    float denom = (n_dot_h2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;
    return nom / denom;
}

float geometry_schlick_ggx(float n_dot_v, float roughness) {
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;
    float nom = n_dot_v;
    float denom = n_dot_v * (1.0f - k) + k;
    return nom / denom;
}

float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness) {
    float n_dot_v = max(dot(N, V), 0.0f);
    float n_dot_l = max(dot(N, L), 0.0f);
    float ggx2 = geometry_schlick_ggx(n_dot_v, roughness);
    float ggx1 = geometry_schlick_ggx(n_dot_l, roughness);
    return ggx1 * ggx2;
}

vec3 fresnel_schlick(float cos_theta, vec3 f0) {
    return f0 + (1.0f - f0) * pow(clamp(1.0f - cos_theta, 0.0f, 1.0f), 5.0f);
}

vec3 draw_light(vec3 view_dir, vec3 f0, MaterialSample sample) {
    vec3 l = -light.direction;
    vec3 h = normalize(view_dir + l);
    float attenuation = 1.0;
    vec3 radiance = light.color * attenuation;

    float ndf = distribution_ggx(sample.normal, h, sample.roughness);   
    float g = geometry_smith(sample.normal, view_dir, l, sample.roughness);      
    vec3 f = fresnel_schlick(max(dot(h, view_dir), 0.0), f0);
        
    vec3 numerator = ndf * g * f; 
    float denominator = 4.0 * max(dot(sample.normal, view_dir), 0.0) *
        max(dot(sample.normal, l), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;
    
    vec3 ks = f;
    vec3 kd = vec3(1.0) - ks;
    kd *= 1.0 - sample.metallic;	  

    float n_dot_l = max(dot(sample.normal, l), 0.0);        
    return (kd * sample.albedo / PI + specular) * radiance * n_dot_l;
}

float compute_shadow(vec4 frag_pos_light_space) {
    vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
    proj_coords = proj_coords * 0.5f + 0.5f;
    float closest_depth = texture(light.shadow, proj_coords.xy).r;
    float current_depth = proj_coords.z;
    float shadow = (current_depth > closest_depth) ? 1.0f : 0.0f;
    if (current_depth > 1.0f)
        shadow = 0.0f;
    return shadow;
}

void main() {
    MaterialSample sample;
    sample.position = texture(gbuffer.position, vs_uv).rgb;
    sample.normal = texture(gbuffer.normal, vs_uv).rgb;
    sample.albedo = pow(texture(gbuffer.albedo_roughness, vs_uv).rgb, vec3(GAMMA));
    sample.roughness = texture(gbuffer.albedo_roughness, vs_uv).a;
    sample.metallic = 0.0f;
    sample.ao = 1.0f;
    sample.shadow = compute_shadow(light.matrix * vec4(sample.position, 1.0f));
    vec3 view_dir = normalize(view_position - sample.position);

    vec3 f0 = vec3(0.04); 
    f0 = mix(f0, sample.albedo, sample.metallic);

    vec3 direct = vec3(0.0);
    direct += draw_light(view_dir, f0, sample) * (1.0f - sample.shadow);

    frag_color = vec4(direct, 0.5f);
}
