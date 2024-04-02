#include "simple_render_system.hpp"

struct WireFramePipelineModifier {
    static void modifyPipeline(PipelineConfigInfo &configInfo) {
        configInfo.rasterizationInfo.polygonMode = VkPolygonMode::VK_POLYGON_MODE_LINE;
    }
};

template <VklVertexType VertexType>
using SimpleWireFrameRenderSystem =
    SimpleRenderSystem<VertexType, SimplePushConstantInfoList, WireFramePipelineModifier>;