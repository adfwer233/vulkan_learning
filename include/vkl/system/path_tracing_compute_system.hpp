#pragma once

#include "vkl/vkl_device.hpp"
#include "vkl/vkl_buffer.hpp"
#include "vkl/vkl_compute_pipeline.hpp"

class PathTracingComputeSystem {

public:

private:

    VklDevice &device;
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

    void updateUniformBuffer(uint32_t frameIndex, float deltaTime);

    std::vector<std::unique_ptr<VklBuffer>> uniformBuffers_;

    void recordComputeCommandBuffer(size_t frameIndex);

public:
    std::vector<VkSemaphore> computeFinishedSemaphores;
    std::vector<VkFence> computeInFlightFences;

    PathTracingComputeSystem(VklDevice &device);
    ~PathTracingComputeSystem();

    PathTracingComputeSystem(const PathTracingComputeSystem &) = delete;
    PathTracingComputeSystem operator=(const PathTracingComputeSystem &) = delete;

    void computeSubmission(size_t frameIndex, float deltaTime);
};