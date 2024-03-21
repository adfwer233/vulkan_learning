#pragma once

#include "vkl/vkl_scene.hpp"
class UIManager;

class SceneRenderUI {
  private:
    VklScene &scene_;
    UIManager &uiManager_;

  public:
    explicit SceneRenderUI(VklScene &scene, UIManager &uiManager);

    void renderImgui();
};