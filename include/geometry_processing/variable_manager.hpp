#pragma once

#include "quantity/vertex_scalar_quantity.hpp"
#include "quantity/vertex_vector_quantity.hpp"

#include "vkl/scene/vkl_model.hpp"

#include "map_concept/mesh_to_vertex_scalar_quantity.hpp"
#include "map_concept/single_vertex_to_vertex_scalar_quantity.hpp"

#include <memory>
#include <vector>

class GeometryVariableManager {
  private:
    std::vector<std::unique_ptr<VertexScalarQuantity>> vertexScalarQuantities;
    std::vector<std::unique_ptr<VertexVectorQuantity>> vertexVectorQuantities;

  public:
    template <MeshToVertexScalarQuantityMap Map> void meshToVertexMap(VklModel &model);

    template <MeshToVertexVectorQuantityMap Map> void meshToVectorMap(VklModel &model);

    template <SingleVertexToVertexScalarQuantityMap Map>
    void singleVertexToVertexScalarQuantityMap(VklModel &model, uint32_t vertexIndex);

    std::vector<VertexScalarQuantity *> getModelScalarQuantities(VklModel &model) {
        std::vector<VertexScalarQuantity *> result;

        for (auto &quant : this->vertexScalarQuantities) {
            if (quant->modelPtr == &model) {
                result.push_back(quant.get());
            }
        }
        return result;
    }

    std::vector<VertexVectorQuantity *> getModelVectorQuantities(VklModel &model) {
        std::vector<VertexVectorQuantity *> result;

        for (auto &quant : this->vertexVectorQuantities) {
            if (quant->modelPtr == &model) {
                result.push_back(quant.get());
            }
        }

        return result;
    }
};

#include "variable_manager.hpp.impl"