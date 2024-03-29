#pragma once

#include "Eigen/Eigen"
#include "vkl/scene/vkl_object.hpp"

class NormalVector {
private:
    Eigen::MatrixXd V;
    Eigen::MatrixXi F;

public:
    explicit NormalVector(VklModel &model);

    Eigen::MatrixXd perform();

    std::string description() {
        return "Normal Vector of Mesh";
    }
};