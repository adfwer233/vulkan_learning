#include "geometry_processing/gauss_curvature.hpp"

#include "igl/gaussian_curvature.h"
#include "igl/readOBJ.h"

void GaussCurvature::perform(VklObject &model) {
    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
    Eigen::VectorXd K;

    // igl::readOBJ()
    if (not model.dataFilePath.has_value())
        return;

    igl::readOBJ(model.dataFilePath.value(), V, F);

    igl::gaussian_curvature(V, F, K);
}