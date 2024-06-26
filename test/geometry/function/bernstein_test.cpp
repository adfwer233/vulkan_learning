#include "gtest/gtest.h"
#include "geometry/function/bernstein.hpp"

TEST(BernsteinBasisFunctionTest, Evaluate) {
    for (int i = 0; i < 10; i++) {
        std::vector<std::array<float, 3>> control_pts {
            {0.0, 0.0, 0.0},
            {1.0, 0.0, 0.0},
            {1.0, 1.0, 0.0},
            {2.0, 1.0, 0.0}
        };

        auto res = BernsteinBasisFunction::evaluate(i * 0.1, control_pts);
    }
}

TEST(BernsteinBasisFunctionTest, AutodiffEvaluate) {
    for (int i = 0; i < 10; i++) {
        std::vector<autodiff_vec3> control_pts {
            autodiff_vec3{0.0, 0.0, 0.0},
            autodiff_vec3{1.0, 0.0, 0.0},
            autodiff_vec3{1.0, 1.0, 0.0},
            autodiff_vec3{2.0, 1.0, 0.0}
        };

        autodiff::var param = i * 0.1;
        auto res = BernsteinBasisFunction::evaluate_autodiff(param, control_pts);
        auto [prime_x] = autodiff::derivatives(res[0], autodiff::wrt(param));
        std::cout << prime_x << std::endl;
        std::cout <<  autodiff::reverse::detail::expr_value(res[0]) << ' ' << res[1] << ' ' << res[2] << std::endl;
    }
}