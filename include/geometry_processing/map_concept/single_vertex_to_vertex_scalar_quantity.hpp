#pragma once

#include "Eigen/Eigen"

template<typename T>
concept SingleVertexToVertexScalarQuantityMap = requires(T t, uint32_t idx) {
    {t.perform(idx)} -> std::same_as<Eigen::VectorXd>;
    {t.description()} -> std::same_as<std::string>;
};