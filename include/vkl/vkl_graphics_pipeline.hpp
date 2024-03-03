#pragma once

#include "string"
#include "vkl_pipeline.hpp"

#include "vkl/templates/vkl_concept.hpp"

template<VklVertexType VertexType>
class VklGraphicsPipeline: public VklPipeline{
  private:
    VkShaderModule vertShaderModule_{};
    VkShaderModule fragShaderModule_{};

    void createGraphicsPipeline(const std::string &vertFilepath, const std::string &fragFilepath,
                                const PipelineConfigInfo &configInfo);

  public:
    VkPipeline graphicsPipeline_{};

    VklGraphicsPipeline(VklDevice &device, const std::string &vertFilepath, const std::string &fragFilepath,
                        const PipelineConfigInfo &configInfo);

    ~VklGraphicsPipeline();

    VklGraphicsPipeline(const VklGraphicsPipeline &) = delete;
    VklGraphicsPipeline &operator=(const VklGraphicsPipeline &) = delete;

    void bind(VkCommandBuffer commandBuffer);

    static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);
};

#include "templates/vkl_graphics_pipeline.hpp.impl"