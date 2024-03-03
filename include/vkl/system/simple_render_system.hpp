#pragma once

#include "vkl/vkl_frame_info.hpp"
#include "vkl/vkl_graphics_pipeline.hpp"

#include <format>
#include <iostream>
#include <memory>
#include <optional>

#include "vkl/templates/vkl_concept.hpp"

#ifndef SHADER_DIR
#define SHADER_DIR "./shader/"
#endif

template<VklVertexType VertexType>
class SimpleRenderSystem {
  private:
    const std::string vertex_shader_path = std::format("{}/simple_shader.vert.spv", SHADER_DIR);
    const std::string fragment_shader_path = std::format("{}/simple_shader.frag.spv", SHADER_DIR);

    VklDevice &device_;

    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

  public:
    std::unique_ptr<VklGraphicsPipeline<VertexType>> pipeline_;
    VkPipelineLayout pipelineLayout_{};
    SimpleRenderSystem(VklDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);

    SimpleRenderSystem(const SimpleRenderSystem &) = delete;
    SimpleRenderSystem operator=(const SimpleRenderSystem &) = delete;

    ~SimpleRenderSystem();

    void renderObject(FrameInfo &frameInfo);
};

#include "simple_render_system.hpp.impl"