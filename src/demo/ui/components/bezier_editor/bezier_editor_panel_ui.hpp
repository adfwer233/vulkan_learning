#pragma once

#include "../../ui_manager_register.hpp"
#include "vkl/io/vkl_image_export.hpp"
#include "vkl/scene/vkl_scene.hpp"

class UIManager;

class BezierEditorPanelUI {
  private:
    VklScene &scene_;
    UIManager &uiManager_;

  public:
    explicit BezierEditorPanelUI(VklScene &scene, UIManager &uiManager);

    ~BezierEditorPanelUI();

    void renderImgui();
};

REGISTER_TYPE(UIManagerRegisteredTypeTag, BezierEditorPanelUI)