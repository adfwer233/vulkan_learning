#include "vkl/io/vkl_image_export.hpp"
#include "glm/glm.hpp"
#include <fstream>
#include <sstream>

#include "vkl/core/vkl_buffer.hpp"

#ifdef OPENCV_FOUND
#include <opencv2/opencv.hpp>
#endif

void VklImageExporter::exportToImage(VkImage image, uint32_t width, uint32_t height) {
    VkBuffer imageBuffer;
    VkDeviceMemory imageBufferMemory;

    device_.createBuffer(width * height * 4, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, imageBuffer, imageBufferMemory);

    VkImage dstImage;
    VkDeviceMemory dstImageMemory;

    device_.createImage(width, height, VkFormat::VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_LINEAR,
                        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, dstImage, dstImageMemory);

    device_.transitionImageLayout(dstImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

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

    VkImageCopy imageCopyRegion {
        .srcSubresource = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .layerCount = 1
        },
        .dstSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .layerCount = 1
        },
        .extent {
            .width = width,
            .height = height,
            .depth = 1
        }
    };

    vkCmdCopyImage(
            commandBuffer,
            image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &imageCopyRegion
        );

    auto imageBarrier2 = VklImageUtils::transferSrcToReadOnlyBarrier(image);

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0,
                         nullptr, 0, nullptr, 1, &imageBarrier2);

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

    VkSubmitInfo submitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer
    };

    vkQueueSubmit(device_.graphicsQueue(), 1, &submitInfo, fence);
    vkQueueWaitIdle(device_.graphicsQueue());

    vkFreeCommandBuffers(device_.device(), device_.getCommandPool(), 1, &commandBuffer);

    assert(vkWaitForFences(device_.device(), 1, &fence, VK_TRUE, UINT64_MAX) == VK_SUCCESS);

    struct color {
        char r, g, b, a;
    };

    char* imageData;

    VkImageSubresource subResource{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 0 };
    VkSubresourceLayout subResourceLayout;
    vkGetImageSubresourceLayout(device_.device(), dstImage, &subResource, &subResourceLayout);

    assert(vkMapMemory(device_.device(), dstImageMemory, 0, VK_WHOLE_SIZE,
                0, (void **) &imageData) == VK_SUCCESS);

    imageData += subResourceLayout.offset;

    #ifdef OPENCV_FOUND
        cv::Mat cv_image(width, height, CV_8UC4);

        std::memcpy(cv_image.data, imageData, width * height * 4 * sizeof(char));

        // Copy the BGRA data to the cv::Mat with correct channel order
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                cv_image.at<cv::Vec4b>(y, x)[0] = imageData[(y * width + x) * 4 + 2]; // Blue
                cv_image.at<cv::Vec4b>(y, x)[1] = imageData[(y * width + x) * 4 + 1]; // Green
                cv_image.at<cv::Vec4b>(y, x)[2] = imageData[(y * width + x) * 4    ]; // Red
                cv_image.at<cv::Vec4b>(y, x)[3] = imageData[(y * width + x) * 4 + 3]; // Alpha
            }
        }

        // Save the image into a PNG file
        bool success = cv::imwrite("output_image.png", cv_image);

        if (!success) {
            std::cerr << "Error: Couldn't write the image to a PNG file." << std::endl;
        }

        std::cout << "Image saved successfully as output_image.png" << std::endl;
    #elif
        std::ofstream file("render.ppm", std::ios::out);

        file << "P3\n" << height << ' ' << width << "\n255\n";


        std::stringstream ss;

        for (auto i = 0; i < height; i++) {
            for (auto j = 0; j < width; j++) {
                auto pixel = (color *)(imageData + (i * width + j) * 4);
                int x = 0;
                // pixel.r *= 255.99;
                // pixel.g *= 255.99;
                // pixel.b *= 255.99;
                ss << unsigned (pixel->r) << ' ' << unsigned(pixel->g) << ' ' << unsigned(pixel->b) << ' ' << unsigned(pixel->a) << '\n';
            }
        }
    #endif
}