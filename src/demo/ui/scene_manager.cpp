#include "scene_manager.hpp"

#include <format>
#include <iostream>

#include "imgui.h"
#include "ui_manager.hpp"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

using namespace SceneManagerUINamespace;

SceneManagerUI::SceneManagerUI(VklScene &scene, UIManager &uiManager): scene_(scene), uiManager_(uiManager) {
    std::ifstream f("scene_info.json");
    json data = json::parse(f);

    auto scene_num = data["scene_num"].get<int>();

    for (auto scene_json: data["scenes"]) {
        auto &scene_info = scene_infos.emplace_back();

        scene_info.sceneName = scene_json["scene_name"].get<std::string>();

        auto object_paths = scene_json["object_paths"];
        for (const auto& object_path_json: object_paths) {
            scene_info.object_paths.push_back(object_path_json.get<std::string>());
        }
        auto light_model_json = scene_json["light_model"];
        scene_info.lightModel.object_index = light_model_json["object_index"].get<uint32_t>();
        scene_info.lightModel.model_index = light_model_json["model_index"].get<uint32_t >();

        auto material_settings = scene_json["material_settings"];

        for (auto material_setting_json: material_settings) {
            auto &material_setting = scene_info.materialSettings.emplace_back();
            material_setting.object_index = material_setting_json["object_index"].get<uint32_t>();
            material_setting.model_index = material_setting_json["model_index"].get<uint32_t >();
            material_setting.material_index = material_setting_json["material_index"].get<uint32_t >();
        }
    }
}

void SceneManagerUI::renderImgui() {
    std::vector<const char *> scene_name_items;
    for (const auto& scene_info: this->scene_infos) {
        scene_name_items.push_back(scene_info.sceneName.c_str());
    }

    auto sceneInfo = scene_infos[current_scene_index];

    ImGui::Begin("Scene Manager");

    if (ImGui::Button("Load Scene")) {
        for (auto &objectPath: sceneInfo.object_paths) {
            VklObject::ImportBuilder builder(std::format("{}/{}", DATA_DIR, objectPath));
            scene_.addObject(builder);
        }
        scene_.objects[sceneInfo.lightModel.object_index]->models[sceneInfo.lightModel.model_index]->materialIndex = 3;
        for (auto &materialSetting: sceneInfo.materialSettings) {
            scene_.objects[materialSetting.object_index]->models[materialSetting.model_index]->materialIndex = materialSetting.material_index;
        }
        uiManager_.resetPathTracingCompute();
    }


    ImGui::SeparatorText("Scene Information");

    ImGui::Combo("Scene List", &this->current_scene_index, scene_name_items.data(), scene_name_items.size());

    ImGui::SeparatorText("Objects");
    for (auto & objectPath : sceneInfo.object_paths) {
        ImGui::Text("%s", objectPath.c_str());
    }
    ImGui::SeparatorText("Light Info");
    ImGui::LabelText("Object Index", "%d", sceneInfo.lightModel.object_index);
    ImGui::LabelText("Model Index", "%d", sceneInfo.lightModel.model_index);

    ImGui::SeparatorText("Materials");

    for (auto i = 0; i < sceneInfo.materialSettings.size(); i++) {
        auto materialSetting = sceneInfo.materialSettings[i];
        ImGui::Text("<%d, %d>: %d", materialSetting.object_index, materialSetting.model_index, materialSetting.material_index);
    }

    ImGui::End();
}
