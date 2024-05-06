#pragma once

#include "geometry/function/bernstein.hpp"
#include "glm/glm.hpp"

#include <boost/math/tools/polynomial.hpp>

#include "Eigen/Eigen"

#include <vector>

#include <chrono>
#include <iostream>

class BezierCurve2D {
  private:
    std::vector<std::array<float, 2>> control_points_;

  public:
    std::vector<glm::vec2> control_point_vec2;

    float derivative_bound = -1.0f;

    using point_type = std::array<float, 2>;

    boost::math::tools::polynomial<float> polynomial1, polynomial2, polynomial1_deriv, polynomial2_deriv;

    /**
     * constructor
     * @param control_points right value reference
     */
    BezierCurve2D(decltype(control_points_) &&control_points) {
        control_points_ = std::move(control_points);

        initialize();
    }

    /**
     * evaluate the beizer curve with given parameter
     * @param param
     * @return vec2 in parameter space
     */
    glm::vec2 evaluate(double param) {
        auto res = BernsteinBasisFunction::evaluate(param, control_points_);
        return {res[0], res[1]};
    }

    glm::vec2 evaluate_polynomial(float param) const;

    std::tuple<float, float> projection(glm::vec2 test_point);

    float winding_number(glm::vec2 test_point);

    float winding_number_internal(glm::vec2 test_point, glm::vec2 start_pos, glm::vec2 end_pos, float start, float end,
                                  float derivative_bound);

    float winding_number_bi_periodic_internal(glm::vec2 test_point, glm::vec2 start_pos, glm::vec2 end_pos, float start,
                                              float end, float derivative_bound);

    float winding_number_u_periodic_internal(glm::vec2 test_point, glm::vec2 start_pos, glm::vec2 end_pos, float start,
                                             float end, float derivative_bound);

    void add_control_point(std::array<float, 2>);

private:
    void initialize();
};