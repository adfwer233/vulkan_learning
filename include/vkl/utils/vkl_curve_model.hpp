#pragma once

#include "vkl/scene/vkl_model.hpp"

using VklCurveModel3D = VklModelTemplate<Vertex3D, LineIndex, VklBox3D>;
using VklCurveModel2D = VklModelTemplate<Vertex2D, LineIndex, VklBox2D>;