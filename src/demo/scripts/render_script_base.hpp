#pragma once

#include <array>
#include <vector>

#include "vkl/core/vkl_offscreen_renderer.hpp"
#include "vkl/utils/vkl_camera.hpp"
#include "vkl/utils/vkl_curve_model.hpp"
#include "vkl/scene/vkl_geometry_model.hpp"
#include "vkl/system/render_system/param_line_render_system.hpp"
#include "vkl/system/render_system/simple_render_system_2d.hpp"

class RenderScriptsBase {
public:
    explicit RenderScriptsBase(VklDevice &device) : device_(device) {}

    void renderResult() {
        using VklModel2D = VklModelTemplate<VklVertex2D, TriangleIndex, VklBox2D>;

        auto globalSetLayout = VklDescriptorSetLayout::Builder(device_)
                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
                .build();

        auto globalPool = VklDescriptorPool::Builder(device_)
                .setMaxSets(VklSwapChain::MAX_FRAMES_IN_FLIGHT * 200)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VklSwapChain::MAX_FRAMES_IN_FLIGHT * 200)
                .build();

        SimpleRenderSystem2D<VklModel2D::vertex_type> renderSystem(
                device_, renderer_.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout(),
                {{std::format("{}/simple_shader_2d.vert.spv", SHADER_DIR), VK_SHADER_STAGE_VERTEX_BIT},
                 {std::format("{}/simple_shader_2d.frag.spv", SHADER_DIR), VK_SHADER_STAGE_FRAGMENT_BIT}});

        ParamLineRenderSystem<VklCurveModel2D::vertex_type, 1> paramCurveRenderSystem(
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

        std::vector<std::unique_ptr<BezierCurve2D>> curves;

        std::vector<std::array<float, 2>> control_points1{
                {1.0f, 0.8f}, {0.5f, 0.3f}, {0.0, 0.8f}
        };
        curves.push_back(std::move(std::make_unique<BezierCurve2D>(std::move(control_points1))));

        std::vector<std::array<float, 2>> control_points2{
                {0.0f, 0.2f}, {0.5f, 0.7f}, {1.0, 0.2f}
        };
        curves.push_back(std::move(std::make_unique<BezierCurve2D>(std::move(control_points2))));


        VklModel2D::BuilderFromImmediateData builder;

        int n = 1024;
        int m = 1024;

        float delta_u = 1.0f / n;
        float delta_v = 1.0f / m;

        glm::vec3 lower_color{0.0f, 0.0f, 1.0f};
        glm::vec3 upper_color(1.0f, 0.0f, 0.0f);

        for (int i = 0; i <= m; i++) {
            for (int j = 0; j <= n; j++) {
                decltype(builder.vertices)::value_type vertex;
                vertex.position = {i * delta_v, j * delta_u};
                vertex.color = {0.0f, 1.0f, 0.0f};

                double winding_number = 0.0;
                for (auto &curve: curves) {
                    winding_number += curve->winding_number_u_periodic(vertex.position);
                }

                double pi = std::numbers::pi;
                double coeff = (winding_number + 2 * pi) / (4 * pi);
                vertex.color = lower_color + (upper_color - lower_color) * float((winding_number + 2 * pi) / (4 * pi));

                if (coeff > 0.75) {
                    // vertex.color = {0.0f, 1.0f, 0.5f};
                } else {
                    vertex.color = {1.0f, 1.0f, 1.0f};
                }

                builder.vertices.push_back(vertex);
            }
        }

        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                auto idx1 = i * (n + 1) + j;
                auto idx2 = i * (n + 1) + j + 1;
                auto idx3 = (i + 1) * (n + 1) + j;
                auto idx4 = (i + 1) * (n + 1) + j + 1;

                decltype(builder.indices)::value_type primitive_idx1, primitive_idx2;
                primitive_idx1.i = idx1;
                primitive_idx1.j = idx2;
                primitive_idx1.k = idx4;

                primitive_idx2.i = idx1;
                primitive_idx2.j = idx4;
                primitive_idx2.k = idx3;

                builder.indices.push_back(primitive_idx1);
                builder.indices.push_back(primitive_idx2);
            }
        }

        VklModel2D grid(device_, builder);
        grid.allocDescriptorSets(*globalSetLayout, *globalPool);

        auto commandBuffer = renderer_.beginFrame();
        renderer_.beginSwapChainRenderPass(commandBuffer);

        auto endFrameRender = [&]() {
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
            vkDeviceWaitIdle(device_.device());
        };

        FrameInfo<VklModel2D> gridModelFrameInfo {
            .frameIndex = 0,
            .frameTime = 0.0f,
            .commandBuffer = commandBuffer,
            .camera = camera,
            .pGlobalDescriptorSet = &grid.descriptorSets[0],
            .model = grid
        };

        renderSystem.renderObject(gridModelFrameInfo);

        vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);

        for (auto &curve: curves) {
            auto modelBuffer = VklGeometryModelBuffer<BezierCurve2D>::instance();
            auto geometryModel = modelBuffer->getGeometryModel(device_, curve.get());

            FrameInfo<VklCurveModel2D> curveModelFrameInfo{0,
                                                           0.0f,
                                                           commandBuffer,
                                                           camera,
                                                           &geometryModel->curveMesh->descriptorSets[0],
                                                           *geometryModel->curveMesh};

            paramCurveRenderSystem.renderObject(curveModelFrameInfo, paramLineRenderSystemPushConstantList);
        }

        endFrameRender();

        renderer_.exportCurrentImageToPPM();
    }

private:
    VklDevice &device_;
    VklOffscreenRenderer renderer_{device_, 1024, 1024, 2};
};