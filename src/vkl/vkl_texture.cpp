#include <stdexcept>

#include "vkl_texture.hpp"

VklTexture::VklTexture(VklDevice &device_, int texWidth, int texHeight, int texChannels) {
    if (texChannels == 3) {
        throw std::runtime_error("unsupported texture type \n");
    } else if (texChannels == 4) {
        device_.createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->image_, this->memory_);
    } else {
        throw std::runtime_error("wrong texture channel number \n");
    }

}
