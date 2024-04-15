#pragma once

#include <array>
#include <glm/glm.hpp>
#include <ranges>
#include <vector>

#include <geometry/autodiff/autodiff.hpp>

#include <boost/math/interpolators/bezier_polynomial.hpp>

template <std::ranges::random_access_range RandomAccessContainer> class BernsteinPolynomial {
  public:
    using PointType = typename RandomAccessContainer::value_type;
    using Real = typename PointType::value_type;
    using SizeType = typename RandomAccessContainer::size_type;

    explicit BernsteinPolynomial(RandomAccessContainer &controlPoints);

    inline PointType operator()(Real t);

    inline PointType prime(Real t);

  private:
    RandomAccessContainer &controlPoints_;
    RandomAccessContainer storage;

    void decasteljau_recursion(RandomAccessContainer &points, SizeType n, Real t) const;
};

class BernsteinBasisFunction {
  public:
    template <size_t len>
    static std::array<float, len> evaluate(double param, std::vector<std::array<float, len>> &coefficients);

    template <size_t len>
    static std::array<float, len> evaluate_derivative(double param, std::vector<std::array<float, len>> &coefficients);

    static autodiff_vec3 evaluate_autodiff(autodiff::var param, std::vector<autodiff_vec3> &coefficients);

    static autodiff_vec3 evaluate_derivative_autodiff(autodiff::var param, std::vector<autodiff_vec3> &coefficients);
};

#include "bernstein.hpp.impl"