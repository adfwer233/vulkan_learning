#pragma once

#include "vkl_device.hpp"

namespace VklImageUtils {
VkImageMemoryBarrier ReadOnlyToGeneralBarrier(const VkImage &image) {
    VkImageMemoryBarrier memoryBarrier = {};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    memoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    memoryBarrier.image = image;
    memoryBarrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    memoryBarrier.srcAccessMask = 0;
    memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    return memoryBarrier;
}

VkImageMemoryBarrier generalToTransferDstBarrier(const VkImage &image) {
    VkImageMemoryBarrier memoryBarrier = {};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
    memoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    memoryBarrier.image = image;
    memoryBarrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    memoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    return memoryBarrier;
}

VkImageMemoryBarrier generalToTransferSrcBarrier(const VkImage &image) {
    VkImageMemoryBarrier memoryBarrier = {};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
    memoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    memoryBarrier.image = image;
    memoryBarrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    memoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    return memoryBarrier;
}

VkImageMemoryBarrier transferDstToGeneralBarrier(const VkImage &image) {
    VkImageMemoryBarrier memoryBarrier = {};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    memoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    memoryBarrier.image = image;
    memoryBarrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    return memoryBarrier;
}

VkImageMemoryBarrier transferSrcToReadOnlyBarrier(const VkImage &image) {
    VkImageMemoryBarrier memoryBarrier = {};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    memoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    memoryBarrier.image = image;
    memoryBarrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    return memoryBarrier;
}

VkImageCopy imageCopyRegion(uint32_t width, uint32_t height) {
    VkImageCopy region;
    region.dstOffset = {0, 0, 0};
    region.srcOffset = {0, 0, 0};
    region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.dstSubresource.mipLevel = 0;
    region.dstSubresource.baseArrayLayer = 0;
    region.dstSubresource.layerCount = 1;
    region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.srcSubresource.mipLevel = 0;
    region.srcSubresource.baseArrayLayer = 0;
    region.srcSubresource.layerCount = 1;
    region.extent = {width, height, 1};
    return region;
}
} // namespace VklImageUtils

class VklImage {
  private:
    VklDevice &device_;

  public:
    VkImage image_ = VK_NULL_HANDLE;
    VkDeviceMemory memory_ = VK_NULL_HANDLE;
    VkImageView imageView;
    uint32_t width;
    uint32_t height;

    VklImage(VklDevice &device, uint32_t width, uint32_t height, VkImageUsageFlags usage) : device_(device) {
        device_.createImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, usage,
                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->image_, this->memory_);
        this->imageView = device_.createImageView(image_, VK_FORMAT_R8G8B8A8_UNORM);
    }

    VkDescriptorImageInfo descriptorInfo(VkImageLayout imageLayout) {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = imageLayout;
        imageInfo.imageView = imageView;

        return imageInfo;
    }

    ~VklImage();
};