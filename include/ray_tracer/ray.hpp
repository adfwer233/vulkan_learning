#pragma once

#include "glm/glm.hpp"

class Ray {
public:
    glm::vec3 base;
    glm::vec3 dir;

    Ray() {}
    Ray(const glm::vec3 &t_base, const glm::vec3 &t_dir): base(t_base), dir(t_dir) {}

    glm::vec3 at(float t) const;

    std::tuple<bool, float, float, float, float> ray_triangle_intersection(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2) const;
};