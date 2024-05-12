#pragma once

#include "simple_render_system.hpp"

struct SimpleRender2DPipelineModifier {
    static void modifyPipeline(PipelineConfigInfo &configInfo) {
        configInfo.depthStencilInfo.depthTestEnable = VK_FALSE;
    }
};

template <VklVertexType VertexType, uint32_t Subpass = 0>
using SimpleRenderSystem2D = SimpleRenderSystem<VertexType, Subpass, SimplePushConstantInfoList , SimpleRender2DPipelineModifier>;