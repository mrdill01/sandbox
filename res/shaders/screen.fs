#version 330 core

const float GAMMA = 2.2f;
const float PI = 3.14159265359f;

out vec4 frag_color;

in vec2 vs_uv;

uniform sampler2D screen;

uniform sampler2D g_position;
uniform sampler2D g_albedo_roughness;
uniform sampler2D g_normal;
uniform sampler2D g_depth;
uniform int debug_buffer;
uniform sampler2D sun_shadow;

uniform vec3 view_position;
uniform vec3 view_direction;
uniform vec3 sun_direction;

float linearize_depth(float depth) {
    float ndc = depth * 2.0f - 1.0f;
    float near = 0.01f;
    float far = 100.0f;
    float linear_depth = (2.0f * near * far) / (far + near - ndc * (far - near));	
    linear_depth /= far; 
    return linear_depth;
}

vec3 add_fog(vec3 color, float depth) {
    const vec3 default_color = vec3(0.25f, 0.75f, 1.85f);
    const vec3 sun_color = vec3(6.0f, 4.0f, 1.25f);
    const float density = 0.07f;

    float amount = 1.0f - exp(-depth * density);
    amount = clamp(amount, 0.0f, 1.0f);

    float sun_factor = dot(normalize(view_position - texture(g_position, vs_uv).rgb), sun_direction);
    //sun_factor = pow(sun_factor, 8.0f);
    sun_factor = clamp(sun_factor, 0.0f, 1.0f);
    vec3 fog_color = mix(default_color, sun_color, sun_factor);

    return mix(color, fog_color, amount);
}

vec3 vignette(vec3 color, vec2 uv, float radius, float smoothness) {
	float difference = radius - distance(uv, vec2(0.5f, 0.5f));
	float vignette = smoothstep(-smoothness, smoothness, difference);
    return mix(color, color * vec3(vignette), 0.5f);
}

void main() {
    vec3 color = texture(screen, vs_uv).rgb;
    color = vignette(color, vs_uv, 0.5f, 0.25f);
    color = add_fog(color, texture(g_depth, vs_uv).r);
    color = color / (color + vec3(1.0f));
    color = pow(color, vec3(1.0f / GAMMA)); 

    if (debug_buffer == 0)
        frag_color = vec4(color, 1.0f);
    else if (debug_buffer == 1)
        frag_color = vec4(texture(g_position, vs_uv).rgb, 1.0f);
    else if (debug_buffer == 2)
        frag_color = vec4(texture(g_albedo_roughness, vs_uv).rgb, 1.0f);
    else if (debug_buffer == 3)
        frag_color = vec4(vec3(texture(g_albedo_roughness, vs_uv).a), 1.0f);
    else if (debug_buffer == 4)
        frag_color = vec4(texture(g_normal, vs_uv).rgb * 0.5f + 0.5f, 1.0f);
    else if (debug_buffer == 5)
        frag_color = vec4(vec3(texture(g_depth, vs_uv).r), 1.0f);
    //frag_color = vec4(vec3(linearize_depth(texture(sun_shadow, vs_uv).r)), 1.0f);
}
