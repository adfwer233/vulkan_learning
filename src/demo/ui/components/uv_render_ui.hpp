#pragma once

#include "../ui_manager_register.hpp"
#include "vkl/scene/vkl_scene.hpp"

class UIManager;

class UVRenderUI {
  private:
    VklScene &scene_;
    UIManager &uiManager_;

    std::vector<VkDescriptorSet> resTex;

  public:
    explicit UVRenderUI(VklScene &scene, UIManager &uiManager);

    ~UVRenderUI();

    void renderImgui();
};

REGISTER_TYPE(UIManagerRegisteredTypeTag, UVRenderUI)