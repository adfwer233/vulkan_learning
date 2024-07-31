#pragma once

#include "simple_render_system.hpp"

struct PointCloud2DPipelineModifier {
    static void modifyPipeline(PipelineConfigInfo &configInfo) {
        configInfo.inputAssemblyInfo.topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        configInfo.depthStencilInfo.depthTestEnable = VK_FALSE;
        configInfo.colorBlendAttachment.blendEnable = VK_TRUE;
    }
};

struct PointCloud2DRenderSystemPushConstantData {
    float zoom, shift_x, shift_y;

    static VkShaderStageFlags getStageFlags() {
        return VK_SHADER_STAGE_VERTEX_BIT;
    };
};

using PointCloud2DRenderSystemPushConstantList = VklPushConstantInfoList<PointCloud2DRenderSystemPushConstantData>;

template <uint32_t Subpass = 0>
using PointCloud2DRenderSystem =
    SimpleRenderSystem<Subpass, PointCloud2DRenderSystemPushConstantList, PointCloud2DPipelineModifier>;