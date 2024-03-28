#include "ray_tracer/ray_picker.hpp"
#include "ray_tracer/ray.hpp"

#include <iostream>
#include <map>

RayPicker::RayPicker(VklScene &scene, Ray ray) : scene_(scene), ray_(ray) {
}

std::optional<RayPicker::RayPickingResult> RayPicker::trace() {

    std::map<float, RayPickingResult> param_result_map;
    for (size_t object_index = 0; object_index < scene_.objects.size(); object_index++) {
        auto &object = scene_.objects[object_index];

        Ray object_ray = ray_;
        glm::mat4 model_transformation = object->getModelTransformation();

        for (size_t model_index = 0; model_index < object->models.size(); model_index++) {
            auto model = object->models[model_index];

            for (size_t face_index = 0; face_index < model->indices_.size(); face_index++) {
                auto &face = model->indices_[face_index];

                auto [flag, t, u, v, w] = object_ray.ray_triangle_intersection(
                    model_transformation * glm::vec4(model->vertices_[face.i].position, 1.0f),
                    model_transformation * glm::vec4(model->vertices_[face.j].position, 1.0f),
                    model_transformation * glm::vec4(model->vertices_[face.k].position, 1.0f));

                if (flag) {
                    param_result_map[t] = {object_index, model_index, face_index, 0, t, u, v, w};
                }
            }
        }
    }
    std::optional<RayPickingResult> result = std::nullopt;

    if (not param_result_map.empty()) {
        auto &[t, res] = *param_result_map.begin();
        result = res;
    }

    return result;
}
