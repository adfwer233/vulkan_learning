#pragma once

#include "vkl/vkl_scene.hpp"
class UIManager;

class PickingUI {
  private:
    VklScene &scene_;
    UIManager &uiManager_;

  public:
    explicit PickingUI(VklScene &scene, UIManager &uiManager);

    void renderImgui();
};