#pragma once

#include <vector>

#include "vkl_device.hpp"

class VklOffscreenRenderer {
    VklDevice &device_;
    std::vector<VkCommandBuffer> commandBuffers_;

    uint32_t currentImageIndex;
    int currentFrameIndex{0};
    bool isFrameStarted{false};

    std::vector<VkImage> images_;
    std::vector<VkDeviceMemory> memory_;
    std::vector<VkImageView> imageViews_;

    VkRenderPass renderPass_;

    void createImages();
    void createRenderPass();
    void createFrameBuffer();

  public:

    VklOffscreenRenderer(VklDevice &device, int width, int height);
};
