#pragma once

#include "vkl/vkl_scene.hpp"
class UIManager;

class GeometryProcessingUI {
private:
    VklScene &scene_;
    UIManager &uiManager_;

public:
    explicit GeometryProcessingUI(VklScene &scene, UIManager &uiManager);

    void renderImgui();
};