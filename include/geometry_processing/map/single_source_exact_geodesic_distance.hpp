#pragma once

#include "vkl/vkl_model.hpp"

#include "Eigen/Eigen"

class SingleSourceExactGeodesicDistance {
  private:
    Eigen::MatrixXd V;
    Eigen::MatrixXd F;

  public:
    explicit SingleSourceExactGeodesicDistance(VklModel &model);

    Eigen::VectorXd perform(uint32_t vertexIndex);

    std::string description() const {
        return "Single source exact geodesic result";
    }
};