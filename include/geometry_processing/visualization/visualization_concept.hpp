#pragma once

#include "vkl/vkl_model.hpp"

template<typename T>
concept VariableVisualizer = requires(VklModel &model) {
    T::visualize(model);
};