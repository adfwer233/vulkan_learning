#include "scene_manager.hpp"

#include <format>
#include <iostream>

#include "../ui_manager.hpp"
#include "imgui.h"

#include "geometry/surface/tensor_product_bezier.hpp"
#include "nlohmann/json.hpp"
#include "vkl/io/assimp_loader.hpp"
#include "vkl/io/tiny_obj_loader.hpp"

#include "simulation/walk_on_sphere/walk_on_sphere.hpp"
#include "simulation/walk_on_sphere/walk_on_sphere_anisotropic.hpp"

using json = nlohmann::json;

using namespace SceneManagerUINamespace;

SceneManagerUI::SceneManagerUI(VklScene &scene, UIManager &uiManager) : scene_(scene), uiManager_(uiManager) {
    std::ifstream f("scene_info.json");
    json data = json::parse(f);

    auto scene_num = data["scene_num"].get<int>();

    for (auto scene_json : data["scenes"]) {
        auto &scene_info = scene_infos.emplace_back();

        scene_info.sceneName = scene_json["scene_name"].get<std::string>();

        auto object_paths = scene_json["object_paths"];
        for (const auto &object_path_json : object_paths) {
            scene_info.object_paths.push_back(object_path_json.get<std::string>());
        }
        auto light_model_json = scene_json["light_model"];
        scene_info.lightModel.object_index = light_model_json["object_index"].get<uint32_t>();
        scene_info.lightModel.model_index = light_model_json["model_index"].get<uint32_t>();

        auto material_settings = scene_json["material_settings"];

        for (auto material_setting_json : material_settings) {
            auto &material_setting = scene_info.materialSettings.emplace_back();
            material_setting.object_index = material_setting_json["object_index"].get<uint32_t>();
            material_setting.model_index = material_setting_json["model_index"].get<uint32_t>();
            material_setting.material_index = material_setting_json["material_index"].get<uint32_t>();
        }

        auto model_scale_infos_json = scene_json["model_scale"];

        for (auto model_scale_info_json : model_scale_infos_json) {
            auto &model_scale = scene_info.modelScaleInfos.emplace_back();
            model_scale.object_index = model_scale_info_json["object_index"].get<uint32_t>();
            model_scale.scale = model_scale_info_json["scale"].get<float>();
        }
    }
}

void SceneManagerUI::renderImgui() {
    std::vector<const char *> scene_name_items;
    for (const auto &scene_info : this->scene_infos) {
        scene_name_items.push_back(scene_info.sceneName.c_str());
    }

    auto sceneInfo = scene_infos[current_scene_index];

    ImGui::Begin("Scene Manager");

    ImGui::SeparatorText("Scene Loading");

    ImGui::RadioButton("Assimp Loader", reinterpret_cast<int *>(&uiManager_.modelLoader), ModelLoader::AssimpLoader);
    ImGui::SameLine();
    ImGui::RadioButton("TinyObj Loader", reinterpret_cast<int *>(&uiManager_.modelLoader), ModelLoader::TinyObjLoader);

    if (ImGui::Button("Load Scene")) {
        for (auto &objectPath : sceneInfo.object_paths) {
            std::string full_path = std::format("{}/{}", DATA_DIR, objectPath);
            if (uiManager_.modelLoader == AssimpLoader) {
                VklObject::ImportBuilder<VklAssimpLoader> builder(full_path);
                scene_.addObject(builder);
            } else if (uiManager_.modelLoader == TinyObjLoader) {
                VklObject::ImportBuilder<VklTinyObjLoader> builder(full_path);
                scene_.addObject(builder);
            }
            scene_.objects.back()->dataFilePath = full_path;
        }
        scene_.objects[sceneInfo.lightModel.object_index]->models[sceneInfo.lightModel.model_index]->materialIndex = 3;
        for (auto &materialSetting : sceneInfo.materialSettings) {
            scene_.objects[materialSetting.object_index]->models[materialSetting.model_index]->materialIndex =
                materialSetting.material_index;
        }
        for (auto &scaleInfo : sceneInfo.modelScaleInfos) {
            scene_.objects[scaleInfo.object_index]->modelScaling = {scaleInfo.scale, scaleInfo.scale, scaleInfo.scale};
        }
        uiManager_.resetPathTracingCompute();
    }

    if (ImGui::Button("Load Bezier Boundary")) {
        std::string full_path = std::format("{}/{}", DATA_DIR, "bezier/shape2.json");
        std::cout << full_path << std::endl;
        std::ifstream f(full_path);
        json data = json::parse(f);

        auto get_point = [&](json &point_json) -> std::pair<float, float> {
            return {point_json["x"].get<float>() / 500, point_json["y"].get<float>() / 500};
        };

        std::vector<std::vector<std::array<float, 2>>> boundary_data;

        for (auto path_json: data) {
            for (auto bezier: path_json) {
                auto straight = false;
                for (auto points: bezier) {
                    if (points.is_boolean()) {
                        straight = true;
                    }
                }
                auto& boundary = boundary_data.emplace_back();
                if (straight) {
                    auto [x1, y1] = get_point(bezier[0]);
                    auto [x2, y2] = get_point(bezier[3]);
                    std::cout << std::format("({}, {}), ({}, {}) \n", x1, y1, x2, y2);
                    boundary.push_back({x1, y1});
                    boundary.push_back({x2, y2});
                } else {
                    auto [x1, y1] = get_point(bezier[0]);
                    auto [x2, y2] = get_point(bezier[1]);
                    auto [x3, y3] = get_point(bezier[2]);
                    auto [x4, y4] = get_point(bezier[3]);
                    boundary.push_back({x1, y1});
                    boundary.push_back({x2, y2});
                    boundary.push_back({x3, y3});
                    boundary.push_back({x4, y4});
                    std::cout << std::format("({}, {}), ({}, {}), ({}, {}), ({}, {}) \n", x1, y1, x2, y2, x3, y3, x4, y4);
                }
            }
        }

        std::vector<std::vector<glm::vec3>> control_points{{{-1.0, 0.0, -1.0}, {-1.0, 0.0, 0.0}, {-1.0, 0.0, 1.0}},
                                                           {{0.0, 0.0, -1.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}},
                                                           {{1.0, 0.0, -1.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 1.0}}};

        scene_.addTensorProductBezierSurface(std::move(control_points), std::move(boundary_data));
    }

    if (ImGui::Button("Load Bezier Surface")) {
        std::vector<std::vector<glm::vec3>> control_points{{{-1.0, 0.0, -1.0}, {-1.0, 0.0, 0.0}, {-1.0, 0.0, 1.0}},
                                                           {{0.0, 0.0, -1.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}},
                                                           {{1.0, 0.0, -1.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 1.0}}};

        scene_.addTensorProductBezierSurface(std::move(control_points));
    }

    if (ImGui::Button("Load Bezier Surface As Mesh")) {
        std::vector<std::vector<glm::vec3>> control_points{{{-1.0, 0.0, -1.0}, {-1.0, 0.0, 0.0}, {-1.0, 0.0, 1.0}},
                                                           {{0.0, 5.0, -1.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}},
                                                           {{1.0, 0.0, -1.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 1.0}}};

        TensorProductBezierSurface surface(std::move(control_points));

        auto builder = surface.getMeshModelBuilder();
        scene_.addObject(builder);
    }

    if (ImGui::Button("Load Bezier Surface Wos Parameter Space Evaluated")) {
        std::vector<std::vector<glm::vec3>> control_points{{{-1.0, 0.0, -1.0}, {-1.0, 0.0, 0.0}, {-1.0, 0.0, 1.0}},
                                                           {{0.0, 0.0, -1.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}},
                                                           {{1.0, 0.0, -1.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 1.0}}};

        TensorProductBezierSurface surface(std::move(control_points));

        ParameterSpaceWalkOnSphere wosSolver(&surface);

        auto builder = wosSolver.getMeshModelBuilderWos();
        scene_.addObject(builder);
    }

    if (ImGui::Button("Load Bezier Surface Wos Anisotropic")) {
        std::vector<std::vector<glm::vec3>> control_points{{{-1.0, 0.0, -1.0}, {-1.0, 0.0, 0.0}, {-1.0, 0.0, 1.0}},
                                                           {{0.0, 5.0, -1.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}},
                                                           {{1.0, 0.0, -1.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 1.0}}};

        TensorProductBezierSurface surface(std::move(control_points));

        AnisotropicWalkOnSphere wosSolver(&surface);

        auto builder = wosSolver.getMeshModelBuilderWos();
        scene_.addObject(builder);
    }

    ImGui::SeparatorText("Scene Information");

    ImGui::Combo("Scene List", &this->current_scene_index, scene_name_items.data(), scene_name_items.size());

    ImGui::SeparatorText("Objects");
    for (auto &objectPath : sceneInfo.object_paths) {
        ImGui::Text("%s", objectPath.c_str());
    }
    ImGui::SeparatorText("Light Info");
    ImGui::LabelText("Object Index", "%d", sceneInfo.lightModel.object_index);
    ImGui::LabelText("Model Index", "%d", sceneInfo.lightModel.model_index);

    ImGui::SeparatorText("Materials");

    for (auto i = 0; i < sceneInfo.materialSettings.size(); i++) {
        auto materialSetting = sceneInfo.materialSettings[i];
        ImGui::Text("object %d, model %d: %d", materialSetting.object_index, materialSetting.model_index,
                    materialSetting.material_index);
    }

    ImGui::End();
}
