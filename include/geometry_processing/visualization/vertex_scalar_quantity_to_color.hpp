#pragma once

#include "../quantity/vertex_scalar_quantity.hpp"
#include "vkl/scene/vkl_model.hpp"

class VertexScalarQuantityToColor {
  public:
    static void visualize(VklModel &model, VertexScalarQuantity *quantity, glm::vec3 lowColor, glm::vec3 highColor) {
        int n = quantity->data.size();

        auto minValue = quantity->data.minCoeff();
        auto maxValue = quantity->data.maxCoeff();

        for (int i = 0; i < n; i++) {
            model.geometry->vertices[i].color =
                lowColor + (highColor - lowColor) * float((quantity->data[i] - minValue) / (maxValue - minValue));
        }

        model.updateVertexBuffer();
    }
};