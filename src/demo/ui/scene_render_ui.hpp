#pragma once

#include "vkl/vkl_scene.hpp"
class UIManager;

class SceneRenderUI {
  private:
    VklScene &scene_;
    UIManager &uiManager_;

    std::vector<VkDescriptorSet> resTex;

  public:
    explicit SceneRenderUI(VklScene &scene, UIManager &uiManager);

    ~SceneRenderUI();

    void renderImgui();
};