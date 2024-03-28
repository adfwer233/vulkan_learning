#pragma once

#include "quantity/vertex_scalar_quantity.hpp"
#include "quantity/vertex_vector_quantity.hpp"

#include "vkl/vkl_model.hpp"

#include "map_concept/mesh_to_vertex_scalar_quantity.hpp"

#include <memory>
#include <vector>

class GeometryVariableManager {
private:
    std::vector<std::unique_ptr<VertexScalarQuantity>> vertexScalarQuantities;
    std::vector<std::unique_ptr<VertexVertexQuantity>> vertexVectorQuantities;

public:

    template<MeshToVertexScalarQuantityMap Map>
    void meshToVertexMap(VklModel &model);

    std::vector<VertexScalarQuantity*> getModelScalarQuantities(VklModel &model) {
        std::vector<VertexScalarQuantity*> result;

        for(auto &quant: this->vertexScalarQuantities) {
            if (quant->modelPtr == &model) {
                result.push_back(quant.get());
            }
        }

        return result;
    }
};

#include "variable_manager.hpp.impl"