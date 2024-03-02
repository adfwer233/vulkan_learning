#include "ray_tracer/ray.hpp"

#include "tuple"

glm::vec3 Ray::at(float t) const {
    return base + t * dir;
}

std::tuple<bool, float, float, float, float> Ray::ray_triangle_intersection(const glm::vec3 &v0, const glm::vec3 &v1,
                                                                            const glm::vec3 &v2) const {
    // compute the normal of triangle
    constexpr float eps = 1e-8;

    auto normal = glm::cross(v1 - v0, v2 - v0);
    float area2 = glm::length(normal);

    if (area2 < eps)
        return std::make_tuple(false, 0, 0, 0, 0);
    normal = normal / area2;

    auto n_dot_dir = glm::dot(normal, this->dir);
    if (std::fabs(n_dot_dir) < eps) {
        return std::make_tuple(false, 0, 0, 0, 0);
    }

    float d = -1.0f * glm::dot(normal, v0);

    float t = -1.0f * (glm::dot(normal, this->base) + d) / n_dot_dir;

    if (t < eps)
        return std::make_tuple(false, 0, 0, 0, 0);

    auto p = this->at(t);

    float u, v, w;

    auto get_param = [&](const glm::vec3 &r, const glm::vec3 &s) -> float {
        auto edge = r - s;
        auto tmp = p - s;
        auto c = glm::cross(edge, tmp);
        auto area = glm::length(c);
        auto dot = glm::dot(normal, c);
        if (glm::dot(normal, c) < 0)
            return -1;
        return area / area2;
    };

    w = get_param(v1, v0);
    u = get_param(v2, v1);
    v = get_param(v0, v2);

    if (u < 0 or v < 0 or w < 0)
        return std::make_tuple(false, 0, 0, 0, 0);

    return std::make_tuple(true, t, u, v, w);
}