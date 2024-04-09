#include "geometry/surface/tensor_product_bezier.hpp"
#include "geometry/function/bernstein.hpp"
#include <iterator>

glm::vec3 TensorProductBezierSurface::evaluate(glm::vec2 param) {
    if (control_points_.empty())
        throw std::runtime_error("empty control points");

    auto m = control_points_.size();
    auto n = control_points_[0].size();

    for (auto &item : control_points_)
        if (item.size() != n)
            throw std::runtime_error("control points data invalid");

    std::vector<glm::vec3> outer_control_points;
    for (auto i = 0; i < m; i++) {
        std::vector<glm::vec3> inner_control_points;
        std::ranges::copy(control_points_[i], std::back_inserter(inner_control_points));

        outer_control_points.emplace_back(BernsteinBasisFunction::evaluate(param.y, inner_control_points));
    }

    return BernsteinBasisFunction::evaluate(param.x, outer_control_points);
}

glm::vec3 TensorProductBezierSurface::evaluate_ru(glm::vec2 param) {
    if (control_points_.empty())
        throw std::runtime_error("empty control points set");

    auto m = control_points_.size();
    auto n = control_points_[0].size();

    // compute r_u

    std::vector<glm::vec3> outer_control_points;
    for (auto i = 0; i < m; i++) {
        std::vector<glm::vec3> inner_control_points;
        std::ranges::copy(control_points_[i], std::back_inserter(inner_control_points));

        outer_control_points.emplace_back(BernsteinBasisFunction::evaluate(param.y, inner_control_points));
    }

    return BernsteinBasisFunction::evaluate_derivative(param.x, outer_control_points);
}

glm::mat2 TensorProductBezierSurface::evaluate_metric_tensor(glm::vec2 param) {
    // validation

    if (control_points_.empty())
        throw std::runtime_error("empty control points set");

    auto m = control_points_.size();
    auto n = control_points_[0].size();

    // compute r_u

    std::vector<glm::vec3> u_control_points;
    for (auto i = 0; i < m; i++) {
        std::vector<glm::vec3> inner_control_points;
        std::ranges::copy(control_points_[i], std::back_inserter(inner_control_points));
        u_control_points.emplace_back(BernsteinBasisFunction::evaluate(param.y, inner_control_points));
    }

    // derivative of r with respect to u
    auto ru = BernsteinBasisFunction::evaluate_derivative(param.x, u_control_points);

    // compute r_v

    std::vector<glm::vec3> v_control_points;
    for (auto i = 0; i < n; i++) {
        std::vector<glm::vec3> inner_control_points(m);
        for (int j = 0; j < m; j++)
            inner_control_points[j] = control_points_[j][i];
        v_control_points.emplace_back(BernsteinBasisFunction::evaluate(param.x, inner_control_points));
    }

    auto rv = BernsteinBasisFunction::evaluate_derivative(param.x, v_control_points);

    // build up the metric tensor

    glm::mat2 result;

    result[0][0] = glm::dot(ru, ru);
    result[0][1] = result[1][0] = glm::dot(ru, rv);
    result[1][1] = glm::dot(rv, rv);

    return result;
}

autodiff_mat2 TensorProductBezierSurface::evaluate_metric_tensor_autodiff(autodiff_vec2 &param) {
    // validation

    if (control_points_.empty())
        throw std::runtime_error("empty control points set");

    auto m = control_points_.size();
    auto n = control_points_.front().size();

    // compute r_u

    std::vector<autodiff_vec3> u_control_points;
    for (auto i = 0; i < m; i++) {
        std::vector<autodiff_vec3> inner_control_points(n);
        for (int j = 0; j < n; j++)
            inner_control_points[j] = control_points_[i][j];
        u_control_points.emplace_back(BernsteinBasisFunction::evaluate_autodiff(param.y(), inner_control_points));
    }

    // derivative of r with respect to u
    auto ru = BernsteinBasisFunction::evaluate_derivative_autodiff(param.x(), u_control_points);

    // compute r_v

    std::vector<autodiff_vec3> v_control_points;
    for (auto i = 0; i < n; i++) {
        std::vector<autodiff_vec3> inner_control_points(m);
        for (int j = 0; j < m; j++)
            inner_control_points[j] = control_points_[j][i];
        v_control_points.emplace_back(BernsteinBasisFunction::evaluate_autodiff(param.x(), inner_control_points));
    }

    auto rv = BernsteinBasisFunction::evaluate_derivative_autodiff(param.x(), v_control_points);

    // build up the metric tensor

    autodiff_mat2 result;

    result(0, 0) = GeometryAutodiff::dot(ru, ru);
    result(0, 1) = result(1, 0) = GeometryAutodiff::dot(ru, rv);
    result(1, 1) = GeometryAutodiff::dot(rv, rv);

    return std::move(result);
}

double TensorProductBezierSurface::evaluate_det_metric_tensor(glm::vec2 param) {
    auto metric_tensor = evaluate_metric_tensor(param);
    return metric_tensor[0][0] * metric_tensor[1][1] - metric_tensor[0][1] * metric_tensor[1][0];
}

autodiff::var TensorProductBezierSurface::evaluate_det_metric_tensor_autodiff(autodiff_vec2 &param) {
    auto metric_tensor = evaluate_metric_tensor_autodiff(param);
    return metric_tensor(0, 0) * metric_tensor(1, 1) - metric_tensor(0, 1) * metric_tensor(1, 0);
}

glm::mat2 TensorProductBezierSurface::evaluate_inverse_metric_tensor(glm::vec2 param) {
    auto metric_tensor = evaluate_metric_tensor(param);
    double det = metric_tensor[0][0] * metric_tensor[1][1] - metric_tensor[0][1] * metric_tensor[1][0];

    glm::mat2 result;

    result[0][0] = metric_tensor[1][1] / det;
    result[0][1] = -metric_tensor[1][0] / det;
    result[1][0] = metric_tensor[0][1] / det;
    result[1][1] = metric_tensor[0][0] / det;

    return result;
}

autodiff_mat2 TensorProductBezierSurface::evaluate_inverse_metric_tensor_autodiff(autodiff_vec2 &param) {

    auto metric_tensor = evaluate_metric_tensor_autodiff(param);
    autodiff::var det = metric_tensor(0, 0) * metric_tensor(1, 1) - metric_tensor(0, 1) * metric_tensor(1, 0);

    autodiff_mat2 result;

    result(0, 0) = metric_tensor(1, 1) / det;
    result(0, 1) = -metric_tensor(1, 0) / det;
    result(1, 0) = metric_tensor(0, 1) / det;
    result(1, 1) = metric_tensor(0, 0) / det;

    return result;
}
