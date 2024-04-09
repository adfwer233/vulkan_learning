#pragma once

enum GeometrySurfaceType {
    TensorProductBezier
};

class GeometrySurface {
  public:
    virtual GeometrySurfaceType type() = 0;
};