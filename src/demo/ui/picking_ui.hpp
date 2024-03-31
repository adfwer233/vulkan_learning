#pragma once

#include "vkl/scene/vkl_scene.hpp"

#include "ui_manager_register.hpp"

class UIManager;

class PickingUI {
  private:
    VklScene &scene_;
    UIManager &uiManager_;

  public:
    explicit PickingUI(VklScene &scene, UIManager &uiManager);

    void renderImgui();
};

REGISTER_TYPE(UIManagerRegisteredTypeTag, PickingUI)