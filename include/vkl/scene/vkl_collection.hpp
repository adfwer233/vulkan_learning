#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

#include <vector>
#include <memory>

#include "vkl_model.hpp"

class VklCollection {

  public:
    std::vector<std::unique_ptr<VklCollection>> subCollections;

    std::vector<std::unique_ptr<VklModel>> models;

    glm::mat4 getModelTransformation();

    glm::mat4 applyModelTransformation();

    glm::vec3 modelScaling{};
    glm::quat modelRotation{};
    glm::vec3 modelTranslation{};

    void setMaterial();
};