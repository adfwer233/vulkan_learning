#pragma once

#include "simple_render_system.hpp"

struct LinePipelineModifier {
    static void modifyPipeline(PipelineConfigInfo &configInfo) {
        configInfo.inputAssemblyInfo.topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    }
};

template <VklVertexType VertexType> using LineRenderSystem = SimpleRenderSystem<VertexType, SimplePushConstantInfoList, LinePipelineModifier>;