#include "render_mode_ui.hpp"

#include "../ui_manager.hpp"
#include "vkl/ray_tracing/vkl_cpu_ray_tracer.hpp"

RenderModeUI::RenderModeUI(VklScene &scene, UIManager &uiManager) : scene_(scene), uiManager_(uiManager) {
}

void RenderModeUI::renderImgui() {
    ImGui::Begin("Rendering Options");

    ImGui::SeparatorText("Rendering Mode");

    ImGui::RadioButton("Raw", (int*)&uiManager_.renderMode, RenderMode::Raw);
    ImGui::SameLine();
    ImGui::RadioButton("Wire Frame", (int*)&uiManager_.renderMode, RenderMode::WireFrame);
    ImGui::SameLine();
    ImGui::RadioButton("With Texture", (int*)&uiManager_.renderMode, RenderMode::WithTexture);
    ImGui::SameLine();
    ImGui::RadioButton("Path Tracing", (int*)&uiManager_.renderMode, RenderMode::PathTracing);

    ImGui::SeparatorText("Shading Mode");

    ImGui::RadioButton("Point Light Shading", (int*)&uiManager_.shadingMode, ShadingMode::PointLightShading);
    ImGui::SameLine();
    ImGui::RadioButton("Solid Shading", (int*)&uiManager_.shadingMode, ShadingMode::SolidShading);
    ImGui::SameLine();
    ImGui::RadioButton("Pure Color", (int*)&uiManager_.shadingMode, ShadingMode::PureColor);

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
