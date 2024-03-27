#include "geometry_processing_ui.hpp"

#include "material_ui.hpp"

#include "ui_manager.hpp"

#include "geometry_processing/gauss_curvature.hpp"

GeometryProcessingUI::GeometryProcessingUI(VklScene &scene, UIManager &uiManager) : scene_(scene), uiManager_(uiManager) {
}

void GeometryProcessingUI::renderImgui() {
    ImGui::Begin("Geometry Processing Algorithms");

    ImGui::SeparatorText("Geometry Processing Algorithms");

    if (this->uiManager_.picking_result.has_value()) {

        auto object_index = uiManager_.picking_result->object_index;
        auto model_index = uiManager_.picking_result->model_index;

        auto model = this->scene_.objects[object_index]->models[model_index];

        if (ImGui::Button("Compute Gauss Curvature")) {
            GaussCurvature gaussCurvature;
            gaussCurvature.perform(*model);
        }
    }

    ImGui::End();
}
