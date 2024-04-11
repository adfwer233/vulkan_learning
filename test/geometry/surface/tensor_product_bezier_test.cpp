#include "gtest/gtest.h"
#include "geometry/surface/tensor_product_bezier.hpp"
#include "geometry/function/bernstein.hpp"

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

    surface.evaluate_ru(glm::vec2 {0.5f, 0.5f});
}

TEST(TensorProductTest, MetricTensor) {
    std::vector<std::vector<glm::vec3>> control_points {
            {{-1.0, 0.0, -1.0}, {-1.0, 1.0, 0.0}, {-1.0, 0.0, 1.0}},
            {{0.0, 0.5, -1.0}, {0.0, 1.5, 0.0}, {0.0, 0.5, 1.0}},
            {{1.0, 0.0, -1.0}, {1.0, 1.0, 0.0}, {1.0, 0.0, 1.0}}
    };

    TensorProductBezierSurface surface(std::move(control_points));

    for (int i = 0; i < 10; i++) {
        glm::vec2 param(i * 0.1f, i * 0.1f);
        auto pos = surface.evaluate(param);
        auto metric_inverse = surface.evaluate_inverse_metric_tensor(param);
        std::cout << std::format("{} {} {} {} \n", metric_inverse[0][0], metric_inverse[0][1], metric_inverse[1][0], metric_inverse[1][1]);
    }
}

TEST(TensorProductTest, MetricTensorAutodiff) {
    std::vector<std::vector<glm::vec3>> control_points {
            {{-1.0, 0.0, -1.0}, {-1.0, 1.0, 0.0}, {-1.0, 0.0, 1.0}},
            {{0.0, 0.5, -1.0}, {0.0, 1.5, 0.0}, {0.0, 0.5, 1.0}},
            {{1.0, 0.0, -1.0}, {1.0, 1.0, 0.0}, {1.0, 0.0, 1.0}}
    };

    TensorProductBezierSurface surface(std::move(control_points));

    for (int i = 0; i < 10; i++) {
        autodiff_vec2 param(i * 0.1f, i * 0.1f);
        // auto res = surface.evaluate(glm::vec2 {0.5f, i * 0.1f});
        auto metric_tensor = surface.evaluate_metric_tensor_autodiff(param);
        auto metric_inverse = surface.evaluate_inverse_metric_tensor_autodiff(param);
        auto det = surface.evaluate_det_metric_tensor_autodiff(param);

        autodiff::var det_sqrt = autodiff::reverse::detail::sqrt(det);

        autodiff::var a11 = det_sqrt * metric_inverse(0, 0);
        autodiff::var a12 = det_sqrt * metric_inverse(0, 1);
        autodiff::var a21 = det_sqrt * metric_inverse(1, 0);
        autodiff::var a22 = det_sqrt * metric_inverse(1, 1);

        auto [b1_0] = autodiff::reverse::detail::derivativesx(a11, autodiff::wrt(param.x()));
        auto [b1_1] = autodiff::reverse::detail::derivativesx(a12, autodiff::wrt(param.y()));
        auto [b2_0] = autodiff::reverse::detail::derivativesx(a21, autodiff::wrt(param.x()));
        auto [b2_1] = autodiff::reverse::detail::derivativesx(a22, autodiff::wrt(param.y()));

        autodiff::var b1 = ((b1_0 + b1_1) / det_sqrt) / autodiff::reverse::detail::sqrt(metric_tensor(0, 0));
        autodiff::var b2 = ((b2_0 + b2_1) / det_sqrt - metric_tensor(1, 0)) / autodiff::reverse::detail::sqrt(metric_tensor(1, 1));

        auto [b1_y] = autodiff::derivatives(b1, autodiff::wrt(param.y()));
        auto [b2_x] = autodiff::derivatives(b2, autodiff::wrt(param.x()));

        std::cout << "curl b " << b1_y << ' ' << b2_x << std::endl;

        // std::cout << std::format("{} {} {} {} \n", b1_0, b1_1, b2_0, b2_1);
        std::cout << std::format("{} {} {} {} \n", double(metric_inverse(0, 0)),
                                 double((metric_inverse(0, 1))), double(metric_inverse(1, 0)),
                                 double(metric_inverse(1, 1)));

        std::cout << b1 / det_sqrt << ' ' << b2 / det_sqrt << std::endl;
    }
}