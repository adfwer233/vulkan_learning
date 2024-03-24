#pragma once

#include "vkl/vkl_scene.hpp"
// #include "ui_manager.hpp"
class UIManager;

class SceneUI {
  private:
    VklScene &scene_;
    UIManager &uiManager_;

  public:
    explicit SceneUI(VklScene &scene, UIManager &uiManager);

    void renderImgui();
};