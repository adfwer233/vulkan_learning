#include <string>
#include <iostream>
#include <format>

#include "vkl/vkl_model.hpp"

using MeshModel = VklModel::BuilderFromImmediateData;

class VklTinyObjLoader {
    std::string path_;
public:
    explicit VklTinyObjLoader(std::string_view path): path_(path) {}

    std::vector<MeshModel> load_model();
};