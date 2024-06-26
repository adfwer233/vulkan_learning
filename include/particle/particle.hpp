#pragma once

#include "glm/glm.hpp"

#include "vkl/core/vkl_buffer.hpp"
#include "vkl/core/vkl_descriptor.hpp"
#include "vkl/core/vkl_device.hpp"

#include "vkl/utils/vkl_box.hpp"

struct Particle {
    using geometry_type = Particle;
    glm::vec2 position{};
    glm::vec2 velocity{};
    glm::vec4 color{};

    static std::vector<VkVertexInputBindingDescription> getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Particle);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        attributeDescriptions.push_back(
            {0, 0, VK_FORMAT_R32G32_SFLOAT, static_cast<uint32_t>(offsetof(Particle, position))});
        attributeDescriptions.push_back(
            {1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, static_cast<uint32_t>(offsetof(Particle, color))});

        return attributeDescriptions;
    }
};