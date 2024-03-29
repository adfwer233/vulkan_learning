#pragma once

#include "vkl/scene/vkl_scene.hpp"
class UIManager;

class GeometryProcessingUI {
  private:
    VklScene &scene_;
    UIManager &uiManager_;

    int vertexScalarVariableIndex = 0;
    int vertexVectorVariableIndex = 0;

    glm::vec3 lowColor{0.0f, 0.0f, 0.f};
    glm::vec3 highColor{1.0f, 0.0f, 0.0f};

  public:
    explicit GeometryProcessingUI(VklScene &scene, UIManager &uiManager);

    void renderImgui();
};