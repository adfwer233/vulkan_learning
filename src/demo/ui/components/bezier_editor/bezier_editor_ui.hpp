#pragma once

#include "../../ui_manager_register.hpp"
#include "vkl/scene/vkl_scene.hpp"

class UIManager;

class BezierEditorUI {
  private:
    VklScene &scene_;
    UIManager &uiManager_;

    std::vector<VkDescriptorSet> resTex;

  public:
    explicit BezierEditorUI(VklScene &scene, UIManager &uiManager);

    ~BezierEditorUI();

    void renderImgui();
};

REGISTER_TYPE(UIManagerRegisteredTypeTag, BezierEditorUI)