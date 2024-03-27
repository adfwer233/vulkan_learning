#pragma once

#include "vkl/vkl_scene.hpp"

#include "material_ui.hpp"
#include "picking_ui.hpp"
#include "ray_tracer/ray_picker.hpp"
#include "render_mode_ui.hpp"
#include "scene_manager.hpp"
#include "scene_render_ui.hpp"
#include "scene_ui.hpp"
#include <optional>

#include "vkl/system/path_tracing_compute_system.hpp"

class UIManager {
  private:
    VklDevice &device_;
    VklScene &scene_;

    std::unique_ptr<SceneUI> sceneUi;
    std::unique_ptr<PickingUI> pickingUi;
    std::unique_ptr<RenderModeUI> renderModeUi;
    std::unique_ptr<MaterialUI> materialUi;
    std::unique_ptr<SceneRenderUI> sceneRenderUi;
    std::unique_ptr<SceneManagerUI> sceneManagerUi;

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

    /**
     * shading mode, point light or solid
     */
    int shadingMode = 0;

    /**
     * path tracing result texture
     */
    VklTexture *renderResultTexture = nullptr;
    VkDescriptorSet pathTracingResTex = VK_NULL_HANDLE;

    /**
     * offline rendering target image
     */
    std::vector<VkImageView> *offscreenImageViews;
    VkSampler offscreenSampler;

    std::unique_ptr<PathTracingComputeModel> pathTracingComputeModel_;
    std::unique_ptr<PathTracingComputeSystem> pathTracingComputeSystem_;

    uint32_t frameIndex;

    explicit UIManager(VklDevice &device, VklScene &scene);

    ~UIManager() {
        delete renderResultTexture;
    }

    void renderImgui();

    void resetBVH();

    void resetPathTracingCompute();

    void pickObject(float mouse_x_pos, float mouse_y_pos);
};