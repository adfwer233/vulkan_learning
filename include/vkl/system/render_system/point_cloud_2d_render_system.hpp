#pragma once

#include "simple_render_system.hpp"

struct PointCloud2DPipelineModifier {
    static void modifyPipeline(PipelineConfigInfo &configInfo) {
        configInfo.inputAssemblyInfo.topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        configInfo.depthStencilInfo.depthTestEnable = VK_FALSE;
    }
};

struct PointCloud2DRenderSystemPushConstantData {
    float zoom, shift_x, shift_y;

    static VkShaderStageFlags getStageFlags() {
        return VK_SHADER_STAGE_VERTEX_BIT;
    };
};

using PointCloud2DRenderSystemPushConstantList = VklPushConstantInfoList<PointCloud2DRenderSystemPushConstantData>;

using PointCloud2DRenderSystem = SimpleRenderSystem<VklVertex2D, PointCloud2DRenderSystemPushConstantList, PointCloud2DPipelineModifier>;