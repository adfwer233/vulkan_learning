#pragma once

#include "vkl_model.hpp"
#include "vkl_swap_chain.hpp"

class VklObject {
  private:
    VklDevice &device_;

  public:
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