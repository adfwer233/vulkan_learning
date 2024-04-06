#include "geometry/autodiff/autodiff.hpp"

#include "gtest/gtest.h"
#include "glm/glm.hpp"

#include "effolkronium/random.hpp"

using Random = effolkronium::random_static;

TEST(GeometryAutodiffTest, OperatorFuzzing) {
    for (int i = 0; i < 1000; i++) {
        glm::vec2 glm_vec1{Random::get(0.0f, 100.0f), Random::get(0.0f, 100.0f)};
        glm::vec2 glm_vec2{Random::get(0.0f, 100.0f), Random::get(0.0f, 100.0f)};

        autodiff_vec2 vec1(glm_vec1.x, glm_vec1.y);
        autodiff_vec2 vec2(glm_vec2.x, glm_vec2.y);

        auto add_res = vec1 + vec2;

        EXPECT_TRUE(abs(add_res.data[0] - glm_vec1.x - glm_vec2.x) < 1e-8);

        auto [vec1_x_deriv, vec2_y_deriv] =
            autodiff::derivatives(add_res.data[0], autodiff::wrt(vec1.data[0], vec2.data[0]));
        EXPECT_TRUE(abs(vec1_x_deriv - 1.0) < 1e-8);
    }
}