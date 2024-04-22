#include "geometry/surface/tensor_product_bezier.hpp"
#include "geometry/function/bernstein.hpp"
#include <iterator>

#include "vkl/utils/vkl_curve_model.hpp"

glm::vec3 TensorProductBezierSurface::evaluate(glm::vec2 param) {
    if (control_points_.empty())
        throw std::runtime_error("empty control points");

    auto m = control_points_.size();
    auto n = control_points_[0].size();

    for (auto &item : control_points_)
        if (item.size() != n)
            throw std::runtime_error("control points data invalid");

    std::vector<std::array<float, 3>> outer_control_points;
    for (auto i = 0; i < m; i++) {
        std::vector<std::array<float, 3>> inner_control_points;
        std::ranges::copy(control_points_[i], std::back_inserter(inner_control_points));

        outer_control_points.push_back(BernsteinBasisFunction::evaluate(param.y, inner_control_points));
    }

    auto res = BernsteinBasisFunction::evaluate(param.x, outer_control_points);

    return {res[0], res[1], res[2]};
}

glm::vec3 TensorProductBezierSurface::evaluate_ru(glm::vec2 param) {
    if (control_points_.empty())
        throw std::runtime_error("empty control points set");

    auto m = control_points_.size();
    auto n = control_points_[0].size();

    // compute r_u

    std::vector<std::array<float, 3>> outer_control_points;
    for (auto i = 0; i < m; i++) {
        std::vector<std::array<float, 3>> inner_control_points;
        std::ranges::copy(control_points_[i], std::back_inserter(inner_control_points));

        outer_control_points.emplace_back(BernsteinBasisFunction::evaluate(param.y, inner_control_points));
    }

    auto res = BernsteinBasisFunction::evaluate_derivative(param.x, outer_control_points);
    return {res[0], res[1], res[2]};
}

glm::mat2 TensorProductBezierSurface::evaluate_metric_tensor(glm::vec2 param) {
    // validation

    if (control_points_.empty())
        throw std::runtime_error("empty control points set");

    auto m = control_points_.size();
    auto n = control_points_[0].size();

    // compute r_u

    std::vector<std::array<float, 3>> u_control_points;
    for (auto i = 0; i < m; i++) {
        std::vector<std::array<float, 3>> inner_control_points;
        std::ranges::copy(control_points_[i], std::back_inserter(inner_control_points));
        u_control_points.emplace_back(BernsteinBasisFunction::evaluate(param.y, inner_control_points));
    }

    // derivative of r with respect to u
    auto ru_arr = BernsteinBasisFunction::evaluate_derivative(param.x, u_control_points);
    glm::vec3 ru = {ru_arr[0], ru_arr[1], ru_arr[1]};
    // compute r_v

    std::vector<std::array<float, 3>> v_control_points;
    for (auto i = 0; i < n; i++) {
        std::vector<std::array<float, 3>> inner_control_points(m);
        for (int j = 0; j < m; j++)
            inner_control_points[j] = control_points_[j][i];
        v_control_points.emplace_back(BernsteinBasisFunction::evaluate(param.x, inner_control_points));
    }

    auto rv_arr = BernsteinBasisFunction::evaluate_derivative(param.y, v_control_points);
    glm::vec3 rv = {rv_arr[0], rv_arr[1], rv_arr[2]};
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
            inner_control_points[j] =
                autodiff_vec3{control_points_[i][j][0], control_points_[i][j][1], control_points_[i][j][2]};
        u_control_points.emplace_back(BernsteinBasisFunction::evaluate_autodiff(param.y(), inner_control_points));
    }

    // derivative of r with respect to u
    auto ru = BernsteinBasisFunction::evaluate_derivative_autodiff(param.x(), u_control_points);

    // compute r_v

    std::vector<autodiff_vec3> v_control_points;
    for (auto i = 0; i < n; i++) {
        std::vector<autodiff_vec3> inner_control_points(m);
        for (int j = 0; j < m; j++)
            inner_control_points[j] =
                autodiff_vec3{control_points_[j][i][0], control_points_[j][i][1], control_points_[j][i][2]};
        v_control_points.emplace_back(BernsteinBasisFunction::evaluate_autodiff(param.x(), inner_control_points));
    }

    auto rv = BernsteinBasisFunction::evaluate_derivative_autodiff(param.y(), v_control_points);

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
    result[0][1] = -metric_tensor[0][1] / det;
    result[1][0] = -metric_tensor[1][0] / det;
    result[1][1] = metric_tensor[0][0] / det;

    return result;
}

autodiff_mat2 TensorProductBezierSurface::evaluate_inverse_metric_tensor_autodiff(autodiff_vec2 &param) {

    auto metric_tensor = evaluate_metric_tensor_autodiff(param);
    autodiff::var det = metric_tensor(0, 0) * metric_tensor(1, 1) - metric_tensor(0, 1) * metric_tensor(1, 0);

    autodiff_mat2 result;

    result(0, 0) = metric_tensor(1, 1) / det;
    result(0, 1) = -metric_tensor(0, 1) / det;
    result(1, 0) = -metric_tensor(1, 0) / det;
    result(1, 1) = metric_tensor(0, 0) / det;

    return result;
}

glm::mat2 TensorProductBezierSurface::evaluate_laplacian_diffusion_coefficients(glm::vec2 param) {
    return evaluate_inverse_metric_tensor(param);
}

glm::vec2 TensorProductBezierSurface::evaluate_laplacian_drift_coefficients(glm::vec2 param) {
    // autodiff_vec2 param_autodiff(param.x, param.y);
    //
    // auto metric_inverse = evaluate_inverse_metric_tensor_autodiff(param_autodiff);
    // auto det = evaluate_det_metric_tensor_autodiff(param_autodiff);
    //
    // autodiff::var det_sqrt = autodiff::reverse::detail::sqrt(det);
    //
    // autodiff::var a11 = det_sqrt * metric_inverse(0, 0);
    // autodiff::var a12 = det_sqrt * metric_inverse(0, 1);
    // autodiff::var a21 = det_sqrt * metric_inverse(1, 0);
    // autodiff::var a22 = det_sqrt * metric_inverse(1, 1);
    //
    // auto [b1_0] = autodiff::reverse::detail::derivativesx(a11, autodiff::wrt(param_autodiff.x()));
    // auto [b1_1] = autodiff::reverse::detail::derivativesx(a12, autodiff::wrt(param_autodiff.y()));
    // auto [b2_0] = autodiff::reverse::detail::derivativesx(a21, autodiff::wrt(param_autodiff.x()));
    // auto [b2_1] = autodiff::reverse::detail::derivativesx(a22, autodiff::wrt(param_autodiff.y()));
    //
    // autodiff::var b1 = ((b1_0 + b1_1) / det_sqrt);
    // autodiff::var b2 = ((b2_0 + b2_1) / det_sqrt);

    auto res = laplacianEvaluator->evaluate(param);

    return res;
}

void TensorProductBezierSurface::initializeBoundary() {
    // the boundary of parameter space

    std::vector<BezierCurve2D::point_type> default_boundary1{BezierCurve2D::point_type{0.0, 0.0},
                                                             BezierCurve2D::point_type{1.0, 0.0}};
    boundary_curves.push_back(std::move(std::make_unique<BezierCurve2D>(std::move(default_boundary1))));

    std::vector<BezierCurve2D::point_type> default_boundary2{BezierCurve2D::point_type{1.0, 0.0},
                                                             BezierCurve2D::point_type{1.0, 1.0}};
    boundary_curves.push_back(std::move(std::make_unique<BezierCurve2D>(std::move(default_boundary2))));

    std::vector<BezierCurve2D::point_type> default_boundary3{BezierCurve2D::point_type{1.0, 1.0},
                                                             BezierCurve2D::point_type{0.0, 1.0}};
    boundary_curves.push_back(std::move(std::make_unique<BezierCurve2D>(std::move(default_boundary3))));

    std::vector<BezierCurve2D::point_type> default_boundary4{BezierCurve2D::point_type{0.0, 1.0},
                                                             BezierCurve2D::point_type{0.0, 0.0}};
    boundary_curves.push_back(std::move(std::make_unique<BezierCurve2D>(std::move(default_boundary4))));

    // custom boundary

    std::vector<BezierCurve2D::point_type> circle1_boundary_1{{0.3, 0.5}, {0.3, 0.7}, {0.5, 0.7}};
    std::vector<BezierCurve2D::point_type> circle1_boundary_2{{0.5, 0.7}, {0.7, 0.7}, {0.7, 0.5}};
    std::vector<BezierCurve2D::point_type> circle1_boundary_3{{0.7, 0.5}, {0.7, 0.3}, {0.5, 0.3}};
    std::vector<BezierCurve2D::point_type> circle1_boundary_4{{0.5, 0.3}, {0.3, 0.3}, {0.3, 0.5}};

    boundary_curves.push_back(std::move(std::make_unique<BezierCurve2D>(std::move(circle1_boundary_1))));
    boundary_curves.push_back(std::move(std::make_unique<BezierCurve2D>(std::move(circle1_boundary_2))));
    boundary_curves.push_back(std::move(std::make_unique<BezierCurve2D>(std::move(circle1_boundary_3))));
    boundary_curves.push_back(std::move(std::make_unique<BezierCurve2D>(std::move(circle1_boundary_4))));
}

MeshModelTemplate<Vertex3D, TriangleIndex> TensorProductBezierSurface::getMeshModelBuilder() {
    render_type builder;

    constexpr int n = 50;
    constexpr int m = 50;

    float delta_u = 1.0f / n;
    float delta_v = 1.0f / m;

    for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n; j++) {
            glm::vec2 param{delta_u * i, delta_v * j};
            auto position = evaluate(param);
            decltype(builder.vertices)::value_type vertex;

            if (containment_test(param)) {
                vertex.color = {1.0f, 0.0f, 0.0f};
            } else {
                std::cout << "out " << param.x << ' ' << param.y << std::endl;
            }

            vertex.position = position;
            builder.vertices.push_back(vertex);
        }
    }

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            auto idx1 = i * (n + 1) + j;
            auto idx2 = i * (n + 1) + j + 1;
            auto idx3 = (i + 1) * (n + 1) + j;
            auto idx4 = (i + 1) * (n + 1) + j + 1;

            decltype(builder.indices)::value_type primitive_idx1, primitive_idx2;
            primitive_idx1.i = idx1;
            primitive_idx1.j = idx2;
            primitive_idx1.k = idx4;

            primitive_idx2.i = idx1;
            primitive_idx2.j = idx4;
            primitive_idx2.k = idx3;

            builder.indices.push_back(primitive_idx1);
            builder.indices.push_back(primitive_idx2);
        }
    }

    return builder;
}
bool TensorProductBezierSurface::containment_test(glm::vec2 test_param) {

    float winding_number = 0.0;

    for (const auto & boundary_curve : boundary_curves) {
        winding_number += boundary_curve->winding_number(test_param);
    }

    std::cout << "Test " << winding_number << std::endl;
    return winding_number > 3.14;
}
