#version 330 core

const float GAMMA = 2.2f;
const float PI = 3.14159265359f;
#define MAX_MATERIALS 4

out vec4 frag_color;

in vec3 vs_frag_position;
in vec3 vs_normal;
in vec2 vs_uv;
flat in int vs_mat;

struct Material {
    sampler2D albedo;
    sampler2D roughness;
    sampler2D normal;
    float tilex;
    float tiley;
    float scrollx;
    float scrolly;
};

struct MaterialSample {
    vec3 position;
    vec3 normal;
    vec4 albedo;
    float roughness;
    float metallic;
    float ao;
};

struct SunLight {
    vec3 direction;
    vec3 color;
};

uniform Material materials[MAX_MATERIALS];
uniform SunLight sun_light;
uniform vec3 view_position;

mat3 cotangent_frame(vec3 normal, vec3 p, vec2 uv) {
    vec3 dp1 = dFdx(p);
    vec3 dp2 = dFdy(p);
    vec2 duv1 = dFdx(uv);
    vec2 duv2 = dFdy(uv);
 
    vec3 dp2perp = cross(dp2, normal);
    vec3 dp1perp = cross(normal, dp1);
    vec3 tangent = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 bitangent = dp2perp * duv1.y + dp1perp * duv2.y;
 
    float invmax = inversesqrt(max(dot(tangent, tangent), dot(bitangent, bitangent)));
    return mat3(tangent * invmax, bitangent * invmax, normal);
}

vec3 perturb_normal(vec3 normal, vec3 view_dir, vec2 uv) {
    mat3 tbn = cotangent_frame(normal, -view_dir, uv);
    vec3 map = texture2D(materials[vs_mat].normal, uv).xyz;
    map.y = 1.0f - map.y;
    return normalize(tbn * map);
}

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
    vec3 l = -sun_light.direction;
    vec3 h = normalize(view_dir + l);
    float attenuation = 1.0;
    vec3 radiance = sun_light.color * attenuation;

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
    return (kd * sample.albedo.rgb / PI + specular) * radiance * n_dot_l;
}

void main() {
    vec2 uv = vs_uv * vec2(materials[vs_mat].tilex, materials[vs_mat].tiley);
    uv += vec2(materials[vs_mat].scrollx, materials[vs_mat].scrolly);

    vec3 view_dir = normalize(view_position - vs_frag_position);

    MaterialSample sample;
    sample.position = vs_frag_position;
    sample.normal = perturb_normal(vs_normal, view_dir, uv);
    sample.albedo = texture(materials[vs_mat].albedo, uv);
    sample.albedo.rgb = pow(sample.albedo.rgb, vec3(GAMMA));
    sample.roughness = texture(materials[vs_mat].roughness, uv).r;
    sample.metallic = 0.0f;
    sample.ao = 1.0f;

    vec3 f0 = vec3(0.04); 
    f0 = mix(f0, sample.albedo.rgb, sample.metallic);

    vec3 direct = vec3(0.0);
    direct += draw_light(view_dir, f0, sample);

    vec3 ambient = vec3(0.4) * sample.albedo.rgb * sample.ao;
    
    frag_color = vec4(direct + ambient, sample.albedo.a);
}
