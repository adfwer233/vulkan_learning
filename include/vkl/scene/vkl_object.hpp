#pragma once

#include "vkl/core/vkl_swap_chain.hpp"
#include "vkl_model.hpp"

#include "vkl/io/model_loader_concept.hpp"

#include "glm/gtc/quaternion.hpp"

class VklObject {
  private:
    VklDevice &device_;

  public:
    glm::mat4 getModelTransformation();

    glm::vec3 modelScaling{};
    glm::quat modelRotation{};
    glm::vec3 modelTranslation{};

    template <VklModelLoader Loader> struct ImportBuilder {
        std::string modelPath;
    };

    template <VklModelLoader Loader> explicit VklObject(VklDevice &device, ImportBuilder<Loader> builder);

    ~VklObject();

    std::vector<VklModel *> models;

    std::optional<std::string> dataFilePath;

    void allocDescriptorSets(VklDescriptorSetLayout &setLayout, VklDescriptorPool &pool);

    void allocDescriptorSets(VklDescriptorPool &pool);

    void render_object();

    int get_triangle_num();

    void setMaterial(int materialIndex) {
        for (auto model : models) {
            model->materialIndex = materialIndex;
        }
    }
};

#include "vkl_object.hpp.impl"