#include "geometry_processing/map/single_source_exact_geodesic_distance.hpp"

#include "igl/exact_geodesic.h"

SingleSourceExactGeodesicDistance::SingleSourceExactGeodesicDistance(VklModel &model) {
    int n = model.vertices_.size();

    V.resize(n, 3);
    for (int i = 0; i < n; i++) {
        V(i, 0) = model.vertices_[i].position.x;
        V(i, 1) = model.vertices_[i].position.y;
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

Eigen::VectorXd SingleSourceExactGeodesicDistance::perform(uint32_t vertexIndex) {
    Eigen::VectorXi VS, FS, VT, FT;

    VS.resize(1);

    VS << vertexIndex;
    VT.setLinSpaced(V.rows(), 0, V.rows() - 1);

    Eigen::VectorXd d;
    igl::exact_geodesic(V, F, VS, FS, VT, FT, d);
    return d;
}