#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUV;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;
layout(location = 3) out vec2 fragTextCoord;

void main() {
    gl_Position = vec4((inPosition.x - 0.5) * 2, (inPosition.y - 0.5) * 2, 0.0, 1.0) ;
    fragPosWorld = vec3(inPosition, 0.0);
    fragNormalWorld = inNormal;
    fragColor = inColor;
    fragTextCoord = inUV;
}