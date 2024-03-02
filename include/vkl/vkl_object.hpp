#pragma once

#include "vkl_model.hpp"
#include "vkl_swap_chain.hpp"

#include "glm/gtc/quaternion.hpp"

class VklObject {
  private:
    VklDevice &device_;

  public:
    glm::mat4 getModelTransformation();

    glm::vec3 modelScaling;
    glm::quat modelRotation;
    glm::vec3 modelTranslation;

    struct ImportBuilder {
        std::string modelPath;
    };

    explicit VklObject(VklDevice &device, ImportBuilder builder);

    ~VklObject();

    std::vector<VklModel *> models;

    void allocDescriptorSets(VklDescriptorSetLayout &setLayout, VklDescriptorPool &pool);

    void render_object();

    int get_triangle_num();
};