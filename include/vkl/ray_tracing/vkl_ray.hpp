#pragma once

#include "glm/glm.hpp"

class VklRay {
public:
    glm::vec3 base;
    glm::vec3 dir;

    VklRay() {}

    VklRay(const glm::vec3 &t_base, const glm::vec3 &t_dir) : base(t_base), dir(t_dir) {}

    glm::vec3 at(float t) const {
        return base + dir * t;
    };
};