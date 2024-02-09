#include "simple_render_system.hpp"

#include <stdexcept>

SimpleRenderSystem::SimpleRenderSystem(VklDevice &device, VkRenderPass renderPass,
                                       VkDescriptorSetLayout descriptorSetLayout) : device_(device){
    createPipelineLayout(descriptorSetLayout);
    createPipeline(renderPass);
}

void SimpleRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(device_.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout_) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
    PipelineConfigInfo pipelineConfigInfo{};

    VklGraphicsPipeline::defaultPipelineConfigInfo(pipelineConfigInfo);
    pipelineConfigInfo.renderPass = renderPass;
    pipelineConfigInfo.pipelineLayout = pipelineLayout_;
    pipeline_ = std::make_unique<VklGraphicsPipeline>(device_, "vertexshader", "fragshader", pipelineConfigInfo);
}