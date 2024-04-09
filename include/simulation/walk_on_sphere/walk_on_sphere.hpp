#pragma once

#include "geometry/surface/tensor_product_bezier.hpp"

class ParameterSpaceWalkOnSphere {
  private:
    TensorProductBezierSurface *targetSurface;

    double sdf_evaluate(glm::vec2 param);

    double boundary_evaluation(glm::vec2 param);

    double evaluate_internal(glm::vec2 param);

  public:
    double evaluate(glm::vec2 param);

    TensorProductBezierSurface::render_type::BuilderFromImmediateData getMeshModelBuilderWos();

    explicit ParameterSpaceWalkOnSphere(TensorProductBezierSurface *surface) : targetSurface(surface) {
    }
};