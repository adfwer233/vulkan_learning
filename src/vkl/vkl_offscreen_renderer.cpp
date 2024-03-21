#include "vkl/vkl_offscreen_renderer.hpp"

#include "vkl/vkl_swap_chain.hpp"

VklOffscreenRenderer::VklOffscreenRenderer(VklDevice &device, int width, int height) {
    createImages();
    createRenderPass();
    createFrameBuffer();
}

void VklOffscreenRenderer::createImages() {
    images_.resize(VklSwapChain::MAX_FRAMES_IN_FLIGHT);
    memory_.resize(VklSwapChain::MAX_FRAMES_IN_FLIGHT);
    imageViews_.resize(VklSwapChain::MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < VklSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
        device_.createImage(1024, 1024, VkFormat::VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->images_[i], this->memory_[i]);
        imageViews_[i] = device_.createImageView(this->images_[i], VkFormat::VK_FORMAT_R8G8B8A8_UNORM);
    }
}

void VklOffscreenRenderer::createRenderPass() {

}
