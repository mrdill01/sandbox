#version 330 core

const float GAMMA = 2.2f;
const float PI = 3.14159265359f;

out vec4 frag_color;

in vec2 vs_uv;
in float vs_visibility;

uniform sampler2D screen;
uniform sampler2D depth;
uniform vec3 view_direction;
uniform vec3 sun_direction;

vec3 add_fog(vec3 color, float depth) {
    const vec3 default_color = vec3(0.2f, 0.75f, 0.9f);
    const vec3 sun_color = vec3(1.0f, 1.0f, 0.25f);

    float amount = pow(depth, 4.0f);
    amount = clamp(amount, 0.0f, 1.0f);

    float sun_factor = max(dot(view_direction, sun_direction), 0.0f);
    sun_factor = pow(sun_factor, 8.0f);
    vec3 fog_color = mix(default_color, sun_color, sun_factor);
    return mix(color, fog_color, amount);
}

vec3 vignette(vec3 color, vec2 uv, float radius, float smoothness) {
	float difference = radius - distance(uv, vec2(0.5, 0.5));
	float vignette = smoothstep(-smoothness, smoothness, difference);
    return mix(color, color * vec3(vignette), 0.5f);
}

void main() {
    float depth = texture(depth, vs_uv).r;
    vec3 color = texture(screen, vs_uv).rgb;
    color = vignette(color, vs_uv, 0.5f, 0.25f);

    color = add_fog(color, depth);
    color = color / (color + vec3(1.0f));
    color = pow(color, vec3(1.0f / 2.2f)); 
    frag_color = vec4(color, 1.0f);
}
