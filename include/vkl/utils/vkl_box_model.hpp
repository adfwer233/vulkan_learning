#pragma once

#include "vkl/scene/vkl_model.hpp"
#include "vkl/templates/vkl_index.hpp"
#include "vkl_box.hpp"

using VklBoxModel2D = VklModelTemplate<Vertex2DRaw, LineIndex>;
using VklBoxModel3D = VklModelTemplate<Vertex3DRaw, LineIndex>;

VklBoxModel3D::BuilderFromImmediateData getStandardBox3D() {
    VklBoxModel3D::BuilderFromImmediateData builder;

    builder.vertices = std::vector<Vertex3DRaw>{{{0, 0, 0}}, {{0, 1, 0}}, {{1, 0, 0}}, {{1, 1, 0}},
                                                {{0, 0, 1}}, {{0, 1, 1}}, {{1, 0, 1}}, {{1, 1, 1}}};

    builder.indices = std::vector<LineIndex>{{0, 1}, {1, 3}, {3, 2}, {2, 0}, {4, 5}, {5, 7},
                                             {7, 6}, {6, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

    return builder;
}

VklBoxModel2D::BuilderFromImmediateData getStandardBox2D() {
    VklBoxModel2D::BuilderFromImmediateData builder;

    builder.vertices = std::vector<Vertex2DRaw>{
        {{0, 0}},
        {{0, 1}},
        {{1, 0}},
        {{1, 1}},
    };

    builder.indices = std::vector<LineIndex>{
        {0, 1},
        {1, 3},
        {3, 2},
        {2, 0},
    };

    return builder;
}