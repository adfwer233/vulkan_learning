#pragma once

#include "Eigen/Eigen"
#include "vkl/scene/vkl_model.hpp"

template <typename T>
concept MeshToVertexScalarQuantityMap = requires(T t, VklModel model) {
    T(model);
    { t.perform() } -> std::same_as<Eigen::VectorXd>;
    { t.description() } -> std::same_as<std::string>;
};