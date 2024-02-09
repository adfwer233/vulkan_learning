#pragma once

#include "vkl_device.hpp"

#include "memory"

/** \page swapChainPage Vulkan Swap Chain
 * ## Introduction
 *
 * ##
 */

/**
 * @brief Vulkan swap chain encapsulation
 */
class VklSwapChain {
  private:
    /** reference to logical device*/
    VklDevice &device_;

    /** current window extent */
    VkExtent2D windowExtent_;

    VkSwapchainKHR swapChain_;
    std::shared_ptr<VklSwapChain> oldSwapChain_;

    VkRenderPass renderPass_;

    std::vector<VkImage> swapChainImages_;
    std::vector<VkImageView> swapChainImageViews_;

    VkFormat swapChainImageFormat_;
    VkExtent2D swapChainExtent_;

    std::vector<VkFramebuffer> swapChainFrameBuffers_;

    std::vector<VkSemaphore> imageAvailableSemaphores_;
    std::vector<VkSemaphore> renderFinishedSemaphores_;
    std::vector<VkFence> inFlightFences_;
    std::vector<VkFence> imagesInFlight_;

    size_t currentFrame = 0;

    /**
     * @brief initialize swap chain
     */
    void init();

    /**
     * @brief Create swap chain
     */
    void createSwapChain();

    /**
     * @brief Create image view
     */
    void createImageView();

    /**
     * @brief Create render pass (step 3)
     */
    void createRenderPass();

    /**
     * @brief Create frame buffers (step 4)
     */
    void createFrameBuffers();

    /**
     * @brief auxiliary function,  choose swap surface format form a vector
     * @param availableFormats vector of VkSurfaceFormatKHR
     * @return
     */
    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    /**
     * @brief auxiliary function, choose present mode from a vector
     * @param availablePresentModes
     * @return
     */
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

    /**
     * @brief auxiliary function, choose extent from capabilities, `windowExtent_` is clipped by
     * `capabilities.minImageExtent` and `capabilities.maxImageExtent`
     * @param capabilities
     * @return
     */
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  public:
    VklSwapChain(VklDevice &device, VkExtent2D windowExtent);
    VklSwapChain(VklDevice &deviceRef, VkExtent2D windowExtent, std::shared_ptr<VklSwapChain> previous);

    ~VklSwapChain();

    VklSwapChain(const VklSwapChain &) = delete;
    VklSwapChain &operator=(const VklSwapChain &) = delete;

    VkFramebuffer getFrameBuffer(int index) {
        return swapChainFrameBuffers_[index];
    }

    VkRenderPass getRenderPass() {
        return renderPass_;
    }

    VkImageView getImageView(int index) {
        return swapChainImageViews_[index];
    }

    float extentAspectRatio() {
        return static_cast<float>(swapChainExtent_.width) / static_cast<float>(swapChainExtent_.height);
    }

    VkExtent2D getSwapChainExtent() {
        return swapChainExtent_;
    }

    VkResult acquireNextImage(uint32_t *imageIndex);
    VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
};