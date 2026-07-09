#version 330 core

layout (location = 0) out vec3 g_position;
layout (location = 1) out vec3 g_normal;
layout (location = 2) out vec4 g_albedo_roughness;

#define GAMMA 2.2

#define MAX_MATERIALS 4

in vec3 vs_frag_position;
in vec3 vs_normal;
in vec2 vs_uv;
flat in int vs_mat;

out vec4 frag_color;

struct Material {
    sampler2D albedo;
    sampler2D roughness;
    sampler2D normal;
    float tilex;
    float tiley;
};

uniform Material materials[MAX_MATERIALS];
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
    return normalize(tbn * map);
}

void main() {
    vec2 uv = vs_uv * vec2(materials[vs_mat].tilex, materials[vs_mat].tiley);
    vec3 view_dir = normalize(view_position - vs_frag_position);

    g_position = vs_frag_position;
    g_normal = perturb_normal(vs_normal, view_dir, uv);
    g_albedo_roughness.rgb = texture(materials[vs_mat].albedo, uv).rgb;
    g_albedo_roughness.a = texture(materials[vs_mat].roughness, uv).r;
}
