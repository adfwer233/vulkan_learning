#pragma once

#include "vkl/scene/vkl_model.hpp"

#include "Eigen/Eigen"

template <typename T>
concept MeshToVertexVectorQuantityMap = requires(T t, VklModel &model) {
    T(model);
    { t.perform() } -> std::same_as<Eigen::MatrixXd>;
    { t.description() } -> std::same_as<std::string>;
};