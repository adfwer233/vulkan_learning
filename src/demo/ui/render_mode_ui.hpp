#pragma once

#include "vkl/scene/vkl_scene.hpp"
class UIManager;

class RenderModeUI {
  private:
    VklScene &scene_;
    UIManager &uiManager_;

  public:
    explicit RenderModeUI(VklScene &scene, UIManager &uiManager);

    void renderImgui();
};