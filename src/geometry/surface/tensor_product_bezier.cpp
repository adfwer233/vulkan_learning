#include "geometry/surface/tensor_product_bezier.hpp"
glm::vec3 TensorProductBezierSurface::evaluate(glm::vec2 &param) {
    if (control_points_.empty())
        throw std::runtime_error("empty control points");

    auto m = control_points_.size();
    auto n = control_points_[0].size();

    for (auto &item: control_points_)
        if (item.size() != n)
            throw std::runtime_error("control points data invalid");


}
