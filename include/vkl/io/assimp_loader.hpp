#pragma once

#include <string>
#include <vector>

#include "vkl/scene/vkl_model.hpp"

#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"

using MeshModel = VklModel::BuilderFromImmediateData;

class VklAssimpLoader {
  private:
    std::string directory;
    std::string model_path;

    void load_material_texture(aiMaterial *material, aiTextureType type, MeshModel &vklModel);
    MeshModel process_mesh(aiMesh *mesh, const aiScene *scene);
    void process_node(aiNode *node, const aiScene *scene, std::vector<MeshModel> &meshes);

  public:
    explicit VklAssimpLoader(std::string_view path) : model_path(path) {
    }

    std::vector<MeshModel> load_model();
};