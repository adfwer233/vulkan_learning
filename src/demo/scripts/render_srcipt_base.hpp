#pragma once

#include <array>
#include <vector>

#include "vkl/core/vkl_offscreen_renderer.hpp"
#include "vkl/utils/vkl_camera.hpp"
#include "vkl/utils/vkl_curve_model.hpp"
#include "vkl/scene/vkl_geometry_model.hpp"
#include "vkl/system/render_system/param_line_render_system.hpp"

class RenderScriptsBase {
public:
    explicit RenderScriptsBase(VklDevice &device) : device_(device) {}

    void renderResult() {
        auto globalSetLayout = VklDescriptorSetLayout::Builder(device_)
                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
                .build();

        auto globalPool = VklDescriptorPool::Builder(device_)
                .setMaxSets(VklSwapChain::MAX_FRAMES_IN_FLIGHT * 200)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VklSwapChain::MAX_FRAMES_IN_FLIGHT * 200)
                .build();

        ParamLineRenderSystem<VklCurveModel2D::vertex_type> paramCurveRenderSystem(
                device_, renderer_.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(),
                {{std::format("{}/param_curve_shader.vert.spv", SHADER_DIR), VK_SHADER_STAGE_VERTEX_BIT},
                 {std::format("{}/param_curve_shader.frag.spv", SHADER_DIR), VK_SHADER_STAGE_FRAGMENT_BIT}});

        ParamLineRenderSystemPushConstantData paramLineRenderSystemPushConstantData{
                .zoom = 1.0,
                .shift_x = 0.0,
                .shift_y = 0.0
        };
        ParamLineRenderSystemPushConstantList paramLineRenderSystemPushConstantList;
        paramLineRenderSystemPushConstantList.data[0] = paramLineRenderSystemPushConstantData;


        Camera camera({0, 0, 10}, {0, 1, 0});

        std::vector<std::array<float, 2>> control_points_{
                {0.0f, 0.5f}, {0.5f, 0.8f}, {1.0, 0.5f}
        };
        BezierCurve2D curve2D(std::move(control_points_));

        auto commandBuffer = renderer_.beginFrame();
        renderer_.beginSwapChainRenderPass(commandBuffer);

        auto modelBuffer = VklGeometryModelBuffer<BezierCurve2D>::instance();
        auto geometryModel = modelBuffer->getGeometryModel(device_, &curve2D);

        FrameInfo<VklCurveModel2D> curveModelFrameInfo{0,
                                                       0.0f,
                                                       commandBuffer,
                                                       camera,
                                                       &geometryModel->curveMesh->descriptorSets[0],
                                                       *geometryModel->curveMesh};

        paramCurveRenderSystem.renderObject(curveModelFrameInfo, paramLineRenderSystemPushConstantList);

        renderer_.endSwapChainRenderPass(commandBuffer);

        VkFence fence;
        {
            VkFenceCreateInfo createInfo = {
                    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
            };
            assert(vkCreateFence(device_.device(), &createInfo, nullptr, &fence) == VK_SUCCESS);
        }

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo {
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .commandBufferCount = 1,
                .pCommandBuffers = &commandBuffer
        };

        vkQueueSubmit(device_.graphicsQueue(), 1, &submitInfo, fence);
        vkQueueWaitIdle(device_.graphicsQueue());

        vkFreeCommandBuffers(device_.device(), device_.getCommandPool(), 1, &commandBuffer);

        renderer_.endFrame();

        vkDeviceWaitIdle(device_.device());

        renderer_.exportCurrentImageToPPM();
    }

private:
    VklDevice &device_;
    VklOffscreenRenderer renderer_{device_, 1024, 1024};
};