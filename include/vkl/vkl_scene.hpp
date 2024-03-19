#pragma once

#include <map>
#include <vector>

#include "bvh/vkl_bvh_gpu.hpp"
#include "vkl_object.hpp"

class VklScene {
  private:
    VklDevice &device_;

  public:
    VklScene(VklDevice &device, glm::vec3 camera_pos, glm::vec3 camera_up)
        : device_(device), camera(camera_pos, camera_up) {
        using namespace VklBVHGPUModel;
        pointLight.position = {0, -3, 3, 0};
        pointLight.color = {1.0, 1.0, 1.0, 1.0};

        Material gray{MaterialType::Lambertian, glm::vec3(0.3f, 0.3f, 0.3f)};
        Material red{MaterialType::Lambertian, glm::vec3(0.9f, 0.1f, 0.1f)};
        Material green{MaterialType::Lambertian, glm::vec3(0.1f, 0.9f, 0.1f)};
        Material whiteLight{MaterialType::LightSource, glm::vec3(2.0f, 2.0f, 2.0f)};
        Material metal{MaterialType::Metal, glm::vec3(1.0f, 0.9f, 0.9f)};
        Material glass{MaterialType::Glass, glm::vec3(1.0f, 1.0f, 1.0f)};

        materials.push_back(gray);
        materials.push_back(red);
        materials.push_back(green);
        materials.push_back(whiteLight);
        materials.push_back(metal);
        materials.push_back(glass);
    };

    std::vector<std::unique_ptr<VklObject>> objects;
    std::vector<VklBVHGPUModel::Material> materials;
    int lightObjectId;
    std::map<size_t, size_t> materialMap;

    Camera camera;
    PointLight pointLight;

    void addObject(VklObject::ImportBuilder builder);

    [[nodiscard]] int getSceneTriangleNum() const {
        int triangle_num = 0;
        for (const auto &object_item : objects) {
            triangle_num += object_item->get_triangle_num();
        }
        return triangle_num;
    }

    void setMaterial(size_t objectIndex, size_t materialIndex) {
        materialMap[objectIndex] = materialIndex;
    }
};