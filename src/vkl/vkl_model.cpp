#include "vkl/vkl_model.hpp"
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "vkl/vkl_frame_info.hpp"
#include "vkl/vkl_swap_chain.hpp"
#include "vkl/vkl_texture.hpp"

#include <iostream>

VklModel::VklModel(VklDevice &device, VklModel::BuilderFromImmediateData builder)
    : device_(device), vertices_(builder.vertices), indices_(builder.indices) {
    createVertexBuffers(builder.vertices);
    createIndexBuffers(builder.indices);

    for (const auto &path : builder.texturePaths) {
        createTextureImage(path);
    }
}

VklModel::~VklModel() {
    /** free texture objects */
    for (auto texture : textures_) {
        delete texture;
    }
}

void VklModel::createVertexBuffers(const std::vector<Vertex> &vertices) {
    vertexCount_ = static_cast<uint32_t>(vertices.size());
    uint32_t vertexSize = sizeof(std::remove_reference<decltype(vertices)>::type::value_type);
    VkDeviceSize bufferSize = vertexSize * vertexCount_;

    VklBuffer stagingBuffer{device_, vertexSize, vertexCount_, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void *)vertices.data());

    vertexBuffer_ = std::make_unique<VklBuffer>(device_, vertexSize, vertexCount_,
                                                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    device_.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer_->getBuffer(), bufferSize);
}

void VklModel::createIndexBuffers(const std::vector<FaceIndices> &indices) {
    indexCount_ = static_cast<uint32_t>(indices.size()) * 3;
    hasIndexBuffer = indexCount_ > 0;

    if (not hasIndexBuffer)
        return;

    uint32_t indexSize = sizeof(uint32_t);
    VkDeviceSize bufferSize = indexSize * indexCount_;

    VklBuffer stagingBuffer{device_, indexSize, indexCount_, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void *)indices.data());

    indexBuffer_ = std::make_unique<VklBuffer>(device_, indexSize, indexCount_,
                                               VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    device_.copyBuffer(stagingBuffer.getBuffer(), indexBuffer_->getBuffer(), bufferSize);
}

void VklModel::createTextureImage(const std::string &texturePath) {
    int texWidth, texHeight, texChannels;
    stbi_uc *pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * texChannels;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    if (texChannels == 3) {
        throw std::runtime_error("unsupported texture info");
    }

    VklBuffer stagingBuffer{device_, imageSize, 1, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};
    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void *)pixels);
    stagingBuffer.unmap();

    auto texture = new VklTexture(device_, texWidth, texHeight, texChannels);

    device_.transitionImageLayout(texture->image_, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    device_.copyBufferToImage(stagingBuffer.getBuffer(), texture->image_, static_cast<uint32_t>(texWidth),
                              static_cast<uint32_t>(texHeight), 1);
    device_.transitionImageLayout(texture->image_, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    this->textures_.push_back(texture);

    stbi_image_free(pixels);
}

void VklModel::bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = {vertexBuffer_->getBuffer()};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

    if (hasIndexBuffer) {
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer_->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
    }
}

void VklModel::draw(VkCommandBuffer commandBuffer) {
    if (hasIndexBuffer) {
        vkCmdDrawIndexed(commandBuffer, indexCount_, 1, 0, 0, 0);
    } else {
        vkCmdDraw(commandBuffer, vertexCount_, 1, 0, 0);
    }
}

void VklModel::allocDescriptorSets(VklDescriptorSetLayout &setLayout, VklDescriptorPool &pool) {
    uniformBuffers.resize(VklSwapChain::MAX_FRAMES_IN_FLIGHT);
    descriptorSets.resize(VklSwapChain::MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < uniformBuffers.size(); i++) {
        uniformBuffers[i] = std::make_unique<VklBuffer>(
            device_, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uniformBuffers[i]->map();
    }

    for (int i = 0; i < descriptorSets.size(); i++) {
        auto bufferInfo = uniformBuffers[i]->descriptorInfo();
        auto imageInfo = textures_[0]->descriptorInfo();
        VklDescriptorWriter(setLayout, pool)
            .writeBuffer(0, &bufferInfo)
            .writeImage(1, &imageInfo)
            .build(descriptorSets[i]);
    }
}

int VklModel::get_triangle_num() const {
    int result = 0;
    result += vertexCount_ / 3;
    return result;
}

std::vector<VkVertexInputBindingDescription> VklModel::Vertex::getBindingDescriptions() {
    std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions[0].binding = 0;
    bindingDescriptions[0].stride = sizeof(Vertex);
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> VklModel::Vertex::getAttributeDescriptions() {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

    attributeDescriptions.push_back(
        {0, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex, position))});
    attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex, color))});
    attributeDescriptions.push_back(
        {2, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex, normal))});
    attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex, uv))});

    return attributeDescriptions;
}
