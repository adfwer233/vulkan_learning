#pragma once

#include "geometry/function/bernstein.hpp"
#include "glm/glm.hpp"
#include "vkl/utils/vkl_curve_model.hpp"
#include <vector>

class BezierCurve2D {
  private:
    std::vector<glm::vec2> control_points_;

  public:
    using parameter_space_render_type = VklCurveModel2D;
    using render_type = VklCurveModel3D;

    std::unique_ptr<parameter_space_render_type> parameter_space_mesh_model;

    /**
     * constructor
     * @param control_points right value reference
     */
    BezierCurve2D(decltype(control_points_) &&control_points) {
        control_points_ = std::move(control_points);
    }

    /**
     * evaluate the beizer curve with given parameter
     * @param param
     * @return vec2 in parameter space
     */
    glm::vec2 evaluate(double param) {
        auto res = BernsteinBasisFunction::evaluate(param, control_points_);
        return res;
    }

    /**
     * evaluate the bezier curve in the parameter space and generate renderable curve mesh
     * @param device
     * @return
     */
    parameter_space_render_type *get_parameter_space_mesh_model(VklDevice &device) {
        if (parameter_space_mesh_model) {
            return parameter_space_mesh_model.get();
        }

        parameter_space_render_type ::BuilderFromImmediateData builder;

        constexpr int n = 100;

        double param_delta = 1.0 / n;

        for (int i = 0; i <= n; i++) {
            parameter_space_render_type ::vertex_type vertex;
            auto position = evaluate(param_delta * i);
            vertex.position = position;
            builder.vertices.push_back(vertex);
        }

        parameter_space_mesh_model = std::make_unique<parameter_space_render_type>(device, builder);
        return parameter_space_mesh_model.get();
    }
};