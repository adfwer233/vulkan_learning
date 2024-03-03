#pragma once

template<typename T>
concept VklVertexType = requires {
    T::getBindingDescriptions();
    T::getAttributeDescriptions();
};
