#pragma once

#include "vkl/vkl_object.hpp"
#include "ray.hpp"
#include <vector>
#include <optional>
class RayPicker {
private:
    std::vector<VklObject*> objects_;
    Ray ray_;
public:

    RayPicker(std::vector<VklObject*> &object, Ray ray);

    struct RayPickingResult {
        size_t object_index;
        size_t model_index;
        size_t face_index;
        float param;
        float u, v, w;
    };

    std::optional<RayPickingResult> trace();
};
