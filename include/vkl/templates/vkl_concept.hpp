#pragma once

template <typename T>
concept VklVertexType = requires {
    T::getBindingDescriptions();
    T::getAttributeDescriptions();
};

template <typename T>
concept VklRenderable = requires(T t, VkCommandBuffer commandBuffer) {
    t.bind(commandBuffer);
    t.draw(commandBuffer);
};

template <typename T>
concept VklDataType = requires {
    T::getBindingDescriptions();
    T::getAttributeDescriptions();
};