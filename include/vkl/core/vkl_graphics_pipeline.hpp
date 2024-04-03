#pragma once

#include "string"
#include "vkl_pipeline.hpp"

#include "vkl/templates/vkl_concept.hpp"

#include "vkl/core/vkl_shader_module.hpp"

template <VklVertexType VertexType> class VklGraphicsPipeline : public VklPipeline {
  private:
    std::vector<VkShaderModule> shaderModules;

    void createGraphicsPipeline(const PipelineConfigInfo &configInfo, std::vector<VklShaderModuleInfo> infos);

  public:
    VkPipeline graphicsPipeline_{};

    VklGraphicsPipeline(VklDevice &device, std::vector<VklShaderModuleInfo> shaderInfos,
                        const PipelineConfigInfo &configInfo);

    ~VklGraphicsPipeline();

    VklGraphicsPipeline(const VklGraphicsPipeline &) = delete;
    VklGraphicsPipeline &operator=(const VklGraphicsPipeline &) = delete;

    void bind(VkCommandBuffer commandBuffer);

    static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);
};

#include "vkl_graphics_pipeline.hpp.impl"