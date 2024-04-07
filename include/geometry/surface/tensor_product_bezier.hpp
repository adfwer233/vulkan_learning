#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "vkl/scene/vkl_model.hpp"

#include "geometry/autodiff/autodiff.hpp"
#include "surface.hpp"

/**
 * @brief Tensor Product Bezier Surface with automatic differentiation
 */
class TensorProductBezierSurface: GeometrySurface {
private:
    /**
     * control points of the tensor product bezier surface
     */
    std::vector<std::vector<glm::vec3>> control_points_;
public:

    TensorProductBezierSurface(decltype(control_points_) &control_pts): control_points_(control_pts) {}

    /**
     * evaluate the position with given parameter.
     * @param param vec2 in the parameter space.
     * @return position in the physical space.
     */
    glm::vec3 evaluate(glm::vec2 param);

    /**
     * evaluate the position, automatic differential version
     * @param param autodiff vec2, parameter
     */
    autodiff_vec3 evaluate_autodiff(autodiff_vec2 &param);

    /**
     * evaluate the metric tensor at given position
     * @param param
     * @return
     */
    autodiff_mat2 evaluate_metric_tensor_autodiff(autodiff_vec2 &param);

    /**
     * implementing the RenderableGeometry concept
     */
    struct IsRenderableGeometry{};
    using render_type = VklModel;
    render_type::BuilderFromImmediateData getMeshModelBuilder() {
        render_type::BuilderFromImmediateData builder;

        constexpr int n = 50;
        constexpr int m = 50;

        float delta_u = 1.0f / n;
        float delta_v = 1.0f / m;

        for (int i = 0; i <= m; i++) {
            for (int j = 0; j <= n; j++) {
                glm::vec2 param{delta_u * i, delta_v * j};
                auto position = evaluate(param);
                decltype(builder.vertices)::value_type vertex;
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

    virtual GeometrySurfaceType type() {
        return GeometrySurfaceType::TensorProductBezier;
    };

    std::unique_ptr<render_type> mesh_model_ptr;
};