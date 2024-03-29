#pragma once

#include <format>
#include <string>

#include "vkl/core/vkl_frame_info.hpp"
#include "vkl/core/vkl_graphics_pipeline.hpp"

#include "particle/particle.hpp"
#include "vkl/templates/vkl_concept.hpp"

#include "vkl/core/vkl_compute_pipeline.hpp"

#ifndef PARTICLE_SHADER_DIR
#define PARTICLE_SHADER_DIR "./shader/"
#endif

class ParticleSimulationSystem {
  public:
    struct UniformBufferObject {
        float deltaTime = 1.0f;
    };

  private:
    size_t particle_number_;

    const std::string comp_shader_path = std::format("{}/particle.comp.spv", PARTICLE_SHADER_DIR);

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

    void updateUniformBuffer(uint32_t frameIndex, float deltaTime);

    VklModelTemplate<Particle, TriangleIndex, VklBox2D> &model;
    std::vector<std::unique_ptr<VklBuffer>> uniformBuffers_;

    void recordComputeCommandBuffer(size_t frameIndex);

  public:
    std::vector<VkSemaphore> computeFinishedSemaphores;
    std::vector<VkFence> computeInFlightFences;

    ParticleSimulationSystem(VklDevice &device, VklModelTemplate<Particle, TriangleIndex, VklBox2D> &model,
                             size_t particle_number = 1024);

    ParticleSimulationSystem(const ParticleSimulationSystem &) = delete;
    ParticleSimulationSystem operator=(const ParticleSimulationSystem &) = delete;

    ~ParticleSimulationSystem();

    void computeSubmission(size_t frameIndex, float deltaTime);
};