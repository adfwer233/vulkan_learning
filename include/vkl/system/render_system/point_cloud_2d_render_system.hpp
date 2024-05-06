#pragma once

#include "simple_render_system.hpp"

struct PointCloud2DPipelineModifier {
    static void modifyPipeline(PipelineConfigInfo &configInfo) {
        configInfo.inputAssemblyInfo.topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        configInfo.depthStencilInfo.depthTestEnable = VK_FALSE;
    }
};

using PointCloud2DRenderSystem = SimpleRenderSystem<VklVertex2D, SimplePushConstantInfoList, PointCloud2DPipelineModifier>;