#pragma once

#include "simple_render_system.hpp"

struct NormalRenderPipelineModifier {
    static void modifyPipeline(PipelineConfigInfo &configInfo) {
//        configInfo.inputAssemblyInfo.topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    }
};

template <VklVertexType VertexType> using NormalRenderSystem = SimpleRenderSystem<VertexType, SimplePushConstantInfoList, NormalRenderPipelineModifier>;