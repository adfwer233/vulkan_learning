#pragma once

#include "vkl_model.hpp"

class VklObject {
public:
    std::vector<VklModel> models;

    void render_object();
};