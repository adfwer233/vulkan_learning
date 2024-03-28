#pragma once

#include "../map_concept/mesh_to_vertex_scalar_quantity.hpp"
#include "../map_concept/single_vertex_to_vertex_scalar_quantity.hpp"
#include "Eigen/Eigen"

#include "vkl/vkl_model.hpp"

class VertexScalarQuantity {
  public:
    Eigen::VectorXd data;
    VklModel *modelPtr;
    std::string description;

    template <MeshToVertexScalarQuantityMap Map> explicit VertexScalarQuantity(VklModel &model, Map *map);

    template <SingleVertexToVertexScalarQuantityMap Map>
    explicit VertexScalarQuantity(VklModel &model, uint32_t vertexIndex, Map *map);
};

#include "vertex_scalar_quantity.hpp.impl"