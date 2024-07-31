#pragma once

#include <vector>

#include "../io/vkl_image_export.hpp"
#include "vkl_device.hpp"

class VklOffscreenRenderer {
    VklDevice &device_;
    std::vector<VkCommandBuffer> commandBuffers_;

    uint32_t currentImageIndex{0};
    int currentFrameIndex{0};
    bool isFrameStarted{false};

    std::vector<VkImage> images_;
    std::vector<VkDeviceMemory> memory_;
    std::vector<VkImageView> imageViews_;

    std::vector<VkImage> depthImages_;
    std::vector<VkDeviceMemory> depthImageMemories_;
    std::vector<VkImageView> depthImageViews_;

    std::vector<VkFramebuffer> framebuffers_;

    VkRenderPass renderPass_;

    VklImageExporter imageExporter;

    void createCommandBuffers();
    void createImages();
    void createDepthResources();
    void createRenderPass(uint32_t subpassNum);
    void createFrameBuffer();

  public:
    VkSampler imageSampler;

    VklOffscreenRenderer(VklDevice &device, int width, int height, uint32_t subpassNum = 1);

    ~VklOffscreenRenderer();

    [[nodiscard]] VkRenderPass getSwapChainRenderPass() const {
        return renderPass_;
    }

    [[nodiscard]] bool isFrameInProgress() const {
        return isFrameStarted;
    }

    [[nodiscard]] VkCommandBuffer getCurrentCommandBuffer() const {
        assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
        return commandBuffers_[currentFrameIndex];
    }

    [[nodiscard]] int getFrameIndex() const {
        // assert(isFrameStarted && "Cannot get frame index when frame not in progress");
        return currentFrameIndex;
    }

    auto getImageView() {
        return &imageViews_;
    }

    VkCommandBuffer beginFrame();
    void endFrame();
    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    void exportCurrentImageToPPM();
};
