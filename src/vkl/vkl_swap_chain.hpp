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

    std::vector<VkImage> swapChainImages_;

    VkFormat swapChainImageFormat_;
    VkExtent2D swapChainExtent_;

    void init();
    void createSwapChain();

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

    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
};