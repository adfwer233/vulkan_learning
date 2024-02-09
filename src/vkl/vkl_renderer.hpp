#pragma once

#include "vkl_device.hpp"
#include "vkl_swap_chain.hpp"
#include <cassert>

class VklRenderer {
  private:
    VklWindow &window_;
    VklDevice &device_;
    std::unique_ptr<VklSwapChain> swapChain_;
    std::vector<VkCommandBuffer> commandBuffers_;

    uint32_t currentImageIndex;
    int currentFrameIndex{0};
    bool isFrameStarted{false};

    void createCommandBuffers();
    void recreateSwapChain();

  public:
    VklRenderer(VklWindow &window, VklDevice &device);
    ~VklRenderer();

    VklRenderer(const VklRenderer &) = delete;
    VklRenderer &operator=(const VklRenderer &) = delete;

    VkRenderPass getSwapChainRenderPass() const {
        return swapChain_->getRenderPass();
    }
    float getAspectRatio() const {
        return swapChain_->extentAspectRatio();
    }
    bool isFrameInProgress() const {
        return isFrameStarted;
    }

    VkCommandBuffer getCurrentCommandBuffer() const {
        assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
        return commandBuffers_[currentFrameIndex];
    }

    int getFrameIndex() const {
        assert(isFrameStarted && "Cannot get frame index when frame not in progress");
        return currentFrameIndex;
    }

    VkCommandBuffer beginFrame();
    void endFrame();
    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
};