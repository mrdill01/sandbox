#version 330 core

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

struct Light {
    vec3 position;
    vec3 color;
};

uniform vec3 view_pos;
uniform Material materials[MAX_MATERIALS];
uniform Light light;

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
    vec3 view_dir = normalize(view_pos - vs_frag_position);

    vec2 tiled_uv = vs_uv * vec2(materials[vs_mat].tilex, materials[vs_mat].tiley);

    vec4 albedo = texture(materials[vs_mat].albedo, tiled_uv);
    albedo.rgb = pow(albedo.rgb, vec3(GAMMA));
    float roughness = texture(materials[vs_mat].roughness, tiled_uv).r;
    vec3 normal = perturb_normal(vs_normal, view_dir, tiled_uv);

    vec3 ambient = vec3(0.3, 0.3, 0.4) * albedo.rgb;

    vec3 norm = normalize(normal);
    vec3 light_dir = normalize(light.position - vs_frag_position);
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = light.color * diff * albedo.rgb;

    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = light.color * spec * roughness;

    frag_color = vec4(pow(ambient + diffuse + specular, vec3(1.0 / GAMMA)), albedo.w);
}
