#pragma once

#include "geometry/function/bernstein.hpp"
#include "glm/glm.hpp"

#include <boost/math/tools/polynomial.hpp>

#include "Eigen/Eigen"

#include <vector>

#include <chrono>
#include <iostream>

class BezierCurve2D {
  private:
    std::vector<std::array<float, 2>> control_points_;

  public:
    using point_type = std::array<float, 2>;

    boost::math::tools::polynomial<float> polynomial1, polynomial2, polynomial1_deriv, polynomial2_deriv;

    /**
     * constructor
     * @param control_points right value reference
     */
    BezierCurve2D(decltype(control_points_) &&control_points) {
        control_points_ = std::move(control_points);

        // computing the polynomial coefficients

        auto n = control_points_.size() - 1;

        std::vector<float> polynomial_coeff1, polynomial_coeff2;

        for (int j = 0; j <= n; j++) {
            if (j == 0) {
                polynomial_coeff1.push_back(control_points_[0][0]);
                polynomial_coeff2.push_back(control_points_[0][1]);
                continue;
            }
            float res1{1.0f};
            float res2{1.0f};
            for (int m = 0; m < j; m++) {
                float tmp1{0.0f};
                float tmp2{0.0f};
                for (int i = 0; i <= j; i++) {
                    tmp1 += control_points_[i][0] * std::pow(-1, i + j);
                    tmp2 += control_points_[i][1] * std::pow(-1, i + j);
                }
                res1 *= tmp1 * (1.0f * n - 1.0f * m);
                res2 *= tmp2 * (1.0f * n - 1.0f * m);
            }
            polynomial_coeff1.push_back(res1);
            polynomial_coeff2.push_back(res2);
        }

        polynomial1 = boost::math::tools::polynomial<float>(polynomial_coeff1.cbegin(), polynomial_coeff1.cend());
        polynomial2 = boost::math::tools::polynomial<float>(polynomial_coeff2.cbegin(), polynomial_coeff2.cend());

        polynomial1_deriv = polynomial1.prime();
        polynomial2_deriv = polynomial2.prime();
    }

    /**
     * evaluate the beizer curve with given parameter
     * @param param
     * @return vec2 in parameter space
     */
    glm::vec2 evaluate(double param) {
        auto res = BernsteinBasisFunction::evaluate(param, control_points_);
        return {res[0], res[1]};
    }

    std::tuple<float, float> projection(glm::vec2 test_point);
};