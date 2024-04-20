#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

#include <memory>
#include <vector>

class GeometryCollection {
  public:
    std::vector<std::unique_ptr<GeometryCollection>> subCollections;

    glm::mat4 getTransformation();
    glm::mat4 applyTransformation();

    // transformations
    glm::vec3 modelScaling{};
    glm::quat modelRotation{};
    glm::vec3 modelTranslation{};
};