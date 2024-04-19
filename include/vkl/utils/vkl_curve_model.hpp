#pragma once

#include "vkl/scene/vkl_model.hpp"

using VklCurveModel3D = VklModelTemplate<VklVertex3D, LineIndex, VklBox3D>;
using VklCurveModel2D = VklModelTemplate<VklVertex2D, LineIndex, VklBox2D>;