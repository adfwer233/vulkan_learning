#pragma once

#include "../quantity/vertex_vector_quantity.hpp"
#include "vkl/scene/vkl_model.hpp"

class VertexVectorQuantityToNormal {
public:
    static void visualize(VklModel &model, VertexVectorQuantity *quantity) {
        int n = quantity->data.rows();

        for (int i = 0; i < n; i++) {
            model.vertices_[i].normal = {quantity->data(i, 0), quantity->data(i, 1), quantity->data(i, 2)};
        }

        model.updateVertexBuffer();
    }
};