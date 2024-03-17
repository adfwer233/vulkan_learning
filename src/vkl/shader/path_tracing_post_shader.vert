#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUV;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;
layout(location = 3) out vec2 fragTextCoord;

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
    fragPosWorld = (ubo.proj * ubo.view * vec4(inPosition, 1.0)).xyz;
    fragNormalWorld = inNormal;
    fragColor = inColor;
    fragTextCoord = inUV;
}