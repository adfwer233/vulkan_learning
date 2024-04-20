#pragma once

// #include "../renderable_geometry.hpp"

enum GeometrySurfaceType {
    TensorProductBezier
};

class GeometrySurface {
  public:
    virtual GeometrySurfaceType type() = 0;
};