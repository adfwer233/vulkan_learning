#include "geometry_processing/gauss_curvature.hpp"

#include "igl/gaussian_curvature.h"
#include "igl/readOBJ.h"
#include "igl/massmatrix.h"
#include "igl/invert_diag.h"

void GaussCurvature::perform(VklObject &object) {
    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
    Eigen::VectorXd K;

    // igl::readOBJ()
    if (not object.dataFilePath.has_value())
        return;

//    igl::readOBJ(object.dataFilePath.value(), V, F);
    auto model = object.models[0];
    int n = model->vertices_.size();

    V.resize(n, 3);
    for (int i = 0; i < n; i++) {
        V(i, 0) = model->vertices_[i].position.x;
        V(i, 1) = model->vertices_[i].position.y;
        V(i, 2) = model->vertices_[i].position.z;
    }

    int nf = model->indices_.size();
    F.resize(nf, 3);
    for (int i = 0; i < nf; i++) {
        F(i, 0) = model->indices_[i].i;
        F(i, 1) = model->indices_[i].j;
        F(i, 2) = model->indices_[i].k;
    }

    igl::gaussian_curvature(V, F, K);

    auto max_curvature = K.maxCoeff();
    auto min_curvature = K.minCoeff();

    auto bound = std::max(std::abs(max_curvature), std::abs(min_curvature));

    if (object.models.size() == 1) {
        auto model = object.models[0];

        int n = K.size();
        for (int i = 0; i < n; i++) {
            model->vertices_[i].color = glm::vec4(float(K[i] - min_curvature) * 1.0f, 0.0f, 0.0f, 1.0f) / float(max_curvature - min_curvature);
        }

        model->updateVertexBuffer();
    }
}