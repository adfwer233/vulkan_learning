#include "geometry_processing/map/normal_vector.hpp"

#include "igl/per_vertex_normals.h"

NormalVector::NormalVector(VklModel &model) {
    int n = model.vertices_.size();

    V.resize(n, 3);
    for (int i = 0; i < n; i++) {
        V(i, 0) = model.vertices_[i].position.x;
        V(i, 1) = -model.vertices_[i].position.y;
        V(i, 2) = model.vertices_[i].position.z;
    }

    int nf = model.indices_.size();
    F.resize(nf, 3);
    for (int i = 0; i < nf; i++) {
        F(i, 0) = model.indices_[i].i;
        F(i, 1) = model.indices_[i].j;
        F(i, 2) = model.indices_[i].k;
    }
}

Eigen::MatrixXd NormalVector::perform() {
    Eigen::MatrixXd K;

    igl::per_vertex_normals(V, F, K);

    return K;
}