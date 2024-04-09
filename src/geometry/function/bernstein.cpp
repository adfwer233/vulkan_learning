#include "geometry/function/bernstein.hpp"

autodiff_vec3 BernsteinBasisFunction::evaluate_autodiff(autodiff::var param, std::vector<autodiff_vec3> &coefficients) {
    using namespace boost::math::interpolators;
    auto bp = bezier_polynomial(std::move(coefficients));
    auto res = bp(param);
    return res;
}

autodiff_vec3 BernsteinBasisFunction::evaluate_derivative_autodiff(autodiff::var param, std::vector<autodiff_vec3> &coefficients) {
    using namespace boost::math::interpolators;
    auto bp = bezier_polynomial(std::move(coefficients));
    auto res = bp.prime(param);
    return res;
}