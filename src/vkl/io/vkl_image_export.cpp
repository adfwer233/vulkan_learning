#include "vkl/io/vkl_image_export.hpp"
#include "glm/glm.hpp"
#include <fstream>
#include <sstream>

#include "vkl/core/vkl_buffer.hpp"

void VklImageExporter::exportToImage(VkImage image, uint32_t width, uint32_t height) {
    VkBuffer imageBuffer;
    VkDeviceMemory imageBufferMemory;

    device_.createBuffer(width * height * 4, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, imageBuffer, imageBufferMemory);

    VkCommandBuffer commandBuffer = device_.beginSingleTimeCommands();

    auto imageBarrier = VklImageUtils::readOnlyToSrcBarrier(image);

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_HOST_BIT, 0, 0,
                         nullptr, 0, nullptr, 1, &imageBarrier);

    VkBufferImageCopy copy = {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
        .imageOffset = {0, 0, 0},
        .imageExtent = {
            .width = width,
            .height = height,
            .depth = 1
        },
    };

    vkCmdCopyImageToBuffer(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           imageBuffer, 1, &copy);

    VkBufferMemoryBarrier transferBarrier = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        .pNext = nullptr,
        .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_HOST_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .buffer = imageBuffer,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
    };

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_HOST_BIT, 0, 0,
                         nullptr, 1, &transferBarrier, 0, nullptr);

    imageBarrier = VklImageUtils::transferSrcToReadOnlyBarrier(image);

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0,
                         nullptr, 0, nullptr, 1, &imageBarrier);

    VkFence fence;
    {
        VkFenceCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = 0,
            .flags = 0,
        };
        assert(vkCreateFence(device_.device(), &createInfo, NULL, &fence) == VK_SUCCESS);
    }

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(device_.graphicsQueue(), 1, &submitInfo, fence);
    vkQueueWaitIdle(device_.graphicsQueue());

    vkFreeCommandBuffers(device_.device(), device_.getCommandPool(), 1, &commandBuffer);

    assert(vkWaitForFences(device_.device(), 1, &fence, VK_TRUE, UINT64_MAX) == VK_SUCCESS);

    struct color {
        int r, g, b, a;
    };

    color* imageData;

    assert(vkMapMemory(device_.device(), imageBufferMemory, 0, VK_WHOLE_SIZE,
                0, (void **) &imageData) == VK_SUCCESS);

    VkMappedMemoryRange flushRange = {
        .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
        .pNext = NULL,
        .memory = imageBufferMemory,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
    };


    assert(vkInvalidateMappedMemoryRanges(device_.device(), 1, &flushRange) == VK_SUCCESS);

    std::ofstream file("render.ppm", std::ios::out);

    file << "P3\n" << height << ' ' << width << "\n255\n";


    std::stringstream ss;

    for (auto i = 0; i < height; i++) {
        for (auto j = 0; j < width; j++) {
            auto pixel = imageData[i * width + j];
            pixel.r *= 255.99;
            pixel.g *= 255.99;
            pixel.b *= 255.99;
            ss << int(pixel.r) << ' ' << int(pixel.g) << ' ' << int(pixel.b) << '\n';
        }
    }

    file << ss.str();
    file.close();
}