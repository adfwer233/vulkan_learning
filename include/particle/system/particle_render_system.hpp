#pragma once

#include <format>
#include <string>

#include "vkl/core/vkl_frame_info.hpp"
#include "vkl/core/vkl_graphics_pipeline.hpp"
#include "vkl/system/render_system/simple_render_system.hpp"

#include "particle/particle.hpp"
#include "vkl/templates/vkl_concept.hpp"

#ifndef PARTICLE_SHADER_DIR
#define PARTICLE_SHADER_DIR "./shader/"
#endif

struct ParticleRenderPipelineModifier {
    static void modifyPipeline(PipelineConfigInfo &configInfo) {
        configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        configInfo.depthStencilInfo.depthTestEnable = VK_FALSE;
        configInfo.colorBlendAttachment.blendEnable = VK_TRUE;
    }
};

using ParticleRenderSystem = SimpleRenderSystem<Particle, 0, SimplePushConstantInfoList, ParticleRenderPipelineModifier>;