#pragma once

#include "vkl/scene/vkl_scene.hpp"

#include "ui_manager_register.hpp"

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

struct ModelScale {
    uint32_t object_index;
    float scale;
};

struct SceneInfo {
    std::string sceneName;
    std::vector<std::string> object_paths;
    LightModel lightModel;
    std::vector<MaterialSetting> materialSettings;
    std::vector<ModelScale> modelScaleInfos;
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

REGISTER_TYPE(UIManagerRegisteredTypeTag, SceneManagerUI)