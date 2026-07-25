float linearize_depth(float depth) {
    float ndc = depth * 2.0f - 1.0f;
    float near = 0.01f;
    float far = 100.0f;
    float linear_depth = (2.0f * near * far) / (far + near - ndc * (far - near));	
    linear_depth /= far; 
    return linear_depth;
}
