#include "vkl_swap_chain.hpp"

#include <array>
#include <iostream>

VklSwapChain::VklSwapChain(VklDevice &device, VkExtent2D windowExtent) : device_(device), windowExtent_(windowExtent) {
    init();
}

VklSwapChain::VklSwapChain(VklDevice &device, VkExtent2D windowExtent, std::shared_ptr<VklSwapChain> previous)
    : device_(device), windowExtent_(windowExtent), oldSwapChain_(previous) {
    init();
    oldSwapChain_ = nullptr;
}

VklSwapChain::~VklSwapChain() {
    for (auto imageView : swapChainImageViews_) {
        vkDestroyImageView(device_.device(), imageView, nullptr);
    }

    for (auto frameBuffer : swapChainFrameBuffers_) {
        vkDestroyFramebuffer(device_.device(), frameBuffer, nullptr);
    }

    vkDestroyRenderPass(device_.device(), renderPass_, nullptr);

    vkDestroySwapchainKHR(device_.device(), swapChain_, nullptr);
}

void VklSwapChain::init() {
    createSwapChain();
    createImageView();
    createRenderPass();
    createFrameBuffers();
    createSyncObjects();
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

    VkSwapchainCreateInfoKHR createInfo{};
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

void VklSwapChain::createImageView() {
    swapChainImageViews_.resize(swapChainImages_.size());

    for (size_t i = 0; i < swapChainImages_.size(); i++) {
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = swapChainImages_[i];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = swapChainImageFormat_;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device_.device(), &imageViewCreateInfo, nullptr, &swapChainImageViews_[i]) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }
    }
}

void VklSwapChain::createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat_;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

    /*
     * what to do before and after rendering
     */
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = nullptr;

    std::array<VkAttachmentDescription, 1> attachments = {colorAttachment};

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 0;
    renderPassInfo.pDependencies = nullptr;

    if (vkCreateRenderPass(device_.device(), &renderPassInfo, nullptr, &renderPass_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void VklSwapChain::createFrameBuffers() {
    swapChainFrameBuffers_.resize(swapChainImages_.size());

    for (size_t i = 0; i < swapChainImages_.size(); i++) {
        std::array<VkImageView, 1> attachments = {swapChainImageViews_[i]};

        VkExtent2D swapChainExtent = swapChainExtent_;
        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass_;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device_.device(), &framebufferInfo, nullptr, &swapChainFrameBuffers_[i]) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void VklSwapChain::createSyncObjects() {
    imageAvailableSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences_.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight_.resize(swapChainImages_.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(device_.device(), &semaphoreInfo, nullptr, &imageAvailableSemaphores_[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device_.device(), &semaphoreInfo, nullptr, &renderFinishedSemaphores_[i]) != VK_SUCCESS ||
            vkCreateFence(device_.device(), &fenceInfo, nullptr, &inFlightFences_[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

VkResult VklSwapChain::acquireNextImage(uint32_t *imageIndex) {
    vkWaitForFences(device_.device(), 1, &inFlightFences_[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

    VkResult result = vkAcquireNextImageKHR(device_.device(), swapChain_, std::numeric_limits<uint64_t>::max(),
                                            imageAvailableSemaphores_[currentFrame], VK_NULL_HANDLE, imageIndex);

    return result;
}

VkResult VklSwapChain::submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex) {
    if (imagesInFlight_[*imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(device_.device(), 1, &imagesInFlight_[*imageIndex], VK_TRUE, UINT64_MAX);
    }
    imagesInFlight_[*imageIndex] = inFlightFences_[currentFrame];

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores_[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = buffers;

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores_[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(device_.device(), 1, &inFlightFences_[currentFrame]);

    if (vkQueueSubmit(device_.graphicsQueue(), 1, &submitInfo, inFlightFences_[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain_};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = imageIndex;

    auto result = vkQueuePresentKHR(device_.presentQueue(), &presentInfo);

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;
}
