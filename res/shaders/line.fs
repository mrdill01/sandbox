#version 330 core

out vec4 frag_color;

uniform vec4 color;

float linearize_depth(float depth) {
    float ndc = depth * 2.0f - 1.0f;
    float near = 0.01f;
    float far = 100.0f;
    float linear_depth = (2.0f * near * far) / (far + near - ndc * (far - near));	
    linear_depth /= far; 
    return linear_depth;
}

void main() {
    float depth_fade = 1.0f - linearize_depth(gl_FragCoord.z);
    frag_color = color * vec4(vec3(depth_fade), 1.0f);
}
