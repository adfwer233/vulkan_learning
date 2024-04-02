#pragma once

#include "../map_concept/mesh_to_vertex_vector_quantity.hpp"

class VertexVectorQuantity {
  public:
    Eigen::MatrixXd data;
    VklModel *modelPtr;
    std::string description;

    template <MeshToVertexVectorQuantityMap Map> explicit VertexVectorQuantity(VklModel &model, Map *map);
};

#include "vertex_vector_quantity.hpp.impl"
