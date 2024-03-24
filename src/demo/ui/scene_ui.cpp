#include "scene_ui.hpp"

#include <format>

#include "imgui.h"
#include "ui_manager.hpp"

SceneUI::SceneUI(VklScene &scene, UIManager &uiManager) : scene_(scene), uiManager_(uiManager) {
}

void SceneUI::renderImgui() {
    ImGui::Begin("Messages");
    ImGui::SeparatorText("Scene Information");
    ImGui::LabelText("# Triangles", "%d", scene_.getSceneTriangleNum());
    ImGui::LabelText("Camera Position", std::format("{:.3f}, {:.3f}, {:.3f}", scene_.camera.position.x,
                                                    scene_.camera.position.y, scene_.camera.position.z)
                                            .c_str());
    ImGui::LabelText("Point Light Position", std::format("{:.3f}, {:.3f}, {:.3f}", scene_.pointLight.position.x,
                                                         scene_.pointLight.position.y, scene_.pointLight.position.z)
                                                 .c_str());
    ImGui::SeparatorText("Performance");
    ImGui::LabelText("FPS", std::format("{:.3f}", 1 / uiManager_.deltaTime).c_str());
    ImGui::End();
}
