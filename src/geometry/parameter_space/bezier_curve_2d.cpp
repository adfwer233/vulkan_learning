#include "geometry/parameter_space/bezier_curve_2d.hpp"

#include <set>

#define M_PI 3.14159265358979323846 // pi
#include "root_finder/root_finder.hpp"

std::tuple<float, float> BezierCurve2D::projection(glm::vec2 test_point)  {
    auto target_poly =
        (test_point.x - polynomial1) * polynomial1_deriv + (test_point.y - polynomial2) * polynomial2_deriv;
    auto n = target_poly.degree();

    Eigen::VectorXd coeffs;
    coeffs.resize(n + 1);

    for (int i = 0; i <= n; i++) {
        coeffs(i) = target_poly.data()[n - i];
    }

    std::set<double> roots;

    if (n == 1) {
        double sol = -coeffs[1] / coeffs[0];
        if (sol > 0 and sol < 1)
            roots.insert(sol);
    } else if (n == 3) {
        auto tmp_roots = RootFinderPriv::solveCub(coeffs[3], coeffs[2], coeffs[1], coeffs[0]);
        for (auto r : tmp_roots) {
            if (r > 0 and r < 1)
                roots.insert(r);
        }
    } else {
        roots = RootFinder::solvePolynomial(coeffs, 0.0, 1.0, 0.01);
    }

    float dist = 10;
    float param = -1;

    double dist_to_start = glm::length(glm::vec2{control_points_[0][1], control_points_[0][1]} - test_point);
    double dist_to_end = glm::length(glm::vec2{control_points_.back()[0], control_points_.back()[1]} - test_point);

    if (dist_to_start < dist_to_end) {
        param = 0;
        dist = dist_to_start;
    } else {
        param = 1;
        dist = dist_to_end;
    }

    for (auto r : roots) {
        auto eval = evaluate(r);
        auto new_dist = glm::length(eval - test_point);

        if (new_dist < dist) {
            dist = new_dist;
            param = r;
        }
    }

    if (param < 0)
        throw std::runtime_error("wrong root");

    return std::make_tuple(dist, param);
}