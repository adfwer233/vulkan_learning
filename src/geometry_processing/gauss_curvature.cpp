#include "geometry_processing/gauss_curvature.hpp"

#include "igl/gaussian_curvature.h"

void GaussCurvature::perform(VklModel &model) {
    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
    Eigen::VectorXd K;
    igl::gaussian_curvature(V, F, K);
}