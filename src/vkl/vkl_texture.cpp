#include <stdexcept>

#include "vkl_texture.hpp"

VklTexture::VklTexture(VklDevice &device, int texWidth, int texHeight, int texChannels) : device_(device) {
    if (texChannels == 3) {
        throw std::runtime_error("unsupported texture type \n");
    } else if (texChannels == 4) {
        device_.createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->image_, this->memory_);
        device_.createSampler(this->textureSampler_);
        this->textureImageView = device_.createImageView(image_, VK_FORMAT_R8G8B8A8_SRGB);
    } else {
        throw std::runtime_error("wrong texture channel number \n");
    }
}

VklTexture::~VklTexture() {
    vkDestroyImageView(device_.device(), this->textureImageView, nullptr);
    vkDestroySampler(device_.device(), this->textureSampler_, nullptr);
    vkDestroyImage(device_.device(), this->image_, nullptr);
    vkFreeMemory(device_.device(), this->memory_, nullptr);
}

VkDescriptorImageInfo VklTexture::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
    return VkDescriptorImageInfo(textureSampler_, textureImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}
