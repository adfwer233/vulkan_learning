#include "geometry/loop/path_2d.hpp"

Path2D::Path2D(std::vector<std::vector<std::array<float, 2>>> &&path_data) {
    for (auto vec : path_data) {
        curves.push_back(std::move(std::make_unique<BezierCurve2D>(std::move(vec))));
        derivative_bound = std::max(derivative_bound, curves.back()->derivative_bound);
    }
}

float Path2D::winding_number(glm::vec2 test_point) {
    return winding_number_internal(test_point, 0, curves.size() - 1);
}

float Path2D::winding_number_internal(glm::vec2 test_point, size_t start_index, size_t end_index) {

    constexpr bool enable_path_optim = true;

    if (not enable_path_optim) {
        float wn = 0;
        for (auto &curve : curves) {
            wn += curve->winding_number(test_point);
        }
        return wn;
    }

    if (start_index >= end_index) {
        return curves[end_index]->winding_number(test_point);
    }

    auto &start_pos = curves[start_index]->control_point_vec2.front();
    auto &end_pos = curves[end_index]->control_point_vec2.back();

    auto d1 = glm::length(start_pos - test_point);
    auto d2 = glm::length(end_pos - test_point);

    if (d1 < 1e-6 or d2 < 1e-6)
        return 0;

    if (d1 + d2 > derivative_bound * (end_index - start_index + 1)) {
        auto v1 = glm::normalize(start_pos - test_point);
        auto v2 = glm::normalize(end_pos - test_point);
        auto outer = v1.x * v2.y - v1.y * v2.x;
        auto inner = glm::dot(v1, v2);

        if (std::isnan(std::acos(inner))) {
            return 0;
        }

        return outer > 0 ? std::acos(inner) : -std::acos(inner);
    }

    auto mid = (start_index + end_index) >> 1;

    return winding_number_internal(test_point, start_index, mid) +
           winding_number_internal(test_point, mid + 1, end_index);
};