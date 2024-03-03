#pragma once

#include <string>
#include <format>

#include "vkl/vkl_frame_info.hpp"
#include "vkl/vkl_graphics_pipeline.hpp"

#include "particle/particle.hpp"
#include "vkl/templates/vkl_concept.hpp"

#include "vkl/vkl_compute_pipeline.hpp"

#ifndef PARTICLE_SHADER_DIR
#define PARTICLE_SHADER_DIR "./shader/"
#endif

class ParticleSimulationSystem {
private:
    const std::string comp_shader_path = std::format("{}/particle.comp.spv", PARTICLE_SHADER_DIR);

    VklDevice &device_;

    std::unique_ptr<VklComputePipeline> pipeline_;
    VkPipelineLayout pipelineLayout_;

    VkDescriptorSetLayout computeDescriptorSetLayout;

    std::vector<VkCommandBuffer> computeCommandBuffers;

    void createComputeDescriptorSetLayout();
    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffer();

public:

    ParticleSimulationSystem(VklDevice &device_, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);

    ParticleSimulationSystem(const ParticleSimulationSystem &) = delete;
    ParticleSimulationSystem operator=(const ParticleSimulationSystem &) = delete;

    ~ParticleSimulationSystem();

    void PerformComputation();
};