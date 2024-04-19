#pragma once

#include "glm/glm.hpp"

struct Vertex2D {
    alignas(8) glm::vec2 position{};
    glm::vec3 color{};
    glm::vec3 normal{};
    glm::vec2 uv{};
};

struct LineIndex {
    uint32_t i, j, k;
    static constexpr size_t vertexCount = 3;
};
