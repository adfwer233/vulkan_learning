#include "ui_manager.hpp"

#include "imgui.h"

UIManager::UIManager(VklDevice &device, VklScene &scene) : scene_(scene), device_(device) {
    sceneUi = std::make_unique<SceneUI>(scene, *this);
    pickingUi = std::make_unique<PickingUI>(scene, *this);
    renderModeUi = std::make_unique<RenderModeUI>(scene, *this);
    materialUi = std::make_unique<MaterialUI>(scene, *this);
    sceneRenderUi = std::make_unique<SceneRenderUI>(scene, *this);
    sceneManagerUi = std::make_unique<SceneManagerUI>(scene, *this);
    geometryProcessingUi = std::make_unique<GeometryProcessingUI>(scene, *this);
}

void UIManager::renderImgui() {
    ImGui::DockSpaceOverViewport();
    sceneUi->renderImgui();
    pickingUi->renderImgui();
    renderModeUi->renderImgui();
    materialUi->renderImgui();
    sceneRenderUi->renderImgui();
    sceneManagerUi->renderImgui();
    geometryProcessingUi->renderImgui();
}

void UIManager::pickObject(float mouse_x_pos, float mouse_y_pos) {

    auto position = scene_.camera.position;

    auto up = scene_.camera.camera_up_axis;
    auto right = scene_.camera.camera_right_axis;

    auto base_on_viewport = scene_.camera.position + scene_.camera.camera_front * 0.1f - up * 0.0414f - right * 0.0414f;
    up = up * 0.0414f / float(1024 / 2);
    right = right * 0.0414f / float(1024 / 2);
    base_on_viewport = base_on_viewport + up * float(mouse_y_pos) + right * float(mouse_x_pos);

    Ray ray(scene_.camera.position, base_on_viewport - scene_.camera.position);

    RayPicker rayTracer(scene_, ray);
    picking_result = rayTracer.trace();
}

void UIManager::resetBVH() {
    if (pathTracingComputeModel_ != nullptr) {
        pathTracingComputeModel_->resetGPUBVH();
    }
}

void UIManager::resetPathTracingCompute() {
    pathTracingComputeModel_ = std::make_unique<PathTracingComputeModel>(device_, scene_);
    pathTracingComputeSystem_ = std::make_unique<PathTracingComputeSystem>(device_, *pathTracingComputeModel_);
}
