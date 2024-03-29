#pragma once

#include "vkl/scene/vkl_model.hpp"

template <typename T>
concept VariableVisualizer = requires(VklModel &model) { T::visualize(model); };