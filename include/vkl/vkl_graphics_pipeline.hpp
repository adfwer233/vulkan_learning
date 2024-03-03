#pragma once

#include "string"
#include "vkl_device.hpp"

#include "vkl/templates/vkl_concept.hpp"

struct PipelineConfigInfo {
    PipelineConfigInfo() = default;
    PipelineConfigInfo(const PipelineConfigInfo &) = delete;
    PipelineConfigInfo &operator=(const PipelineConfigInfo &) = delete;

    std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    std::vector<VkDynamicState> dynamicStateEnables;
    VkPipelineDynamicStateCreateInfo dynamicStateInfo;
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
};

template<VklVertexType VertexType>
class VklGraphicsPipeline {
  private:
    VklDevice &device_;
    VkShaderModule vertShaderModule_{};
    VkShaderModule fragShaderModule_{};

    void createGraphicsPipeline(const std::string &vertFilepath, const std::string &fragFilepath,
                                const PipelineConfigInfo &configInfo);

    void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

    std::vector<char> readFile(const std::string &filepath);

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