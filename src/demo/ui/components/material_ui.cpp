#include "material_ui.hpp"

#include "../ui_manager.hpp"

MaterialUI::MaterialUI(VklScene &scene, UIManager &uiManager) : scene_(scene), uiManager_(uiManager) {
}

void MaterialUI::renderImgui() {
    ImGui::Begin("Object Material");
    if (uiManager_.picking_result.has_value()) {
        auto &object_picked = scene_.objects[uiManager_.picking_result->object_index];
        auto model_picked = object_picked->models[uiManager_.picking_result->model_index];
        ImGui::SeparatorText("Material Information");
        ImGui::LabelText("Object Index", "%zu", uiManager_.picking_result->object_index);
        ImGui::LabelText("Model Index", "%zu", uiManager_.picking_result->model_index);

        auto &material = scene_.materials[model_picked->materialIndex];

        ImGui::LabelText(
            "Albedo",
            std::format("{:.3f}, {:.3f}, {:.3f}", material.albedo.x, material.albedo.y, material.albedo.z).c_str());
        ImGui::ColorEdit3("Base Color", &material.albedo.x);
        ImGui::SliderFloat("Material Metallic", &material.metallic, 0.0f, 1.0f);
        ImGui::SliderFloat("Material Roughness", &material.roughness, 0.0f, 1.0f);
        ImGui::SliderFloat("Material Ao", &material.ao, 0.0f, 1.0f);

        //        ImGui::LabelText("Metal")

        const char *items[] = {"Gray", "Red", "Green", "White Light", "Metal", "Glass"};

        ImGui::Combo("model combo", &model_picked->materialIndex, items, IM_ARRAYSIZE(items));
    }
    ImGui::End();
}
