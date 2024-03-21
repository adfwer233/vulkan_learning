#pragma once

#include "vkl/vkl_scene.hpp"

#include "scene_ui.hpp"
#include "scene_render_ui.hpp"
#include "picking_ui.hpp"
#include "render_mode_ui.hpp"
#include "material_ui.hpp"
#include <optional>
#include "ray_tracer/ray_picker.hpp"

#include "vkl/system/path_tracing_compute_system.hpp"

class UIManager {
private:
    VklScene &scene_;

    PathTracingComputeModel &pathTracingComputeModel_;

    std::unique_ptr<SceneUI> sceneUi;
    std::unique_ptr<PickingUI> pickingUi;
    std::unique_ptr<RenderModeUI> renderModeUi;
    std::unique_ptr<MaterialUI> materialUi;
    std::unique_ptr<SceneRenderUI> sceneRenderUi;

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


    VklTexture *renderResultTexture = nullptr;

    explicit UIManager(VklScene &scene, PathTracingComputeModel &pathTracingComputeModel);

    void renderImgui();

    void resetBVH();

    void pickObject(float mouse_x_pos, float mouse_y_pos);
};