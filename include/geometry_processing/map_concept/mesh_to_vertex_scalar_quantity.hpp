#pragma once

#include "Eigen/Eigen"
// #include "vkl/vkl_model.hpp"

template <typename T>
concept MeshToVertexScalarQuantityMap = requires(T t) {
    { t.perform() } -> std::same_as<Eigen::VectorXd>;
    { t.description() } -> std::same_as<std::string>;
};