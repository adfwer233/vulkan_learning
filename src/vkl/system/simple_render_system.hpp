#pragma once

#include "../vkl_graphics_pipeline.hpp"
#include "../vkl_frame_info.hpp"

#include <memory>
#include <optional>

class SimpleRenderSystem {
  private:
    VklDevice &device_;
    std::unique_ptr<VklGraphicsPipeline> pipeline_;
    VkPipelineLayout pipelineLayout_;

    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

  public:
    SimpleRenderSystem(VklDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout);

    SimpleRenderSystem(const SimpleRenderSystem &) = delete;
    SimpleRenderSystem operator=(const SimpleRenderSystem &) = delete;

    void renderObject();
};