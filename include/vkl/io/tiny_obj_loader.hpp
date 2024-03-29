#include <format>
#include <iostream>
#include <string>

#include "vkl/scene/vkl_model.hpp"

using MeshModel = VklModel::BuilderFromImmediateData;

class VklTinyObjLoader {
    std::string path_;

  public:
    explicit VklTinyObjLoader(std::string_view path) : path_(path) {
    }

    std::vector<MeshModel> load_model();
};