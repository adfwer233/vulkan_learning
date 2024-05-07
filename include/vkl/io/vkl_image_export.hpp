#pragma once

#include <memory>

#include "../core/vkl_image.hpp"
#include "../core/vkl_descriptor.hpp"

class VklImageExporter {
    VklDevice &device_;

  public:
    explicit VklImageExporter(VklDevice &device): device_(device) {}

    void exportToImage(VkImage image, uint32_t width, uint32_t height);
};