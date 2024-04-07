#pragma once

#include <array>
#include <vector>
#include <glm/glm.hpp>

#include <geometry/autodiff/autodiff.hpp>

#include <boost/math/interpolators/bezier_polynomial.hpp>

class BernsteinBasisFunction {
  public:

    template<typename T>
    static T evaluate(double param, const std::vector<T> &coefficients);

    static autodiff_vec3 evaluate_autodiff(autodiff::var param, std::vector<autodiff_vec3> &coefficients) {
        using namespace boost::math::interpolators;
        auto bp = bezier_polynomial(std::move(coefficients));
        auto res = bp(param);
        return res;
    }
};

#include "bernstein.hpp.impl"