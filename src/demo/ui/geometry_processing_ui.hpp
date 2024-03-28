#pragma once

#include "vkl/vkl_scene.hpp"
class UIManager;

class GeometryProcessingUI {
  private:
    VklScene &scene_;
    UIManager &uiManager_;

    int variableIndex = 0;

    glm::vec3 lowColor{0.0f, 0.0f, 0.f};
    glm::vec3 highColor{1.0f, 0.0f, 0.0f};

  public:
    explicit GeometryProcessingUI(VklScene &scene, UIManager &uiManager);

    void renderImgui();
};