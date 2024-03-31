#pragma once

#include "vkl/core/vkl_device.hpp"

#include <vector>
#include <string>
#include <concepts>
#include <array>
#include <format>
#include <type_traits>
#include <variant>

template<typename T>
concept VklPushConstant = requires {
    {T::getStageFlags()} -> std::same_as<VkShaderStageFlags>;
};

template<int N, typename... Types>
using NthTypeOf = typename std::tuple_element<N, std::tuple<Types...>>::type;

template<VklPushConstant ...args>
struct VklPushConstantInfoList {
private:
    std::array<void*, sizeof...(args)> rawPointers;

    template<typename T, T ...indices>
    constexpr void getDataRawPointerLoop(std::integer_sequence<T, indices...>);

public:
    std::array<std::variant<args...>, sizeof...(args)> data;

    std::array<void*, sizeof...(args)> getDataRawPointer();
    static std::vector<VkPushConstantRange> getPushConstantInfo();
};

template<typename T>
concept VklPushConstantInfoListConcept = requires {
    {T::getPushConstantInfo()} -> std::same_as<std::vector<VkPushConstantRange>>;
};

struct SimplePushConstantData {
    glm::mat4 modelMatrix{1.f};
    glm::mat4 normalMatrix{1.f};

    static VkShaderStageFlags getStageFlags() {
        return VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    }
};

using SimplePushConstantInfoList = VklPushConstantInfoList<SimplePushConstantData>;

#include "vkl_push_constant.hpp.impl"