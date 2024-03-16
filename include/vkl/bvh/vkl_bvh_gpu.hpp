#pragma once

#include "glm/glm.hpp"

namespace VklBVHGPUModel {
    enum MaterialType {
        LightSource,
        Lambertian,
        Metal,
        Glass
    };

    struct Material {
        MaterialType type;
        glm::vec3 albedo;
    };

    struct Triangle {
        glm::vec3 v0, v1, v2;
        uint32_t materialIndex;
    };

    struct BVHNode {
        glm::vec3 min, max;
        int leftNodeIndex, rightNodeIndex, objectIndex;
    };

    struct Light {
        uint32_t triangleIndex;
        float area;
    };
}