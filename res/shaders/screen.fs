#version 330 core

const float GAMMA = 2.2f;
const float PI = 3.14159265359f;

out vec4 frag_color;

in vec2 vs_uv;

uniform sampler2D screen;

void main() {
    vec3 color = texture(screen, vs_uv).rgb;
    color = color / (color + vec3(1.0f));
    color = pow(color, vec3(1.0f / 2.2f)); 
    frag_color = vec4(color, 1.0f);
}
