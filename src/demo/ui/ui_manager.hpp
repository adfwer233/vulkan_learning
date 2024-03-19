#pragma once

#include "vkl/vkl_scene.hpp"

#include "scene_ui.hpp"
#include "picking_ui.hpp"
#include "render_mode_ui.hpp"

#include <optional>
#include "ray_tracer/ray_picker.hpp"

class UIManager {
private:
    VklScene &scene_;

    std::unique_ptr<SceneUI> sceneUi;
    std::unique_ptr<PickingUI> pickingUi;
    std::unique_ptr<RenderModeUI> renderModeUi;

public:
    // used in scene ui
    float deltaTime;

    /**
     * picking result, used in picking UI and box rendering
     */
    std::optional<RayPicker::RayPickingResult> picking_result;

    /**
     * renderModel, used in render mode ui
     */
    int renderMode = 0;

    explicit UIManager(VklScene &scene);

    void renderImgui();

    void pickObject(float mouse_x_pos, float mouse_y_pos);
};