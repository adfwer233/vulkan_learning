#pragma once

#include "vkl_device.hpp"

class VklImage {
private:
    VklDevice &device_;
    VkImage image_ = VK_NULL_HANDLE;
};