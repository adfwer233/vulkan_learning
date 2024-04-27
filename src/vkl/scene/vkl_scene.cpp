#include "vkl/scene/vkl_scene.hpp"

void VklScene::addObject(VklModel::BuilderFromImmediateData builder) {
    objects.push_back(std::make_unique<VklObject>(device_, builder));
    objects.back()->allocDescriptorSets(*setLayout_, *descriptorPool_);
}

void VklScene::addTensorProductBezierSurface(std::vector<std::vector<glm::vec3>> &&control_points,
                                             std::vector<std::vector<std::array<float, 2>>> &&boundary_data,
                                             std::vector<size_t> path_indices) {
    std::vector<std::vector<std::array<float, 3>>> control_points_array;
    for (auto item_i : control_points) {
        auto &target = control_points_array.emplace_back();
        for (auto item_j : item_i) {
            target.emplace_back(std::array<float, 3>{item_j.x, item_j.y, item_j.z});
        }
    }

    TensorProductBezierSurface surf(std::move(control_points_array), std::move(boundary_data), path_indices);
    auto meshModel = surf.getMeshModel();

    addObject(*meshModel);

    objects.back()->models.back()->underlyingGeometry = std::move(surf);
}
