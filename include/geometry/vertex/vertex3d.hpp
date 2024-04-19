#pragma once

#include "glm/glm.hpp"

struct Vertex3D {
    glm::vec3 position{};
    glm::vec3 color{};
    glm::vec3 normal{};
    glm::vec2 uv{};
};

struct TriangleIndex {
    uint32_t i, j, k;
    static constexpr size_t vertexCount = 3;
};
