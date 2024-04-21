#pragma once

#include "geometry/function/bernstein.hpp"
#include "glm/glm.hpp"

#include "boost/math/tools/polynomial.hpp"

#include "Eigen/Eigen"

#define M_PI 3.14159265358979323846 // pi
#include "root_finder/root_finder.hpp"

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

    std::tuple<float, float> projection(glm::vec2 test_point) {
        auto target_poly =
            (test_point.x - polynomial1) * polynomial1_deriv + (test_point.y - polynomial2) * polynomial2_deriv;
        auto n = target_poly.degree();

        Eigen::VectorXd coeffs;
        coeffs.resize(n + 1);

        for (int i = 0; i <= n; i++) {
            coeffs(i) = target_poly.data()[n - i];
        }

        std::set<double> roots;

        if (n == 1) {
            double sol = -coeffs[1] / coeffs[0];
            if (sol > 0 and sol < 1)
                roots.insert(sol);
        } else if (n == 3) {
            auto tmp_roots = RootFinderPriv::solveCub(coeffs[3], coeffs[2], coeffs[1], coeffs[0]);
            for (auto r : tmp_roots) {
                if (r > 0 and r < 1)
                    roots.insert(r);
            }
        } else {
            roots = RootFinder::solvePolynomial(coeffs, 0.0, 1.0, 0.01);
        }

        float dist = 10;
        float param = -1;

        double dist_to_start = glm::length(glm::vec2{control_points_[0][1], control_points_[0][1]} - test_point);
        double dist_to_end = glm::length(glm::vec2{control_points_.back()[0], control_points_.back()[1]} - test_point);

        if (dist_to_start < dist_to_end) {
            param = 0;
            dist = dist_to_start;
        } else {
            param = 1;
            dist = dist_to_end;
        }

        for (auto r : roots) {
            auto eval = evaluate(r);
            auto new_dist = glm::length(eval - test_point);

            if (new_dist < dist) {
                dist = new_dist;
                param = r;
            }
        }

        if (param < 0)
            throw std::runtime_error("wrong root");

        return std::make_tuple(dist, param);
    }
};