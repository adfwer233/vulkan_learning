#pragma once

#include <array>
#include <glm/glm.hpp>
#include <vector>

#include <geometry/autodiff/autodiff.hpp>

#include <boost/math/interpolators/bezier_polynomial.hpp>

class BernsteinBasisFunction {
  public:
    template <typename T> static T evaluate(double param, const std::vector<T> &coefficients);

    template <typename VecType>
    static VecType evaluate_derivative(double param, const std::vector<VecType> &coefficients);

    static autodiff_vec3 evaluate_autodiff(autodiff::var param, std::vector<autodiff_vec3> &coefficients);

    static autodiff_vec3 evaluate_derivative_autodiff(autodiff::var param, std::vector<autodiff_vec3> &coefficients);
};

#include "bernstein.hpp.impl"