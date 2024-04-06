#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "vkl/scene/vkl_model.hpp"

#include "geometry/autodiff/autodiff.hpp"

/**
 * @brief Tensor Product Bezier Surface with automatic differentiation
 */
class TensorProductBezierSurface {
private:
    /**
     * control points of the tensor product bezier surface
     */
    std::vector<std::vector<glm::vec3>> control_points_;

public:

    /**
     * evaluate the position with given parameter.
     * @param param vec2 in the parameter space.
     * @return position in the physical space.
     */
    glm::vec3 evaluate(glm::vec2 &param);

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
    render_type* getMeshModel();
};