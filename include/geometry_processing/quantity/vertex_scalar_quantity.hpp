#pragma once

#include "Eigen/Eigen"
#include "../map_concept/mesh_to_vertex_scalar_quantity.hpp"

#include "vkl/vkl_model.hpp"

class VertexScalarQuantity {
public:
    Eigen::VectorXd data;
    VklModel *modelPtr;

    template<MeshToVertexScalarQuantityMap Map>
    explicit VertexScalarQuantity(VklModel& model, Map* map);
};

#include "vertex_scalar_quantity.hpp.impl"