#pragma once

#include "ray.hpp"
#include "vkl/scene/vkl_object.hpp"
#include "vkl/scene/vkl_scene.hpp"

#include <optional>
#include <vector>
class RayPicker {
  private:
    VklScene &scene_;
    Ray ray_;

  public:
    RayPicker(VklScene &scene, Ray ray);

    struct RayPickingResult {
        size_t object_index;
        size_t model_index;
        size_t face_index;
        size_t vertex_index;
        float param;
        float u, v, w;
    };

    std::optional<RayPickingResult> trace();
};
