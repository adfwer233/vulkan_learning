#include "bezier_editor_ui.hpp"

#include "../../ui_manager.hpp"

BezierEditorUI::BezierEditorUI(VklScene &scene, UIManager &uiManager) : scene_(scene), uiManager_(uiManager) {
}

void BezierEditorUI::renderImgui() {

    ImGui::Begin("Bezier Render");
    {
        ImGui::BeginChild("BezierEditorUI Render Result");
        ImVec2 wsize = {1024, 1024};

        scene_.camera.ratio = wsize.x / wsize.y;

        if (resTex.empty()) {
            resTex.resize(uiManager_.bezierImageViews->size());
            for (uint32_t i = 0; i < uiManager_.bezierImageViews->size(); i++)
                resTex[i] = ImGui_ImplVulkan_AddTexture(uiManager_.bezierSampler, (*uiManager_.bezierImageViews)[i],
                                                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }

        if (uiManager_.renderMode != RenderMode::PathTracing) {
            ImGui::Image(resTex[uiManager_.frameIndex], wsize);
        }

        ImGui::EndChild();
    }
    ImGui::End();
}

BezierEditorUI::~BezierEditorUI() {
}
