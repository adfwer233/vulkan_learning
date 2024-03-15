#version 450

layout (location = 0) in vec3 inPos;
layout (location = 0) out vec3 fragColor;

struct PointLight {
    vec4 position;
    vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 model;
    mat4 view;
    mat4 proj;

    vec3 cameraPos;
    PointLight pointLight;
} ubo;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPos, 1.0);
    fragColor = vec3(1.0, 0, 0);
}
