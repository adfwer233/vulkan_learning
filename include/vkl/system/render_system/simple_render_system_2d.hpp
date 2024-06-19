#pragma once

#include "simple_render_system.hpp"

struct SimpleRender2DPipelineModifier {
    static void modifyPipeline(PipelineConfigInfo &configInfo) {
        configInfo.depthStencilInfo.depthTestEnable = VK_FALSE;
    }
};

template <uint32_t Subpass = 0>
using SimpleRenderSystem2D = SimpleRenderSystem<Subpass, SimplePushConstantInfoList , SimpleRender2DPipelineModifier>;