#include "gtest/gtest.h"
#include "geometry/surface/tensor_product_bezier.hpp"

#include <format>

TEST(TensorProductTest, Evaluate) {
    std::vector<std::vector<glm::vec3>> control_points {
            {{-1.0, 0.0, -1.0}, {-1.0, 1.0, 0.0}, {-1.0, 0.0, 1.0}},
            {{0.0, 0.5, -1.0}, {0.0, 1.5, 0.0}, {0.0, 0.5, 1.0}},
            {{1.0, 0.0, -1.0}, {1.0, 1.0, 0.0}, {1.0, 0.0, 1.0}}
    };

    TensorProductBezierSurface surface(std::move(control_points));

    for (int i = 0; i < 10; i++) {
        auto res = surface.evaluate(glm::vec2 {0.5f, i * 0.1f});
        std::cout << std::format("{} {} {} \n", res.x, res.y, res.z);
    }
}