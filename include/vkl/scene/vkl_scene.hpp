#pragma once

#include <map>
#include <vector>

#include "geometry/surface/tensor_product_bezier.hpp"
#include "vkl/bvh/vkl_bvh_gpu.hpp"
#include "vkl_object.hpp"

#include "vkl/io/model_loader_concept.hpp"

class VklScene {
public:
    VklDevice &device_;

    VklScene(VklDevice &device, glm::vec3 camera_pos, glm::vec3 camera_up)
        : device_(device), camera(camera_pos, camera_up) {
        using namespace VklBVHGPUModel;

        pointLight.position = {0, -3, 3, 0};
        pointLight.color = {1.0, 1.0, 1.0, 1.0};

        Material gray{MaterialType::Lambertian, 0.0, 0.5, 1.0, glm::vec3(0.77f, 0.77f, 0.80f)};
        Material red{MaterialType::Lambertian, 0.0, 0.5, 1.0, glm::vec3(0.8f, 0.0f, 0.0f)};
        Material green{MaterialType::Lambertian, 0.0, 0.5, 1.0, glm::vec3(0.0f, 0.8f, 0.0f)};
        Material whiteLight{MaterialType::LightSource, 0.0, 0.0, 1.0, glm::vec3(2.0f, 2.0f, 2.0f)};
        Material metal{MaterialType::Metal, 0.5, 0.0, 1.0, glm::vec3(1.0f, 0.9f, 0.0f)};
        Material glass{MaterialType::Glass, 0.0, 0.0, 1.0, glm::vec3(1.0f, 1.0f, 1.0f)};

        materials.push_back(gray);
        materials.push_back(red);
        materials.push_back(green);
        materials.push_back(whiteLight);
        materials.push_back(metal);
        materials.push_back(glass);
    };

    std::vector<std::unique_ptr<VklObject>> objects;
    std::vector<VklBVHGPUModel::Material> materials;

    std::vector<std::unique_ptr<TensorProductBezierSurface>> surfaces;

    Camera camera;
    PointLight pointLight;

    template <VklModelLoader Loader> void addObject(VklObject::ImportBuilder<Loader> builder);

    void addTensorProductBezierSurface(std::vector<std::vector<glm::vec3>> &&control_points) {
        std::vector<std::vector<std::array<float, 3>>> control_points_array;
        for (auto item_i : control_points) {
            auto &target = control_points_array.emplace_back();
            for (auto item_j : item_i) {
                target.emplace_back(std::array<float, 3>{item_j.x, item_j.y, item_j.z});
            }
        }

        TensorProductBezierSurface surf(std::move(control_points_array));
        auto meshModel = surf.getMeshModel();

        addObject(*meshModel);

        objects.back()->models.back()->underlyingGeometry = std::move(surf);
    }

    void addTensorProductBezierSurface(std::vector<std::vector<glm::vec3>> &&control_points,
                                       std::vector<std::vector<std::array<float, 2>>> &&boundary_data,
                                       std::vector<size_t> path_indices = {});

    void addObject(VklModel::BuilderFromImmediateData builder);

    template <typename VertexType, typename IndexType> void addObject(MeshModelTemplate<VertexType, IndexType> geom);

    [[nodiscard]] int getSceneTriangleNum() const {
        int triangle_num = 0;
        for (const auto &object_item : objects) {
            triangle_num += object_item->get_triangle_num();
        }
        return triangle_num;
    }

    void setMaterial(size_t objectIndex, int materialIndex) {
        objects[objectIndex]->setMaterial(materialIndex);
    }
};

#include "vkl_scene.hpp.impl"