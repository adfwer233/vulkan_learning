#pragma once

#include "vkl/scene/vkl_scene.hpp"
#include "ui_manager_register.hpp"

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

REGISTER_TYPE(UIManagerRegisteredTypeTag, SceneRenderUI)