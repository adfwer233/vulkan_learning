#pragma once

#include <vector>
#include <memory>

class GeometryCollection {
  public:
    std::vector<std::unique_ptr<GeometryCollection>> subCollections;

    // reflection
};