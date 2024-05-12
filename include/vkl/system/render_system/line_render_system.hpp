#pragma once

#include "simple_render_system.hpp"

struct LinePipelineModifier {
    static void modifyPipeline(PipelineConfigInfo &configInfo) {
        configInfo.inputAssemblyInfo.topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    }
};

template <VklVertexType VertexType, uint32_t Subpass = 0>
using LineRenderSystem = SimpleRenderSystem<VertexType, Subpass, SimplePushConstantInfoList, LinePipelineModifier>;