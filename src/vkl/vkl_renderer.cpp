#include "vkl_renderer.hpp"
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