#include "vkl/io/assimp_loader.hpp"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include <format>
#include <iostream>
#include <iterator>

void VklAssimpLoader::load_material_texture(aiMaterial *material, aiTextureType type, MeshModel &model) {
    for (auto i = 0; i < material->GetTextureCount(type); i++) {
        aiString str;
        material->GetTexture(type, i, &str);

        std::string path = std::format("{}/{}", this->directory, std::string(str.C_Str()));

        if (type == aiTextureType::aiTextureType_DIFFUSE)
            model.texturePaths.push_back(path);
    }
}

MeshModel VklAssimpLoader::process_mesh(aiMesh *mesh, const aiScene *scene) {
    MeshModel model;

    for (auto i = 0; i < mesh->mNumVertices; i++) {
        decltype(model.vertices)::value_type vertex;
        vertex.position = {mesh->mVertices[i].x, -mesh->mVertices[i].y, mesh->mVertices[i].z};
        if (mesh->HasNormals())
            vertex.normal = {mesh->mNormals[i].x, -mesh->mNormals[i].y, mesh->mNormals[i].z};
        if (mesh->mTextureCoords[0])
            vertex.uv = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        model.vertices.push_back(vertex);
    }

    for (auto i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        model.indices.emplace_back(face.mIndices[0], face.mIndices[1], face.mIndices[2]);
    }

    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    load_material_texture(material, aiTextureType::aiTextureType_DIFFUSE, model);
    load_material_texture(material, aiTextureType::aiTextureType_SPECULAR, model);

    return model;
}

void VklAssimpLoader::process_node(aiNode *node, const aiScene *scene, std::vector<MeshModel> &meshes) {
    for (auto i = 0; i < node->mNumMeshes; i++) {
        auto mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(process_mesh(mesh, scene));
    }
    for (auto i = 0; i < node->mNumChildren; i++) {
        process_node(node->mChildren[i], scene, meshes);
    }
}

std::vector<MeshModel> VklAssimpLoader::load_model() {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(model_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

    std::vector<MeshModel> res;

    this->directory = model_path.substr(0, model_path.find_last_of('/'));

    process_node(scene->mRootNode, scene, res);

    return res;
}