#version 330 core

out float frag_color;
  
in vec2 vs_uv;

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D noise;

uniform vec3 samples[64];
uniform mat4 view;
uniform mat4 projection;
uniform vec2 screen_size;

void main() {
    vec2 noise_scale = vec2(screen_size.x / 4.0f, screen_size.y / 4.0f);
    const int kernel_size = 64;
    const float radius = 0.5f;
    const float bias = 0.025f;

    vec3 frag_pos = texture(g_position, vs_uv).rgb;
    vec3 normal = normalize(texture(g_normal, vs_uv).rgb);
    vec3 random_vec = normalize(texture(noise, vs_uv * noise_scale).rgb);

    vec3 tangent = normalize(random_vec - normal * dot(random_vec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 tbn = mat3(tangent, bitangent, normal);

    float occlusion = 0.0f;
    for(int i = 0; i < kernel_size; ++i) {
        vec3 sample_pos = tbn * samples[i];
        sample_pos = frag_pos + sample_pos * radius;

        vec4 offset = vec4(sample_pos, 1.0f);
        offset = projection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5f + 0.5f;

        float sample_depth = texture(g_position, offset.xy).z;

        float range_check = smoothstep(0.0f, 1.0f, radius / abs(frag_pos.z - sample_depth));
        occlusion += (sample_depth >= sample_pos.z + bias ? 1.0 : 0.0) * range_check; 
    }

    occlusion = 1.0f - (occlusion / kernel_size);
    frag_color = occlusion;
}
