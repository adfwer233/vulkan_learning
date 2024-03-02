#pragma once

#include "vkl/vkl_object.hpp"
#include "ray.hpp"
#include <vector>
#include <optional>
class RayTracer {
private:
    std::vector<VklObject*> objects_;
    Ray ray_;
public:

    RayTracer(std::vector<VklObject*> &object, Ray ray);

    struct RayTracingResult {
        size_t object_index;
        size_t model_index;
        size_t face_index;
        float param;
        float u, v, w;
    };

    std::optional<RayTracingResult> trace();
};
