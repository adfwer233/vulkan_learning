#include "material_ui.hpp"

#include "ui_manager.hpp"

MaterialUI::MaterialUI(VklScene &scene, UIManager &uiManager): scene_(scene), uiManager_(uiManager) {

}

void MaterialUI::renderImgui() {
    ImGui::Begin("Object Material");
    if (uiManager_.picking_result.has_value()) {
        auto &object_picked = scene_.objects[uiManager_.picking_result->object_index];
        auto model_picked = object_picked->models[uiManager_.picking_result->model_index];
        ImGui::SeparatorText("Material Information");
        ImGui::LabelText("Object Index", "%zu", uiManager_.picking_result->object_index);
        ImGui::LabelText("Model Index", "%zu", uiManager_.picking_result->model_index);

        const char* items[] = { "Gray", "Red", "Green", "White Light", "Metal", "Glass" };

        ImGui::Combo("model combo",&model_picked->materialIndex, items,IM_ARRAYSIZE(items));
    }
    ImGui::End();
}


