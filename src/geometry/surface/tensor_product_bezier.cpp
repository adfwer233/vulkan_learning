#include "geometry/surface/tensor_product_bezier.hpp"
#include "geometry/function/bernstein.hpp"
#include <iterator>

glm::vec3 TensorProductBezierSurface::evaluate(glm::vec2 param) {
    if (control_points_.empty())
        throw std::runtime_error("empty control points");

    auto m = control_points_.size();
    auto n = control_points_[0].size();

    for (auto &item: control_points_)
        if (item.size() != n)
            throw std::runtime_error("control points data invalid");

    std::vector<glm::vec3> outer_control_points;
    for (auto i = 0; i < m; i++) {
        std::vector<glm::vec3> inner_control_points;
        std::ranges::copy(control_points_[i], std::back_inserter(inner_control_points));

        outer_control_points.emplace_back(BernsteinBasisFunction::evaluate(param.y, std::move(inner_control_points)));
    }

    return BernsteinBasisFunction::evaluate(param.x, std::move(outer_control_points));
}
