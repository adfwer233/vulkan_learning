#pragma once

#include <string>
#include <vector>

#include "../vkl_model.hpp"

#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"

using MeshModel = VklModel::BuilderFromImmediateData;

class AssimpLoader {
  private:
    std::string directory;

    void load_material_texture(aiMaterial *material, aiTextureType type, MeshModel &vklModel);
    MeshModel process_mesh(aiMesh *mesh, const aiScene *scene);
    void process_node(aiNode *node, const aiScene *scene, std::vector<MeshModel> &meshes);

  public:
    std::vector<MeshModel> read_model(std::string model_path);
};