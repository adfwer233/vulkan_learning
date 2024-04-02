#pragma once

#include "../ui_manager_register.hpp"
#include "vkl/scene/vkl_scene.hpp"

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