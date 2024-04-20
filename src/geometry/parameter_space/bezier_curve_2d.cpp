#include "geometry/parameter_space/bezier_curve_2d.hpp"

BezierCurve2D::parameter_space_render_type *BezierCurve2D::get_parameter_space_mesh_model(VklDevice &device) {
    if (parameter_space_mesh_model) {
        return parameter_space_mesh_model.get();
    }

    parameter_space_render_type::BuilderFromImmediateData builder;

    constexpr int n = 100;

    double param_delta = 1.0 / n;

    for (int i = 0; i <= n; i++) {
        parameter_space_render_type::vertex_type vertex;
        auto position = evaluate(param_delta * i);
        vertex.position = position;
        builder.vertices.push_back(vertex);
    }

    parameter_space_mesh_model = std::make_unique<parameter_space_render_type>(device, builder);
    return parameter_space_mesh_model.get();
}