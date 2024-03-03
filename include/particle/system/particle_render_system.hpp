#pragma once

#include <string>
#include <format>

#include "vkl/vkl_frame_info.hpp"
#include "vkl/vkl_graphics_pipeline.hpp"

#ifndef PARTICLE_SHADER_DIR
#define PARTICLE_SHADER_DIR "./shader/"
#endif

class ParticleRenderSystem {
private:
    const std::string vertex_shader_path = std::format("{}/particle.vert.spv", PARTICLE_SHADER_DIR);
    const std::string fragment_shader_path = std::format("{}/particle.frag.spv", PARTICLE_SHADER_DIR);

    VklDevice &device_;

    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

public:
    std::unique_ptr<VklGraphicsPipeline> pipeline_;
    VkPipelineLayout pipelineLayout_;

    ParticleRenderSystem(VklDevice &device_, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);

    ParticleRenderSystem(const ParticleRenderSystem &) = delete;
    ParticleRenderSystem operator=(const ParticleRenderSystem &) = delete;

    ~ParticleRenderSystem();

    void renderObject(FrameInfo &frameInfo);
};