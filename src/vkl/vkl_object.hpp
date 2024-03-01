#pragma once

#include "vkl_model.hpp"

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

    void render_object();
};