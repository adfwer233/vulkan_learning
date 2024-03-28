#pragma once

#include "vkl/vkl_object.hpp"
#include "Eigen/Eigen"

class GaussCurvature {
private:
    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
public:
    explicit GaussCurvature(VklModel &model);

    Eigen::VectorXd perform();

    std::string description() {
        return "Gauss Curvature of Mesh";
    }
};