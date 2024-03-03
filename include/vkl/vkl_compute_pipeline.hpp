#pragma once

#include "vkl_pipeline.hpp"

#include <string>

struct ComputePipelineConfigInfo {
    VkDescriptorSetLayout &computeDescriptorSetLayout;
    VkPipelineLayout computePipelineLayout;
};

class VklComputePipeline : public VklPipeline {
  private:
    VkShaderModule computeShaderModule_{};

    void createComputePipeline(const std::string &compFilePath, const ComputePipelineConfigInfo &configInfo);

  public:
    VkPipeline computePipeline_;

    VklComputePipeline(VklDevice &device, const std::string &compFilePath, const ComputePipelineConfigInfo &configInfo);

    ~VklComputePipeline();

    VklComputePipeline(const VklComputePipeline &) = delete;
    VklComputePipeline &operator=(const VklComputePipeline &) = delete;
};
