#include "bezier_editor_panel_ui.hpp"

#include "../../ui_manager.hpp"

BezierEditorPanelUI::BezierEditorPanelUI(VklScene &scene, UIManager &uiManager) : scene_(scene), uiManager_(uiManager) {
}

void BezierEditorPanelUI::renderImgui() {

    ImGui::Begin("Bezier Panel");
    {
        ImGui::BeginChild("Bezier Editor Panel");

        if (ImGui::Button("Add Bezier Curve")) {
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

BezierEditorPanelUI::~BezierEditorPanelUI() {
}
