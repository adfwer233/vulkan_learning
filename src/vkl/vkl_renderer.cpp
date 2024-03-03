#include "vkl/vkl_renderer.hpp"

#include <array>
#include <iostream>
#include <stdexcept>

VklRenderer::VklRenderer(VklWindow &window, VklDevice &device) : window_(window), device_(device) {
    recreateSwapChain();
    createCommandBuffers();
}

VklRenderer::~VklRenderer() {
    vkFreeCommandBuffers(device_.device(), device_.getCommandPool(), static_cast<uint32_t>(commandBuffers_.size()),
                         commandBuffers_.data());
    commandBuffers_.clear();
}

void VklRenderer::recreateSwapChain() {
    swapChain_ = std::make_unique<VklSwapChain>(device_, window_.getExtent());
}

void VklRenderer::createCommandBuffers() {
    commandBuffers_.resize(VklSwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocateInfo;
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool = device_.getCommandPool();
    allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers_.size());
    allocateInfo.pNext = nullptr;

    if (vkAllocateCommandBuffers(device_.device(), &allocateInfo, commandBuffers_.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

VkCommandBuffer VklRenderer::beginFrame() {
    assert(!isFrameStarted && "Can't call beginFrame while already in progress");

    auto result = swapChain_->acquireNextImage(&currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    isFrameStarted = true;

    auto commandBuffer = getCurrentCommandBuffer();
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
    return commandBuffer;
}

void VklRenderer::endFrame() {
    auto commandBuffer = getCurrentCommandBuffer();
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

    auto result = swapChain_->submitCommandBuffers(&commandBuffer, &currentImageIndex, this->semaphoreToWait);
    //    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
    //        window_.wasWindowResized()) {
    //        window_.resetWindowResizedFlag();
    //        recreateSwapChain();
    //    } else if (result != VK_SUCCESS) {
    //        throw std::runtime_error("failed to present swap chain image!");
    //    }

    isFrameStarted = false;
    currentFrameIndex = (currentFrameIndex + 1) % VklSwapChain::MAX_FRAMES_IN_FLIGHT;
}

void VklRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = swapChain_->getRenderPass();
    renderPassInfo.framebuffer = swapChain_->getFrameBuffer(currentImageIndex);

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChain_->getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChain_->getSwapChainExtent().width);
    viewport.height = static_cast<float>(swapChain_->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, swapChain_->getSwapChainExtent()};
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void VklRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    vkCmdEndRenderPass(commandBuffer);
}

void VklRenderer::setSemaphoreToWait(VkSemaphore semaphore) {
    this->semaphoreToWait.clear();
    this->semaphoreToWait.push_back(semaphore);
}
