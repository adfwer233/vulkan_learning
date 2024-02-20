#pragma once

#include "../vkl_frame_info.hpp"
#include "../vkl_graphics_pipeline.hpp"

#include <format>
#include <iostream>
#include <memory>
#include <optional>

#ifndef SHADER_DIR
#define SHADER_DIR "./shader/"
#endif

class SimpleRenderSystem {
  private:
    const std::string vertex_shader_path = std::format("{}/first_triangle_shader.vert.spv", SHADER_DIR);
    const std::string fragment_shader_path = std::format("{}/first_triangle_shader.frag.spv", SHADER_DIR);

    VklDevice &device_;
    VkPipelineLayout pipelineLayout_;

    void createPipelineLayout();
    void createPipeline(VkRenderPass renderPass);

  public:
    std::unique_ptr<VklGraphicsPipeline> pipeline_;
    SimpleRenderSystem(VklDevice &device, VkRenderPass renderPass);

    SimpleRenderSystem(const SimpleRenderSystem &) = delete;
    SimpleRenderSystem operator=(const SimpleRenderSystem &) = delete;

    ~SimpleRenderSystem();

    void renderObject();
};