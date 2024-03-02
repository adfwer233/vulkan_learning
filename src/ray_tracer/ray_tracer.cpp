#include "ray_tracer/ray_tracer.hpp"
#include "ray_tracer/ray.hpp"

#include <iostream>
#include <map>

RayTracer::RayTracer(std::vector<VklObject *> &object, Ray ray): objects_(object), ray_(ray) {}

std::optional<RayTracer::RayTracingResult> RayTracer::trace() {
    std::map<float, RayTracingResult> param_result_map;
    for (size_t object_index = 0; object_index < objects_.size(); object_index++) {
        auto object = objects_[object_index];
        for (size_t model_index = 0; model_index < object->models.size(); model_index++) {
            auto model = object->models[model_index];

            for (size_t face_index = 0; face_index < model->indices_.size(); face_index++) {
                auto &face = model->indices_[face_index];

                auto [flag, t, u, v, w] = ray_.ray_triangle_intersection(
                        model->vertices_[face.i].position,
                        model->vertices_[face.j].position,
                        model->vertices_[face.k].position
                    );

                if (flag) {
                    param_result_map[t] = {object_index, model_index, face_index, t, u, v, w};
                }
            }
        }
    }
    std::optional<RayTracingResult> result = std::nullopt;

    if (not param_result_map.empty()) {
        auto& [t, res] = *param_result_map.begin();
        result = res;
    }

    return result;
}

