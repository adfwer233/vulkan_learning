#include "geometry_processing_ui.hpp"

#include "material_ui.hpp"

#include "ui_manager.hpp"

GeometryProcessingUI::GeometryProcessingUI(VklScene &scene, UIManager &uiManager) : scene_(scene), uiManager_(uiManager) {
}

void GeometryProcessingUI::renderImgui() {
    ImGui::Begin("Geometry Processing Algorithms");

    ImGui::SeparatorText("Geometry Processing Algorithms");

    ImGui::End();
}
