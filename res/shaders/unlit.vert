#version 440 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;    // 0xAARRGGBB
layout (location = 3) in vec2 uv0;
layout (location = 4) in vec2 uv1;

out vec4 variedColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

vec4 convertToSRGB(const uint color) {
    vec4 rgba = vec4(
        float((color >> 16) & 0xFF) / 255.0f,
        float((color >>  8) & 0xFF) / 255.0f,
        float((color >>  0) & 0xFF) / 255.0f,
        float((color >> 24) & 0xFF) / 255.0f
    );

    // Convert to sRGB color space using the sRGB transfer function
    vec4 srgb = mix(
        pow((rgba + 0.055f) / 1.055f, vec4(2.4f)),
        rgba / 12.92f,
        step(0.04045, rgba)
    );

    return rgba;
}

void main() {
	gl_Position = projection * view * model * vec4(position, 1.0f);
	variedColor = color;
}