#pragma once

#include "simple_render_system.hpp"

struct NormalRenderPipelineModifier {
    static void modifyPipeline(PipelineConfigInfo &configInfo) {
        // configInfo.inputAssemblyInfo.topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    }
};

struct NormalRenderSystemPushConstantData {
    alignas(4) float normalStrength;
    alignas(16) glm::vec3 normalColor;

    static VkShaderStageFlags getStageFlags() {
        return VK_SHADER_STAGE_GEOMETRY_BIT;
    };
};

using NormalRenderSystemPushConstantDataList = VklPushConstantInfoList<NormalRenderSystemPushConstantData>;

template <uint32_t Subpass = 0>
using NormalRenderSystem =
    SimpleRenderSystem<Subpass, NormalRenderSystemPushConstantDataList, NormalRenderPipelineModifier>;