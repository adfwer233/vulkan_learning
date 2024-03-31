#include "render_mode_ui.hpp"

#include "../ui_manager.hpp"
#include "vkl/ray_tracing/vkl_cpu_ray_tracer.hpp"

RenderModeUI::RenderModeUI(VklScene &scene, UIManager &uiManager) : scene_(scene), uiManager_(uiManager) {
}

void RenderModeUI::renderImgui() {
    ImGui::Begin("Rendering Options");

    ImGui::SeparatorText("Rendering Mode");

    ImGui::RadioButton("Raw", &uiManager_.renderMode, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Wire Frame", &uiManager_.renderMode, 1);
    ImGui::SameLine();
    ImGui::RadioButton("With Texture", &uiManager_.renderMode, 2);
    ImGui::SameLine();
    ImGui::RadioButton("Path Tracing", &uiManager_.renderMode, 3);

    ImGui::SeparatorText("Shading Mode");

    ImGui::RadioButton("Point Light Shading", &uiManager_.shadingMode, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Solid Shading", &uiManager_.shadingMode, 1);
    ImGui::SameLine();
    ImGui::RadioButton("Pure Color", &uiManager_.shadingMode, 2);

    ImGui::SeparatorText("Options");
    ImGui::Checkbox("Show Normal", &uiManager_.showNormal);
    ImGui::InputFloat("Normal Strength", &uiManager_.normalStrength, 0.1f);
    ImGui::ColorEdit3("Normal Color", (float *)&uiManager_.normalColor.x);

    ImGui::SeparatorText("Utils");

    if (ImGui::Button("Reset BVH Info")) {
        uiManager_.resetBVH();
    }

    ImGui::SameLine();

    if (ImGui::Button("CPU Ray Trace")) {
        VklCpuRayTracer cpuRayTracer(scene_);
        cpuRayTracer.performRayTracing();
    }
    ImGui::End();
}
