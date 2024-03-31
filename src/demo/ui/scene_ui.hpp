#pragma once

#include "vkl/scene/vkl_scene.hpp"

#include "ui_manager_register.hpp"

class UIManager;

class SceneUI {
  private:
    VklScene &scene_;
    UIManager &uiManager_;

  public:
    explicit SceneUI(VklScene &scene, UIManager &uiManager);

    void renderImgui();
};

REGISTER_TYPE(UIManagerRegisteredTypeTag, SceneUI)