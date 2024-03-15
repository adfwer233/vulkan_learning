#pragma once

#include "vkl_concept.hpp"

struct TriangleIndex{
    uint32_t i, j, k;
    static uint32_t vertexCount() {return 3;}
};

struct LineIndex {
    uint32_t i, j;
    static uint32_t vertexCount() {return 2;}
};

template <typename T>
concept VklIndexType = IsAnyOf<T, TriangleIndex, LineIndex> && requires {
T::vertexCount();
};