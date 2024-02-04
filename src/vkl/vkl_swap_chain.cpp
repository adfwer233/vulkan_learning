#include "vkl_swap_chain.hpp"

#include <iostream>

VklSwapChain::VklSwapChain(VklDevice &device, VkExtent2D windowExtent): device_(device), windowExtent_(windowExtent) {
    init();
}

VklSwapChain::VklSwapChain(VklDevice &device, VkExtent2D windowExtent, std::shared_ptr<VklSwapChain> previous): device_(device), windowExtent_(windowExtent), oldSwapChain_(previous) {
    init();
    oldSwapChain_ = nullptr;
}

VklSwapChain::~VklSwapChain() {

}

void VklSwapChain::createSwapChain() {
    SwapChainSupportDetails swapChainSupportDetails = device_.getSwapChainSupport();

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupportDetails.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupportDetails.presentModes);
    VkExtent2D extent2D = chooseSwapExtent(swapChainSupportDetails.capabilities);

    uint32_t imageCount = swapChainSupportDetails.capabilities.minImageCount + 1;

    if (swapChainSupportDetails.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupportDetails.capabilities.maxImageCount) {
        imageCount = swapChainSupportDetails.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = device_.surface();

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent2D;

    /* specifies the amount of layers each image consist of, always 1 unless in a stereoscopic 3D application*/
    createInfo.imageArrayLayers = 1;

    /*
     * specifies what kind of operations we’ll use the images in the swap chain for.
     *
     * In this case, we are rendering the image. In some other cases such as post-processing, we can use
     * `VK_IMAGE_USAGE_TRANSFER_DST_BIT`.
     */
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = device_.findPhysicalQueueFamilies();
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily, indices.presentFamily};

    if (indices.graphicsFamily != indices.presentFamily) {
        /* Image can be owned by more than one swap chain*/
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        /* Image can be owned by only one swap chain */
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;     // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupportDetails.capabilities.currentTransform;

    /* if alpha bit should be used */
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = oldSwapChain_ == nullptr ? VK_NULL_HANDLE : oldSwapChain_->swapChain_;

    if (vkCreateSwapchainKHR(device_.device(), &createInfo, nullptr, &swapChain_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(device_.device(), swapChain_, &imageCount, nullptr);
    swapChainImages_.resize(imageCount);
    vkGetSwapchainImagesKHR(device_.device(), swapChain_, &imageCount, swapChainImages_.data());

    swapChainImageFormat_ = surfaceFormat.format;
    swapChainExtent_ = extent2D;
}

void VklSwapChain::init() {
    createSwapChain();
}

VkSurfaceFormatKHR VklSwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
    for (const auto &availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR VklSwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
    for (const auto &availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            std::cout << "Present mode: Mailbox" << std::endl;
            return availablePresentMode;
        }
    }

    std::cout << "Present mode: V-Sync" << std::endl;
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VklSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = windowExtent_;
        actualExtent.width = std::max(capabilities.minImageExtent.width,
                                      std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height,
                                       std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}
