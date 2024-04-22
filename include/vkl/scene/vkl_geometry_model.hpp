#pragma once

#include <vector>
#include "vkl_model.hpp"
#include "vkl/utils/vkl_curve_model.hpp"
#include "geometry/surface/tensor_product_bezier.hpp"

template<typename T>
class VklGeometryModel {};

template<> class VklGeometryModel<TensorProductBezierSurface> {
    using boundary_render_type = VklCurveModel3D;
    using parameter_render_type = VklCurveModel2D;

    std::vector<std::unique_ptr<boundary_render_type>> boundary_3d;
    std::vector<std::unique_ptr<parameter_render_type>> boundary_2d;

    VklGeometryModel<TensorProductBezierSurface>(TensorProductBezierSurface *surf) {

    }
};