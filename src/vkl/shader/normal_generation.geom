#version 450

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

layout(location = 2) in vec3 inNormal[];

layout(location = 0) out vec3 outColor;

struct PointLight {
    vec4 position;
    vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 model;
    mat4 view;
    mat4 proj;

    PointLight pointLight;
    vec3 cameraPos;
} ubo;

void main(void) {
    float normalLength = 1;

    for (int i = 0; i < gl_in.length(); i++) {
        vec3 pos = gl_in[i].gl_Position.xyz;
        vec3 normal = vec3(0, 0, 1);

        gl_Position = ubo.proj * ubo.view * ubo.model * vec4(pos, 1.0);
        outColor = vec3(1.0, 0.0, 0.0);
        EmitVertex();

        gl_Position = ubo.proj * ubo.view * ubo.model * (vec4(pos + normal, 1.0));
        outColor = vec3(1.0, 0.0, 0.0);
        EmitVertex();

        EndPrimitive();
    }
}