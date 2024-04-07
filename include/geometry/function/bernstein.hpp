#pragma once

#include <array>
#include <vector>
#include <glm/glm.hpp>

#include <geometry/autodiff/autodiff.hpp>

#include <boost/math/interpolators/bezier_polynomial.hpp>

class BernsteinBasisFunction {
  public:
    static glm::vec3 evaluate(double param, const std::vector<glm::vec3> &coefficients) {
        auto n = coefficients.size();
        double result = 0;

        using namespace boost::math::interpolators;

        std::vector<std::array<double, 3>> control_pts(coefficients.size());

        for (int i = 0; i < n; i++) {
            control_pts[i] = {coefficients[i].x, coefficients[i].y, coefficients[i].z};
        }

        auto bp = bezier_polynomial(std::move(control_pts));

        auto res = bp(param);

        return {res[0], res[1], res[2]};
    }

    static autodiff_vec3 evaluate_autodiff(autodiff::var param, std::vector<autodiff_vec3> &coefficients) {
        using namespace boost::math::interpolators;
        auto bp = bezier_polynomial(std::move(coefficients));
        auto res = bp(param);
        return res;
    }
};