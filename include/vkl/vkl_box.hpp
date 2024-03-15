#pragma once

#include "vkl_model.hpp"

struct Vertex3DRaw {
    glm::vec3 position{};

    static std::vector<VkVertexInputBindingDescription> getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex3DRaw);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        attributeDescriptions.push_back(
                {0, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex3DRaw, position))});

        return attributeDescriptions;
    }
};

using VklBoxModel = VklModelTemplate<Vertex3DRaw, LineIndex>;

class VklBox {
public:
    glm::vec3 min_position;
    glm::vec3 max_position;

    static VklBoxModel::BuilderFromImmediateData getStandardBox() {
        VklBoxModel::BuilderFromImmediateData builder;

        builder.vertices = std::vector<Vertex3DRaw> {
                {{0, 0, 0}},
                {{0, 1, 0}},
                {{1, 0, 0}},
                {{1, 1, 0}},
                {{0, 0, 1}},
                {{0, 1, 1}},
                {{1, 0, 1}},
                {{1, 1, 1}}
        };

        builder.indices = std::vector<LineIndex> {
                {0, 1}, {1, 3}, {3, 2}, {2, 0},
                {4, 5}, {5, 7}, {7, 6}, {6, 4},
                {0, 4}, {1, 5}, {2, 6}, {3, 7}
        };

        return builder;
    }
};