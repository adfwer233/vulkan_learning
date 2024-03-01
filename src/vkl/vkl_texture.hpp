#pragma once

#include "vkl_device.hpp"

class VklTexture {
private:
    int texWidth_, texHeight_, texChannels_;

    VklDevice *device_;

public:
    VkImage image_ = VK_NULL_HANDLE;
    VkDeviceMemory memory_ = VK_NULL_HANDLE;

    VklTexture(VklDevice &device_, int texWidth, int texHeight, int texChannels);
};