#pragma once

#include "../vkl_device.hpp"
#include "../vkl_buffer.hpp"
#include "../vkl_compute_pipeline.hpp"

template<typename T>
struct ComputeDescriptor {
    T *data;
    VkShaderStageFlags shaderStageFlags;
};

template <typename T>
concept VklComputeModel = requires(T t) {
    {t.getUniformBufferDescriptors()} -> std::same_as<std::vector<ComputeDescriptor<VklBuffer>>>;
    {t.getTextureDescriptors()} -> std::same_as<std::vector<ComputeDescriptor<VklTexture>>>;
    {t.getStorageDescriptor()} -> std::same_as<std::vector<ComputeDescriptor<VklBuffer>>>;
    {t.getSize()} -> std::same_as<std::tuple<int, int, int>>;
    {t.getLocalSize()} -> std::same_as<std::tuple<int, int, int>>;
};

template <typename UniformBufferType, VklComputeModel ComputeModelType>
class BaseComputeSystem {
private:

    VklDevice &device_;
    std::unique_ptr<VklComputePipeline> pipeline_;
    VkPipelineLayout pipelineLayout_;

    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout computeDescriptorSetLayout;
    std::vector<VkDescriptorSet> computeDescriptorSets;

    std::vector<VkCommandBuffer> computeCommandBuffers;

    void createComputeDescriptorSetLayout();
    void createPipelineLayout();
    void createPipeline();
    void createUniformBuffers();
    void createDescriptorPool();
    void createComputeDescriptorSets();
    void createCommandBuffer();
    void createSyncObjects();

    void updateUniformBuffer(uint32_t frameIndex);

    std::vector<std::unique_ptr<VklBuffer>> uniformBuffers_;

    void recordComputeCommandBuffer(size_t frameIndex);

    ComputeModelType computeModel_;

    std::vector<ComputeDescriptor<VklBuffer>> uniformBufferDescriptors_;
    std::vector<ComputeDescriptor<VklBuffer>> storageBufferDescriptors_;
    std::vector<ComputeDescriptor<VklTexture>> textureDescriptors_;
    // std::vector<ComputeDescriptor<>>

public:
    std::vector<VkSemaphore> computeFinishedSemaphores;
    std::vector<VkFence> computeInFlightFences;

    explicit BaseComputeSystem(VklDevice &device, ComputeModelType computeModel);
    ~BaseComputeSystem();

    BaseComputeSystem(const BaseComputeSystem &) = delete;
    BaseComputeSystem operator=(const BaseComputeSystem &) = delete;

    void computeSubmission(size_t frameIndex);
};

#include "base_compute_system.hpp.impl"