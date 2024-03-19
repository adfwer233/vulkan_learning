#include "ui_manager.hpp"

UIManager::UIManager(VklScene &scene): scene_(scene) {
    sceneUi = std::make_unique<SceneUI>(scene, *this);
    pickingUi = std::make_unique<PickingUI>(scene, *this);
    renderModeUi = std::make_unique<RenderModeUI>(scene, *this);
}

void UIManager::renderImgui() {
    sceneUi->renderImgui();
    pickingUi->renderImgui();
    renderModeUi->renderImgui();
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
