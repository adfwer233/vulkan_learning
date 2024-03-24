#include "scene_render_ui.hpp"

#include "ui_manager.hpp"

SceneRenderUI::SceneRenderUI(VklScene &scene, UIManager &uiManager) : scene_(scene), uiManager_(uiManager) {
}

void SceneRenderUI::renderImgui() {
    ImGui::Begin("Render Result");
    {
        ImGui::BeginChild("RenderResult");
        ImVec2 wsize(1024, 1024);
        if (resTex.empty()) {
            resTex.resize(uiManager_.offscreenImageViews->size());
            for (uint32_t i = 0; i < uiManager_.offscreenImageViews->size(); i++)
                resTex[i] =
                    ImGui_ImplVulkan_AddTexture(uiManager_.offscreenSampler, (*uiManager_.offscreenImageViews)[i],
                                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
        if (uiManager_.renderMode == 3) {
            if (uiManager_.pathTracingResTex == VK_NULL_HANDLE) {
                uiManager_.pathTracingResTex = ImGui_ImplVulkan_AddTexture(
                    uiManager_.renderResultTexture->getTextureSampler(),
                    uiManager_.renderResultTexture->getTextureImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            }
            ImGui::Image(uiManager_.pathTracingResTex, wsize, ImVec2(0, 1), ImVec2(1, 0));
        } else {
            ImGui::Image(resTex[uiManager_.frameIndex], wsize);
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

SceneRenderUI::~SceneRenderUI() {
}
