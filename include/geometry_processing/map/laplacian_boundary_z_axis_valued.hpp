#pragma once

#include "Eigen/Eigen"
#include "vkl/scene/vkl_object.hpp"

/**
 * solve laplacian function, the boundary is mesh boundary, boundary value is defined by z-axis position
 */
class LaplacianBoundaryZAxisValued {
  private:
    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
    Eigen::MatrixXi E;

  public:
    explicit LaplacianBoundaryZAxisValued(VklModel &model);

    Eigen::VectorXd perform();

    std::string description() {
        return "Laplacian equation";
    }
};