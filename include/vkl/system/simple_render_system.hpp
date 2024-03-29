#pragma once

#include "vkl/core/vkl_frame_info.hpp"
#include "vkl/core/vkl_graphics_pipeline.hpp"

#include <format>
#include <iostream>
#include <memory>
#include <optional>

#include "vkl/templates/vkl_concept.hpp"

#ifndef SHADER_DIR
#define SHADER_DIR "./shader/"
#endif

template <typename T>
concept VklPipelineModifierType =
    requires(PipelineConfigInfo &pipelineConfigInfo) { T::modifyPipeline(pipelineConfigInfo); };

struct NullPipelineModifier {
    static void modifyPipeline(PipelineConfigInfo &configInfo) {
    }
};

template <VklVertexType VertexType, VklPipelineModifierType PipelineModifierType = NullPipelineModifier>
class SimpleRenderSystem {
  private:
    std::string vertex_shader_path_, fragment_shader_path_, geometry_shader_path_;

    VklDevice &device_;

    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

  public:
    std::unique_ptr<VklGraphicsPipeline<VertexType>> pipeline_;
    VkPipelineLayout pipelineLayout_{};
    SimpleRenderSystem(VklDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout,
                       std::string vertex_shader_path = std::format("{}/simple_shader.vert.spv", SHADER_DIR),
                       std::string fragment_shader_path = std::format("{}/simple_shader.frag.spv", SHADER_DIR));

    SimpleRenderSystem(VklDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout,
                       std::string vertex_shader_path,
                       std::string fragment_shader_path,
                       std::string geometry_shader_path);

    SimpleRenderSystem(const SimpleRenderSystem &) = delete;
    SimpleRenderSystem operator=(const SimpleRenderSystem &) = delete;

    ~SimpleRenderSystem();

    template <VklRenderable ModelType> void renderObject(FrameInfo<ModelType> &frameInfo);
};

#include "simple_render_system.hpp.impl"