#pragma once

#include "vkl/core/vkl_device.hpp"

#include <vector>
#include <string>

template<typename T>
concept VklPushConstant = requires {
    {T::getStageFlags()} -> std::same_as<VkShaderStageFlags>;
};

template<VklPushConstant ...args>
struct VklPushConstantInfoList {
    std::array<void*, sizeof...(args)> data;
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