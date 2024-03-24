#pragma once

#include "vkl/vkl_scene.hpp"
class UIManager;

class MaterialUI {
  private:
    VklScene &scene_;
    UIManager &uiManager_;

  public:
    explicit MaterialUI(VklScene &scene, UIManager &uiManager);

    void renderImgui();
};