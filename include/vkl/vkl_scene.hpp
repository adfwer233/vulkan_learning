#pragma once

#include <vector>

#include "vkl_object.hpp"

class VklScene {
  private:
    VklDevice &device_;

  public:
    VklScene(VklDevice &device, glm::vec3 camera_pos, glm::vec3 camera_up)
        : device_(device), camera(camera_pos, camera_up) {
        pointLight.position = {0, 0, 10, 0};
        pointLight.color = {1, 1, 1, 0};
    };

    std::vector<std::unique_ptr<VklObject>> objects;
    Camera camera;
    PointLight pointLight;

    void addObject(VklObject::ImportBuilder builder);
};