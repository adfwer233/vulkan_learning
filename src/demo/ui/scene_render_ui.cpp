#include "scene_render_ui.hpp"

#include "ui_manager.hpp"

SceneRenderUI::SceneRenderUI(VklScene &scene, UIManager &uiManager): scene_(scene), uiManager_(uiManager) {}

void SceneRenderUI::renderImgui() {
    ImGui::Begin("Render Result");
    {
        ImGui::BeginChild("RenderResult");
        ImVec2 wsize = ImGui::GetWindowSize();

        auto tex = ImGui_ImplVulkan_AddTexture(uiManager_.renderResultTexture->getTextureSampler(), uiManager_.renderResultTexture->getTextureImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        ImGui::Image(tex, wsize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::EndChild();
    }
    ImGui::End();
}
