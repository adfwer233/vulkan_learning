#pragma once

#include "vkl/vkl_scene.hpp"

class UIManager;

namespace SceneManagerUINamespace {
struct LightModel {
    uint32_t object_index;
    uint32_t model_index;
};

struct MaterialSetting {
    uint32_t object_index;
    uint32_t model_index;
    uint32_t material_index;
};

struct SceneInfo {
    std::string sceneName;
    std::vector<std::string> object_paths;
    LightModel lightModel;
    std::vector<MaterialSetting> materialSettings;
};
} // namespace SceneManagerUINamespace

enum ModelLoader {
    AssimpLoader,
    TinyObjLoader
};

class SceneManagerUI {
  private:
    VklScene &scene_;
    UIManager &uiManager_;

    std::vector<SceneManagerUINamespace::SceneInfo> scene_infos;

    int current_scene_index = 0;

  public:
    explicit SceneManagerUI(VklScene &scene, UIManager &uiManager);

    void renderImgui();
};