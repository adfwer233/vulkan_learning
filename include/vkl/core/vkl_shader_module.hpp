#pragma once

#include <array>
#include <string>
#include <vector>

#include "vkl_device.hpp"

struct VklShaderModuleInfo {
    struct IsVklShaderModuleInfo;
    std::string shaderPath_;
    VkShaderStageFlagBits shaderStage_;

    VklShaderModuleInfo(const std::string &path, VkShaderStageFlagBits stage): shaderPath_(path), shaderStage_(stage) {}
};
