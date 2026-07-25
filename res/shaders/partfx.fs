#version 330 core

out vec4 frag_color;

in vec2 uv;

void main() {
    frag_color = vec4(uv, 1.0f, 1.0f);
}
