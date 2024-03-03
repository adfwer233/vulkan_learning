#include "vkl/vkl_compute_pipeline.hpp"

#include <fstream>

void VklComputePipeline::createComputePipeline(const std::string &compFilePath,
                                               const ComputePipelineConfigInfo &configInfo) {
    auto computeShaderCode = readFile(compFilePath);
    createShaderModule(computeShaderCode, &computeShaderModule_);

    VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
    computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    computeShaderStageInfo.module = computeShaderModule_;
    computeShaderStageInfo.pName = "main";

    VkComputePipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.layout = configInfo.computePipelineLayout;
    pipelineCreateInfo.stage = computeShaderStageInfo;

    if (vkCreateComputePipelines(device_.device(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr,
                                 &computePipeline_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute pipeline");
    }
}

VklComputePipeline::VklComputePipeline(VklDevice &device, const std::string &compFilePath,
                                       const ComputePipelineConfigInfo &configInfo)
    : VklPipeline(device) {
    createComputePipeline(compFilePath, configInfo);
}

VklComputePipeline::~VklComputePipeline() {
    vkDestroyShaderModule(device_.device(), computeShaderModule_, nullptr);
    vkDestroyPipeline(device_.device(), computePipeline_, nullptr);
}
