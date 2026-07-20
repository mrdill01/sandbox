#version 330 core

const float GAMMA = 2.2f;
const float PI = 3.14159265359f;

out vec4 frag_color;

in vec2 vs_uv;

uniform sampler2D screen;
uniform sampler2D depth;
uniform sampler2D position;
uniform sampler2D debug;
uniform vec3 view_position;
uniform vec3 view_direction;
uniform vec3 sun_direction;

vec3 add_fog(vec3 color, float depth) {
    const vec3 default_color = vec3(0.2f, 0.75f, 0.9f);
    const vec3 sun_color = vec3(6.0f, 4.0f, 1.25f);

    float amount = pow(depth, 4.0f);
    amount = clamp(amount, 0.0f, 1.0f);

    /*vec3 view_dir = normalize(view_position - texture(position, vs_uv).rgb);
    float sun_factor = max(dot(view_dir, sun_direction), 0.0f);
    sun_factor = pow(sun_factor, 8.0f);
    vec3 fog_color = mix(default_color, sun_color, amount);*/

    vec3 fog_color = default_color;
    return mix(color, fog_color, amount);
}

vec3 vignette(vec3 color, vec2 uv, float radius, float smoothness) {
	float difference = radius - distance(uv, vec2(0.5f, 0.5f));
	float vignette = smoothstep(-smoothness, smoothness, difference);
    return mix(color, color * vec3(vignette), 0.5f);
}

void main() {
    float depth = texture(depth, vs_uv).r;
    vec3 color = texture(screen, vs_uv).rgb;
    color = vignette(color, vs_uv, 0.5f, 0.25f);
    color = add_fog(color, depth);
    color = color / (color + vec3(1.0f));
    color = pow(color, vec3(1.0f / GAMMA)); 
    frag_color = vec4(color, 1.0f);
    //frag_color = vec4(texture(debug, vs_uv).rrr, 1.0f);
}
