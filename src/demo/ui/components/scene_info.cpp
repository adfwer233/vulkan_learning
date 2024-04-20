#include "scene_info.hpp"

#include "../ui_manager.hpp"

SceneInfoUI::SceneInfoUI(VklScene &scene, UIManager &uiManager) : scene_(scene), uiManager_(uiManager) {
}

void SceneInfoUI::renderImgui() {
    ImGui::Begin("Scene Data");

    ImGui::SeparatorText("Rendering Mode");

    ImGui::End();
}
